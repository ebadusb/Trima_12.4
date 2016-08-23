/*
 * Pool allocator for small memory blocks.  UGL tends to allocate many small
 * chunks of memory.  Using a pool allocator for this significantly improves
 * performance and reduces memory fragmentation associated with long run times.
 *
 */

#include <vxWorks.h>
#include <stdio.h>

#include "trima_datalog.h"
#include "cgui_pool_allocator.hpp"
#include "error.h"
#include "ugl/uglmem.h"
#include "filenames.h"


// Setting this number defines how many pools used.
#define MEMPOOL_NUM_POOLS 4

// Here you can tweak the number and size of each pool layer.
// Layer 0
#define MEMPOOL_BLOCK_SIZE_LAYER_0 64
#define MEMPOOL_NUM_BLOCKS_LAYER_0 4500
// Layer 1
#define MEMPOOL_BLOCK_SIZE_LAYER_1 256
#define MEMPOOL_NUM_BLOCKS_LAYER_1 900
// Layer 2
#define MEMPOOL_BLOCK_SIZE_LAYER_2 1024
#define MEMPOOL_NUM_BLOCKS_LAYER_2 350
// Layer 3
#define MEMPOOL_BLOCK_SIZE_LAYER_3 4096
#define MEMPOOL_NUM_BLOCKS_LAYER_3 50

// Given a pool index, return the block size.
#define MEMPOOL_BLOCK_SIZE(__MEMPOOL_LAYER__) \
   (((__MEMPOOL_LAYER__) < 0 || (__MEMPOOL_LAYER__) >= (MEMPOOL_NUM_POOLS)) ? \
    0 : ((__MEMPOOL_LAYER__ == 0) ? MEMPOOL_BLOCK_SIZE_LAYER_0 : \
         (__MEMPOOL_LAYER__ == 1) ? MEMPOOL_BLOCK_SIZE_LAYER_1 : \
         (__MEMPOOL_LAYER__ == 2) ? MEMPOOL_BLOCK_SIZE_LAYER_2 : \
         (__MEMPOOL_LAYER__ == 3) ? MEMPOOL_BLOCK_SIZE_LAYER_3 : 0))

// Given a pool index, return the number of blocks.
#define MEMPOOL_NUM_BLOCKS(__MEMPOOL_LAYER__) \
   (((__MEMPOOL_LAYER__) < 0 || (__MEMPOOL_LAYER__) >= (MEMPOOL_NUM_POOLS)) ? \
    0 : (   (__MEMPOOL_LAYER__ == 0) ? MEMPOOL_NUM_BLOCKS_LAYER_0 : \
            (__MEMPOOL_LAYER__ == 1) ? MEMPOOL_NUM_BLOCKS_LAYER_1 : \
            (__MEMPOOL_LAYER__ == 2) ? MEMPOOL_NUM_BLOCKS_LAYER_2 : \
            (__MEMPOOL_LAYER__ == 3) ? MEMPOOL_NUM_BLOCKS_LAYER_3 : 0))

// Return the biggest block size being used
#define MEMPOOL_MAX_BLOCK_SIZE (MEMPOOL_BLOCK_SIZE(MEMPOOL_NUM_POOLS - 1))



#define MEM_LOG_PERIOD 100000

// Define this variable for debugging (compiled out for efficiency)
#define MEM_LOGGING_ON

// Memory pool with a specified block size.  Note: block sizes must be positive.
class CGUIMemPool
{
public:

   //
   CGUIMemPool(const size_t blockSize, const unsigned short blockCount);

   virtual ~CGUIMemPool();

   bool containsAddr (const void* blockAddr) const;

   // Returns the block size this pool was created with
   size_t blockSize () const { return _blockSize; }

   // If the memory is held by this pool, return our block size; otherwise return 0.
   size_t blockSize (const void* blockAddr) const
   {
      if (!containsAddr(blockAddr)) return 0;
      return _blockSize;
   }

   void* alloc (const size_t size);
   bool  free (const void* blockAddr);

   size_t numBlocks () const { return _blockCount; }

#ifdef MEM_LOGGING_ON
   unsigned short highWater () const { return _highWater; }
   unsigned short numAllocated () const { return _numAllocated; }
   long           totalAllocCalls () const { return _totalAllocCalls; }
   long           numBufferFull () const { return _numBufferFull; }
   void           bufferFull () { _numBufferFull++; }
#endif // #ifdef MEM_LOGGING_ON

private:
   SEM_ID             _sem;

   const size_t       _blockSize;
   const unsigned int _blockCount;

   unsigned short*    _freeArray;
   unsigned short     _freeStart;
   unsigned short     _freeEnd;

#ifdef MEM_LOGGING_ON
   mutable unsigned short _highWater;
   mutable unsigned short _numAllocated;
   mutable long           _totalAllocCalls;
   mutable long           _numBufferFull;
#endif // #ifdef MEM_LOGGING_ON

   const void* _start;
};


CGUIMemPool::CGUIMemPool(const size_t blockSize, const unsigned short blockCount) :
#ifdef MEM_LOGGING_ON
   _highWater(0),
   _numAllocated(0),
   _totalAllocCalls(0),
   _numBufferFull(0),
#endif // #ifdef MEM_LOGGING_ON
   _blockSize(blockSize),
   _blockCount(blockCount),
   _freeStart(0),
   _freeEnd(blockCount),
   _freeArray(NULL),
   _start(malloc(blockSize * (size_t)blockCount))

{
   // block size of zero not allowed.
   if (_blockSize == 0)
      _FATAL_ERROR(__FILE__, __LINE__, "CGUIMemPool() malloc failure");


   _freeArray = (unsigned short*)malloc(((size_t)blockCount + 1) * sizeof(unsigned short));

   if ( !_freeArray )
   {
      _FATAL_ERROR(__FILE__, __LINE__, "CGUIMemPool() malloc failure");
      return;
   }

   for ( int i = 0; i<blockCount; i++ )
      _freeArray[i] = i;

   _sem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
}


CGUIMemPool::~CGUIMemPool()
{
   //  We really can't just delete a memory pool, because it would
   //  leave a whole bunch bad pointers all over the place.
   DataLog (log_level_gui_mempool_error) << "CGUIMempool destructor not supported." << endmsg;
   _FATAL_ERROR(__FILE__, __LINE__, "~CGUIMemPool not supported");
}


inline bool CGUIMemPool::containsAddr (const void* blockAddr) const
{
   if ( blockAddr >= _start )
   {
      size_t offset = (char*)blockAddr - (char*)_start;

      if ( (offset / _blockSize) < _blockCount )
      {
         if ( !(offset % _blockSize) ) return true;
         else
         {
            DataLog (log_level_gui_mempool_error) << "Bad pool address: [addr="
                                                  << blockAddr << "; poolStart=" << _start << "; blockSize="
                                                  << _blockSize << "; offset=" << offset << "; modulus="
                                                  << (offset % _blockSize) << "]" << endmsg;

            _FATAL_ERROR(__FILE__, __LINE__, "bad pointer in pool space");
         }
      }
   }

   return false;
}


inline void* CGUIMemPool::alloc (const size_t size)
{
   void* result = NULL;

   if ( size <= _blockSize )
   {
      semTake(_sem, WAIT_FOREVER);
      {
         if ( _freeStart != _freeEnd )
         {
            result = static_cast<void*>( (char*)_start + (_blockSize * _freeArray[_freeStart++]) );

            if ( _freeStart > _blockCount ) _freeStart = 0;

#ifdef MEM_LOGGING_ON
            _numAllocated++;
            _totalAllocCalls++;
            _highWater = max(_highWater, _numAllocated);
#endif      // #ifdef MEM_LOGGING_ON
         }
      }
      semGive(_sem);
   }

   return result;
}


inline bool CGUIMemPool::free (const void* blockAddr)
{
   if (!containsAddr(blockAddr)) return false;

   size_t offset = (char*)blockAddr - (char*)_start;
   int    index  = offset / _blockSize;

   semTake(_sem, WAIT_FOREVER);
   {
      _freeArray[_freeEnd++] = index;
      if ( _freeEnd > _blockCount ) _freeEnd = 0;

#ifdef MEM_LOGGING_ON
      _numAllocated--;
#endif // #ifdef MEM_LOGGING_ON
   }
   semGive(_sem);

   return true;
}


//////////////////////////////////////////////////////////////////////



class TrimaGuiMemPool
{
public:
   TrimaGuiMemPool();

   void* trima_gui_alloc (const unsigned int size);
   bool  trima_gui_free (void* blockAddr);

   // If the memory is held by this pool, return our block size; otherwise return 0.
   size_t blockSize (const void* blockAddr) const;

   void log () const;    // no-op if MEM_LOGGING_ON isn't defined.


private:
   CGUIMemPool* _poolLayers[MEMPOOL_NUM_POOLS];

#ifdef MEM_LOGGING_ON
   mutable int _actionNum;
#endif // #ifdef MEM_LOGGING_ON

};

TrimaGuiMemPool::TrimaGuiMemPool()
#ifdef MEM_LOGGING_ON
   : _actionNum(0)
#endif // #ifdef MEM_LOGGING_ON
{
   unsigned long totalSize = 0;

   for (int poolIndex = 0; poolIndex < MEMPOOL_NUM_POOLS; ++poolIndex)
   {
      size_t         newBlockSize = MEMPOOL_BLOCK_SIZE   (poolIndex);
      unsigned short newNumBlocks = MEMPOOL_NUM_BLOCKS   (poolIndex);
      totalSize             += (newBlockSize * newNumBlocks);

      _poolLayers[poolIndex] = new CGUIMemPool(newBlockSize, newNumBlocks);

      DataLog (log_level_gui_mempool_info) << "GUI memory pool layer " << poolIndex
                                           << " created: [blockSize=" << newBlockSize
                                           << " ; numBlocks=" << newNumBlocks
                                           << "; poolSize=" << newBlockSize * newNumBlocks
                                           << "]" << endmsg;
   }

   DataLog (log_level_gui_mempool_info) << "Total memory pool size = " << totalSize
                                        << "; maxBlockSize=" << MEMPOOL_MAX_BLOCK_SIZE << endmsg;
}


inline void* TrimaGuiMemPool::trima_gui_alloc (const unsigned int size)
{
   void* result = NULL;

   // Loop through the pool layers until we get a result (or run out of layers).
   for (unsigned int index = 0; index < MEMPOOL_NUM_POOLS; ++index)
   {
      result = _poolLayers[index]->alloc(size);

      if (result)
         break;
   }

   // If we didn't get anything out of the pool layers, to a traditional malloc.
   if (!result)
   {
      for (unsigned int index = 0; index < MEMPOOL_NUM_POOLS; ++index)
      {
         if (size <= _poolLayers[index]->blockSize())
         {
            if (_poolLayers[index]->numAllocated() == _poolLayers[index]->numBlocks())
            {
               _poolLayers[index]->bufferFull();
            }
            else
            {
               DataLog (log_level_gui_mempool_error) << "ERROR!  Unable to get pool space for size " << size
                                                     << ".  Block size " << _poolLayers[index]->blockSize()
                                                     << " has " << _poolLayers[index]->numAllocated()
                                                     << " blocks allocated but a free size of "
                                                     << _poolLayers[index]->numBlocks() -
                  _poolLayers[index]->numAllocated()
                                                     << endmsg;
            }
            break;
         }
      }

      result = malloc(size);
   }

   // If we're still empty handed, that's a problem.
   if (!result)
   {
      DataLog (log_level_gui_mempool_error) << "Unable to allocate memory (requested block: " << size << ")" << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "CGUIMemPool unable to allocate memory.");
      return NULL;
   }
   return result;
}

inline bool TrimaGuiMemPool::trima_gui_free (void* blockAddr)
{
   bool result = false;

   // Loop through the pool layers until we get a delete (or run out of layers).
   for (unsigned int index = 0; index < MEMPOOL_NUM_POOLS; ++index)
   {
      result = _poolLayers[index]->free(blockAddr);

      if (result)
         break;
   }

   // If we couldn't find it in any of the pools, we probably did an old-school malloc.
   if ( !result )
      free(blockAddr);

   // We have to assume that the regular free call worked.
   return true;
}


inline size_t TrimaGuiMemPool::blockSize (const void* blockAddr) const
{
   int result = 0;

   // Loop through the pool layers until we get a delete (or run out of layers).
   for (unsigned int index = 0; index < MEMPOOL_NUM_POOLS; ++index)
   {
      result = _poolLayers[index]->blockSize(blockAddr);

      if (result != 0)     // true if we've found the pool containing the memory
         break;            // so exit the loop
   }

   return result;
}


inline void TrimaGuiMemPool::log () const
{
#ifdef MEM_LOGGING_ON
   if (!((++_actionNum) % MEM_LOG_PERIOD))
   {
      // Loop through the pool layers until we get a delete (or run out of layers).
      for (unsigned int index = 0; index < MEMPOOL_NUM_POOLS; ++index)
      {
         DataLog (log_level_gui_mempool_info) << "Pool " << index << "["
                                              << _poolLayers[index]->blockSize()
                                              << "] : curr/high/numblk/totAlloc/NumFull ("
                                              << _poolLayers[index]->numAllocated() << "/"
                                              << _poolLayers[index]->highWater() << "/"
                                              << _poolLayers[index]->numBlocks() << "/"
                                              << _poolLayers[index]->totalAllocCalls()
                                              << "/" << _poolLayers[index]->numBufferFull() << ")"
                                              << endmsg;
      }
   }
#endif // #ifdef MEM_LOGGING_ON
}


//////////////////////////////////////////////////////////////////////

// Singleton access
TrimaGuiMemPool& Pool ()
{
   static TrimaGuiMemPool p;
   return p;
}


static UGL_MEM_POOL_ID cguiMemPoolCreate (void* poolAddress, UGL_SIZE poolSize)
{
   // Dummy routine, since pools aren't used.  Just return a unique ID number for each
   // pool created.
   //
   static int id = 0;
   id += 1;
   return (UGL_MEM_POOL_ID)id;
}

static UGL_STATUS cguiMemPoolDestroy (UGL_MEM_POOL_ID poolId)
{
   // No need to destroy the pool - see cguiMemPoolCreate
   //
   return UGL_STATUS_OK;
}

static void* cguiMemAlloc (UGL_MEM_POOL_ID poolId, UGL_SIZE size)
{
   void* pMem = NULL;

   if (size > MEMPOOL_MAX_BLOCK_SIZE)
      pMem = malloc(size);
   else
      pMem = Pool().trima_gui_alloc(size);

   if (pMem)
      memset(pMem, 0, size);

#ifdef MEM_LOGGING_ON
   Pool().log();
#endif // #ifdef MEM_LOGGING_ON

   return pMem;
}

static void* cguiMemCalloc (UGL_MEM_POOL_ID poolId,
                            UGL_ORD numItems,
                            UGL_SIZE itemSize)
{
   UGL_SIZE size = numItems * itemSize;

   void*    pMem = NULL;

   // If this won't fit into any of the pools, use calloc, which zeros it out.
   if (size > MEMPOOL_MAX_BLOCK_SIZE)
   {
      pMem = calloc(numItems, itemSize);
   }
   // cguiMemAlloc zeroes this out for us.
   else
      pMem = cguiMemAlloc(poolId, size);

   return pMem;
}

static UGL_STATUS cguiMemFree (void* pMem)
{
   // calling free on a null ptr is a no-op
   if (!pMem)
      return UGL_STATUS_OK;

   Pool().log();

   Pool().trima_gui_free(pMem);

   // Note: our implementation can't fail at freeing memory.
   return UGL_STATUS_OK;
}

static void* cguiMemRealloc (UGL_MEM_POOL_ID poolId,
                             void* pMem,
                             UGL_SIZE size)
{
   void* result = NULL;

   // If we're given a null pointer, just get the requested amount of memory and return.
   if ( pMem == NULL )
   {
      result = cguiMemAlloc(poolId, size);
      return result;
   }


   // Get the block size of the pool containing the existing data
   size_t blockSize = Pool().blockSize(pMem);

   // If blocksize is 0, that means that the chunk wasn't present in any of our pools.
   if ( blockSize == 0 )
   {
      // If the new size is small enough for one of the pools, try to get it in one.
      if (size <= MEMPOOL_MAX_BLOCK_SIZE)
      {
         result = cguiMemAlloc(poolId, size);

         size_t newBlockSize = Pool().blockSize(result);

         // If we found a place in our mempool, we need to memmove from the
         // external-to-mempool location into the mempool
         if (newBlockSize > 0)
         {
            memmove(result, pMem, Pool().blockSize(result));
            free(pMem);
         }
         else
         {
            // Can't fit it into a mempool (This should be rare.  We checked
            // the size, and we know that the chunk could fit, but the pools
            // must be full because this didn't go in).  Since we don't know
            // the size of either of these blocks, so we don't know how much
            // to copy; fall back to the system's realloc.
            free (result);
            result = realloc(pMem, size);
         }
      }
      // The new size is too big for any of the pools.  Use a traditional realloc.
      else
      {
         result = realloc(pMem, size);
      }
      return result;
   }

   // The existing block is in our pool.

   // If the new size is bigger than the block size of the pool the existing
   // data is in...
   if ( size > blockSize )
   {
      // If the block is small enough for one of the pools, try to get it in
      // one.
      if (size <= MEMPOOL_MAX_BLOCK_SIZE)
      {
         result = cguiMemAlloc(poolId, size);

         if (result != pMem)
         {
            memmove(result, pMem, blockSize);
            cguiMemFree(pMem);
         }
      }
      // The new size is too big for any of the pools.  Use
      // a traditional malloc.
      else
      {
         result = malloc(size);
         if (result)
         {
            memmove(result, pMem, blockSize);
            cguiMemFree(pMem);
         }
      }
   }

   // Else, we don't have to do anything.
   return result;
}

void cguiPoolAllocatorInitialize ()
{
   uglMemInterfaceSet((UGL_FPTR)&cguiMemPoolCreate,
                      (UGL_FPTR)&cguiMemPoolDestroy,
                      (UGL_VOIDFPTR)&cguiMemAlloc,
                      (UGL_VOIDFPTR)&cguiMemCalloc,
                      (UGL_VOIDFPTR)&cguiMemRealloc,
                      (UGL_FPTR)&cguiMemFree);
}

/* FORMAT HASH c571af19d7470ef2c9797476ee7f3bf1 */
