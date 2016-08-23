#include <vxworks.h>

#if (CPU==SIMNT)
void guimain (void);
#else
extern "C" void guimain (void);
#endif


static void (* forceLine) (void) = guimain;

//
// Stub for GUI memory pool. Pooling not necessary
// in service mode.
void cguiPoolAllocatorInitialize (size_t)
{}
//
//
//  Specific startup initialization for service mode
void guiinit ()
{}

/* FORMAT HASH 1c2344792c9852591ee670000b03a014 */
