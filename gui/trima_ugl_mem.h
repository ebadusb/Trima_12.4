#ifndef __TRIMA_UGL_MEM_H__
#define __TRIMA_UGL_MEM_H__

#include <vxWorks.h>

#if CPU==SIMNT
#define GUI_POOL_MALLOC  malloc
#define GUI_POOL_CALLOC  calloc
#define GUI_POOL_FREE    free
#else
#include <ugl/uglmem.h>
#define GUI_POOL_MALLOC  UGL_MALLOC
#define GUI_POOL_CALLOC  UGL_CALLOC
#define GUI_POOL_FREE    UGL_FREE
#endif // #if CPU==SIMNT

#endif // #ifndef __TRIMA_UGL_MEM_H__

/* FORMAT HASH 234d047b7dda048360ed8009393bf877 */
