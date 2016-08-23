/*
 *	Pool allocator for small memory blocks.  UGL tends to allocate many small chunks of memory.
 * Using a pool allocator for this significantly improves performance and reduces memory
 * fragmentation associated with long run times.
 *
 * Revision 1.1  2005/08/30 21:42:50Z  jl11312
 *
 */

#ifndef _CGUI_POOL_ALLOCATOR_HPP_
#define _CGUI_POOL_ALLOCATOR_HPP_

// Set up pool allocator for UGL.  Must be called before initializing UGL.
void cguiPoolAllocatorInitialize ();

#endif

/* FORMAT HASH 6b176ca22a265ecbaebc9f1c498bbc3b */
