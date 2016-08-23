/* FILENAME: platform.h
 * PURPOSE: encapsulates platform dependencies
 * provides common wrappers where needed or redefinitions
 * so that src code can use unix version.
 * CHANGELOG:
 *   12/07/99 - dyes - initial version
 *   01/17/2000 - dyes - add env.h and setenv definition
 *   04/29/2002 - dyes - add VXWORKS specific stuff
 */

#ifndef __platform_h__ // prevents multiple inclusion
#define __platform_h__

//
// define only one platform
//
//#define BUILD_FOR_NT
//#define BUILD_FOR_UNIX
#define BUILD_FOR_VXWORKS

//
// unix definitions
//
#ifdef BUILD_FOR_UNIX

#include <sys/wait.h>
#include <unistd.h>
#include <env.h>
#include <sys/select.h>
#include <ctype.h>
#include <unix.h>

typedef int bool;
const bool false = 0;
const bool true = 1;

#endif
//
// end of UNIX definitions
//

//
// NT definitions
//
#ifdef BUILD_FOR_NT
#include <winsock2.h>
#include <wtypes.h>
#include <winbase.h>
#include <direct.h> // for getcwd
#include <process.h> // for getpid
#include <io.h>

#define getcwd(a,b) _getcwd(a,b)
#define mkdir(f,p) _mkdir(f)
#define set_new_handler(h) _set_new_handler(h)
#define sleep(n) Sleep(1000*n)
#define setenv(n,v,f) !SetEnvironmentVariable(n,v)
#define h_errno WSAGetLastError()
#define getpid() _getpid()
#define tzset() _tzset()
#define stat _stat
#define open(path,mode) _open(path,mode)
#define read(fd,buf,size) _read(fd,buf,size)
#define filelength(fd) _filelength(fd)

#define O_BINARY _O_BINARY
#define O_CREAT _O_CREAT
#define O_WRONLY _O_WRONLY

#endif
//
// end of NT definitions
//


//
// VxWorks definitions
//
#ifdef BUILD_FOR_VXWORKS
#include <selectLib.h>
#include <taskLib.h>
#include <sysLib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// JPH -- change name due to namespace collision with unistd.h!!
int sleep_unixEmu(int n);

#ifdef __cplusplus
};
#endif


#endif
//
// end of VxWorks definitions
//

#endif // __platform_h__
