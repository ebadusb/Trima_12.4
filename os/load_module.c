/*
 * $Header$
 *
 */

#include <vxWorks.h>
#include <loadLib.h>
#include <vmLib.h>
#include "os/load_module.h"

#if CPU != SIMNT
static void* allocDataSegment (unsigned long size)
{
   void* pSeg = (void*)LD_NO_ADDRESS;

   if ( size > 0 )
   {
      /*
       *	Allocate an extra page after the data segment and mark it as not present
       * to aid checking for data overwrites.
       */
      unsigned long paddedSize = (size + 0xfff) & 0xfffff000;
      pSeg = valloc(paddedSize + 0x1000);

      if ( pSeg )
      {
         unsigned long endPage = ((unsigned long)pSeg) + paddedSize;
         vmBaseStateSet(NULL, (void*)endPage, 0x1000, VM_STATE_MASK_VALID, VM_STATE_VALID_NOT);

         /*
          *	Fill the segment with a known value.  To better detect memory overwrites,
          * align the segment at the end of the page, noting that segment should start
          * on a 16 byte boundary.
          */
         memset(pSeg, 0xcc, paddedSize);
         pSeg = (void*)( ((unsigned long)pSeg) + paddedSize - size );
         pSeg = (void*)( ((unsigned long)pSeg) & 0xfffffff0 );
      }
   }

   return pSeg;
}

static void* allocCodeSegment (unsigned long size)
{
   void* pSeg = (void*)LD_NO_ADDRESS;

   if ( size > 0 )
   {
      /*
       *	Pad code segment to a page boundary
       */
      unsigned long paddedSize = (size + 0xfff) & 0xfffff000;
      pSeg = valloc(paddedSize);
   }

   return pSeg;
}
#endif /* if CPU != SIMNT */

STATUS loadModuleFromFile (const char* fileName, LoadModuleInfo* info)
{
   int    loadFD = open(fileName, O_RDONLY, DEFAULT_FILE_PERM);
   STATUS status = ERROR;

   if (loadFD >= 0)
   {
      LoadModuleInfo localInfo;
      if ( info )
      {
         info->fileName = fileName;
         status         = loadModuleFromFileDescriptor(loadFD, info);
      }
      else
      {
         localInfo.fileName = fileName;
         status             = loadModuleFromFileDescriptor(loadFD, &localInfo);
      }

      close(loadFD);
   }
   return status;
}

#if CPU != SIMNT

#include <elf.h>

static void getSectionInfo (int loadFD, const char* fileName, void** ppText, void** ppData, void** ppBSS, unsigned long* textSize)
{
   Elf32_Ehdr    fileHdr;
   int           sectIdx;
   int           numElements;

   unsigned long dataSize = 0;
   unsigned long bssSize  = 0;

   *textSize = 0;
   lseek(loadFD, 0, SEEK_SET);
   if ( read(loadFD, (char*)&fileHdr, sizeof(fileHdr) ) == ERROR )
   {

      printf("getSectionInfo: read1 Error!\n");
      perror(fileName);
   }

   for ( sectIdx = 0; sectIdx<fileHdr.e_shnum; sectIdx++ )
   {
      Elf32_Shdr sectHdr;
      lseek(loadFD, fileHdr.e_shoff + sectIdx * fileHdr.e_shentsize, SEEK_SET);
      read(loadFD, (char*)&sectHdr, sizeof(sectHdr) );

      if ( sectHdr.sh_type == SHT_PROGBITS &&
           (sectHdr.sh_flags & SHF_ALLOC) == SHF_ALLOC &&
           ( (sectHdr.sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR ||
             (sectHdr.sh_flags & SHF_WRITE) != SHF_WRITE ))
      {
         *textSize += sectHdr.sh_size + sectHdr.sh_addralign - 1;
      }
      else if ( sectHdr.sh_type == SHT_PROGBITS &&
                (sectHdr.sh_flags & (SHF_ALLOC | SHF_WRITE)) == (SHF_ALLOC | SHF_WRITE) )
      {
         dataSize += sectHdr.sh_size + sectHdr.sh_addralign - 1;
      }
      else if ( sectHdr.sh_type == SHT_NOBITS &&
                (sectHdr.sh_flags & (SHF_ALLOC | SHF_WRITE)) == (SHF_ALLOC | SHF_WRITE) )
      {
         bssSize += sectHdr.sh_size + sectHdr.sh_addralign - 1;
      }
   }

   printf("%s: textSize=%lx dataSize=%lx bssSize=%lx\n", fileName, *textSize, dataSize, bssSize);

   *ppText = allocCodeSegment(*textSize);
   *ppData = allocDataSegment(dataSize);
   *ppBSS  = allocDataSegment(bssSize);
}
#endif /* if CPU != SIMNT */

STATUS loadModuleFromFileDescriptor (int loadFD, LoadModuleInfo* info)
{
   STATUS        status   = OK;
   MODULE_ID     modID    = NULL;
   const char*   fileName;
   void*         pText    = LD_NO_ADDRESS;
   void*         pData    = LD_NO_ADDRESS;
   void*         pBSS     = LD_NO_ADDRESS;
   unsigned long textSize = 0;
   if ( info && info->fileName )
      fileName = info->fileName;
   else
      fileName = "UnknownFileName";

#if CPU != SIMNT
   getSectionInfo(loadFD, fileName, &pText, &pData, &pBSS, &textSize);
#endif /* if CPU != SIMNT */

   lseek(loadFD, 0, SEEK_SET);

   modID = loadModuleAt(loadFD, LOAD_GLOBAL_SYMBOLS, (char**)&pText, (char**)&pData, (char**)&pBSS);

   if ( modID == NULL )
   {
      printf("\"%s\": failed to load module\n", fileName);
      status = ERROR;
   }
   else
   {
#if CPU != SIMNT
#ifndef DEBUG_BUILD
      textSize = (textSize + 0xfff) & 0xfffff000;
      vmBaseStateSet(NULL, pText, textSize, VM_STATE_MASK_WRITABLE, VM_STATE_WRITABLE_NOT);
#endif /* if not DEBUG_BUILD */
#endif /* if CPU != SIMNT */

      printf("%s: textAddr=%lx dataAddr=%lx bssAddr=%lx\n", fileName, (unsigned long)pText, (unsigned long)pData, (unsigned long)pBSS);

      /*
       *	Return load information if requested
       */
      if ( info )
      {
         info->modId    = modID;
         info->textAddr = (unsigned long)pText;
         info->dataAddr = (unsigned long)pData;
         info->bssAddr  = (unsigned long)pBSS;
      }
   }

   return status;
}

/* FORMAT HASH a6624073fe3c7798eb84dc0959fcf116 */
