/* ftpdLib.c - File Transfer Protocol (FTP) server */
 
/* Copyright 1990 - 2003 Wind River Systems, Inc. */
#include "copyright_wrs.h"
 
/*
modification history
--------------------
03t,05may03,aag  PORT command OK before login complete since login is
                 required to open a connection.  This is needed to
                 support Windows command line FTP client which issues
                 the PORT command after an unsuccessful login, and
                 expects the value to stick.  Also implement a timmed
                 linger using a seperate task.  This is required, again
                 because of Windows.  Change ASCII mode to use fd's
                 instead of FILE's because when the file is fclose()'d
                 the underlying fd would also get closed, this breaks
                 the functionality of the linger.  Change the multi-purpose
                 buffer to 1k instead of 4k.  This configuration works
                 much better on systems with very few resources allocated
                 to the network stack.
03s,28apr03,syk  Added Fix for MKD,RMD commands, fix for allowing
                 Non-guest user to login after ftpdAnonymousAllow is
                 given on FTP server and fix to allow Anonymous users
                 to upload to the "/" directory.(SPR #20768)
03r,12dec02,aag  Add ftpdLoginToCurrentDirectory flag for compatibility
                 with old FTP server
03q,11dec02,aag  Add support for both DOS and UNIX style directory
                 listings.
03p,30sep02,aag  Correction to ftpdAnonymousAllow so that writeDirName
                 specifies a full path, excluding the trailing slash.
                 The trailing slash is enforced by pathcmp().
03o,18sep02,aag  Completely re-do ftpdSockFree to implement a
                 just-long-enough linger mechanism.  Basically, set
                 no-linger and call shutdown to inform client
                 nothing more to send.  Then, set linger to on
                 and 0 seconds.  This will cause the PCB to be
                 freed as soon as close() is called.  However, don't
                 call close on data connections until the client
                 requests a new data connection!  This allows the
                 client to gracefully close the connection and
                 request re-transmit of any pending packets.
---,-----02,elr  Extensive cleanup for coding conventions and documentation
03n,06sep02,aag  Fix bug in setsockopt, remove tabs that somehow made it
                 into the directory listing format specifier since it
                 was confusing Windows-2000.
03m,16jul02,aag  Change command socket to be TCP_NODELAY.  This "fixes"
                 the the problems with the DOS command line versions of
                 FTP which crash if the RST bit is received on the data
                 socket prior to the "226 Transfer complete" being read off
                 the control socket.
03l,11jul02,aag  Used wrs-coding-style.el from David H. to "fix" the
                 code to match the desired indentation...
03k,01may02,aag  Added hook for custom access verification.
                 ftpdPathAccessVerifyRtn is a function pointer that
                 allows any the function to validate if access should
                 be granted based on path, username, and access mode
                 (O_RDONLY or O_WRONLY).  This can provide what one
                 would normally expect from a UNIX based server...
03j,30Arp02,aag  Fixed up code for clients that do not use PASV mode.
03i,06mar02,aag  Added code so that is ftpsMaxClients is set prior to
                 call to ftpdInit, the number of clients allowed in the
                 backlog is set appropriatly.  This should allow dozens
                 of clients to connect at the same time.  Also changed
                 ftpdTaskMask to an array to support max 256 clients
                 instead of max of 32.  Added network buffer tuning
                 information to the "Known Problems" section.
03h,06mar02,aag  Added change to make "/" the default directory for all
                 users, not just anonymous (for anonymous, "/" is relative
                 to the specified directory).  This makes using the
                 Windows-2000 Network place feature and IE work better.
                 Also, added code to explicitly ignore the "/tgtsvr"
                 filesystem since I know it will fail, and because if not
                 target-server is attached, you have to wait several minutes
                 for it to timeout when first listing the root directory.
03g,05mar02,aag  Added feature to be able to access the Root "/" directory.
                 This involved fixing the PathNormalize command to recognize
                 a path starting with "/" as an absolute, not relative path.
                 Currently, only those devices which support the opendir()
                 command (e.g. DOS and NFS) will be returned as part of the
                 root.  Currently, the fileMark value (-F flag on LS) is
                 constructed, but not appended to the listing as it did not
                 seem to be required by Exceed.
03f,05mar02,aag  Changed timeout mechanism from passive (requires new
                 incoming connection to check) to active (uses select).
                 The primary benefits are, if MAX_SESSIONS was open, then
                 there was no mechanism to time out a connection, and if
                 a session spent a long time doing a transfer on the
                 data socket, (more then the timeout value) a new incoming
                 connection would have closed the still valid session.
                 Changed Stack size to 6k.  Testing showed that with the
                 arrays moved into the pSlot object so they could be passed
                 around, the stack never went beyond 3k.  Added ftpdDelete
                 as a reboot hook, and sent session timeout message to
                 clients connected when rebooting.
03e,04mar02,aag  Add processing of ls flags on LIST and NLST command to
                 control the format of the listing.  This is for Exceed.
                 The "known" flags as l, g, F, a, although a is currently
                 ignored.  Removed the ftpdSessionDelete call from the QUIT
                 command, and change the return code to CLOSE.  This
                 eliminates the memPartFree error.  Added bit flags for
                 ftpdDebugMsg so its not an all or nothing kind of thing,
                 and change ftpdDbgMsg and ftpdCmdSend to use varargs so that
                 I don't have to cast everything it (int) and pad 0's to
                 all the calls.
03d,27feb02,aag  Change ftpdWorkTask to be table driven, this should make
                 adding features and debugging much simpler for future
                 revisions.  Reuse empirical data port, and set SO_LINGER
                 to 1 second. Changed ftpdInit back to old style (login func
                 & stack size) to be backwards compatible, changed listing to
                 do HH:MM for files modified in last 11 months, not during
                 current year, and added ftpdDefaultPort to allow setting of
                 the port as an alternative.  Remove ftpdWindowSize.  Changed
                 name of server from "VxWorks" to "Tornado-vxWorks" because
                 some applications like WS_FTP already knew about VxWorks and
                 these changes break it.  Assumed other may have the same
                 problem.
03c,05jun00,dlr  anonymous FTP vastly improved - "virtual root directory" now
                 in place for anonymous users; anonymous path always begins
                 with "/"; permissions fixed for anonymous upload access, etc.
                 DELE no longer removes directories (as it should be)
                 RMD no longer removes files (as it should be)
03b,02jun00,dlr  incorporated prior modifications 02o and 02p
                 also fixed return code for (X)RMD (from 200 to 250)
03a,25may98,lrn  anonymous access with root dir and upload dir
                 connection timeout, default mode Binary
                 enhance transfer rate: bufSize now same as winSize
                 mget "*" support with wildcards, rework directory travel
                 standardized directory listing to work with GUI clients
                 simplified error handling in main loop, to make code smaller
                 fixed: ASCII transfer of binary files breaks connection
02r,13oct00,cn   fixed memory leak (SPR# 25954).
02q,16mar99,spm  recovered orphaned code from tor2_0_x branch (SPR #25770)
02p,01dec98,spm  changed reply code for successful DELE command (SPR #20554)
02o,27mar98,spm  corrected byte-ordering problem in PASV command (SPR #20828)
02n,27mar98,spm  merged from recovered version 02m of tor1_0_x branch
02m,10dec97,spm  upgraded server shutdown routine to terminate active
                 sessions (SPR #9906); corrected response for PASV command
                 to include valid IP address (SPR #1318); modified syntax
                 of PASV command (SPR #5627); corrected handling of PORT
                 command to support multiple interfaces (SPR #3500); added
                 support for maximum number of connections (SPR #2032);
                 applied changes for configurable password authentication
                 from SENS branch (SPR #8602); removed incorrect note from
                 man page concerning user/password verification, which was
                 actually performed (SPR #7672); general cleanup (reorganized
                 code, added FTP responses for error conditions, replaced
                 "static" with LOCAL keyword in function declarations)
02l,09jul97,dgp  doc: add note on UID and password per SPR 7672
02k,06feb97,jdi  made drawing internal.
02j,30sep96,spm  partial fix for spr #7227. Added support for deleting files
                 and using relative pathnames when listing directories.
02i,05aug96,sgv  fix for spr #3583 and spr #5920. Provide login security
                 for VxWorks login
02h,21may96,sgv  Added global variable ftpdWindowSize which can be set by
                 the user. the server would set the window size after the
                 connection is established.
02g,29mar95,kdl  changed ftpdDirListGet() to use ANSI time format in stat.
02f,11feb95,jdi  doc format tweak.
02e,20aug93,jag  Fixed memory leak by calling fclose (SPR #2194)
                 Changed ftpdWorkTask Command Read Logic, Added error checking
                 on write calls to the network and file operations.
                 Added case-conversion changes (SPR #2035)
02d,20aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
02c,27feb93,kdl  Removed 01z case-conversion changes (SPR #2035).
02b,05feb93,jag  Changed call to inet_ntoa to inet_ntoa_b. SPR# 1814
02a,20jan93,jdi  documentation cleanup for 5.1.
01z,09sep92,jmm  fixed spr 1568, ftpd now recognizes lower case commands
                 changed errnoGet() to errno to get rid of warning message
01y,19aug92,smb  Changed systime.h to sys/times.h.
01x,16jun92,kdl  increased slot buffer to hold null terminator; use calloc()
                 to allocate slot struct (SPR #1509).
01w,26may92,rrr  the tree shuffle
                  -changed includes to have absolute path from h/
01v,08apr92,jmm  cleaned up some ansi warnings
01u,18dec91,rrr  removed a recursive macro (killed the mips compiler)
01t,10dec91,gae  ANSI cleanup. Changed ftpdSlotSem to an Id so that internal
                    routine semTerminate() not used.
01s,19nov91,rrr  shut up some ansi warnings.
01r,14nov91,rrr  shut up some warnings
01q,12nov91,wmd  fixed bug in ftpdDataStreamSend() and ftpdDataStreamReceive(),
                 EOF is cast to type char to prevent endless looping.
01p,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
                  -changed includes to have absolute path from h/
                  -changed VOID to void
                  -changed copyright notice
01o,10jul91,gae  i960 fixes: non-varargs usage, added ntohs().  added HELP
                 command, changed listing to support NFS/DOS not old style.
01n,30apr91,jdi  documentation tweaks.
01m,05apr91,jdi  documentation -- removed header parens and x-ref numbers;
                 doc review by dnw.
01l,12feb91,jaa  documentation.
01k,08oct90,hjb  included "inetLib.h".
01j,05oct90,dnw  made ftpdWorkTask() be LOCAL.
                 documentation tweaks.
01i,02oct90,hjb  deleted "inet.h".  added more doc to ftpdInit().  added a call
                   to htons() where needed.
01h,18sep90,kdl  removed erroneous forward declaration of "ftpDataStreamRecv()".
01g,10aug90,dnw  added forward declaration of ftpdDataStreamReceive().
01f,10aug90,kdl  added forward declarations for functions returning void.
01e,26jun90,jcf  changed ftpd semaphore to static mutex.
01d,07may90,hjb  various bug fixes -- too numerous to mention.
01c,17apr90,jcf  changed ftpd work task name to tFtpd...
01b,11apr90,hjb  de-linted
01a,01mar90,hjb  written
*/
 
/*
DESCRIPTION
This library implements the server side of the File Transfer Protocol (FTP),
which provides remote access to the file systems available on a target.
The protocol is defined in RFC 959. This implementation supports all commands
required by that specification, as well as several additional commands.
 
USER INTERFACE
During system startup, the ftpdInit() routine creates a control connection
at the predefined FTP server port which is monitored by the primary FTP
task. Each FTP session established is handled by a secondary server task
created as necessary. The server accepts the following commands (commands
marked with + existed in or prior to RFC 959, and are not included when
responding to the FEAT command):
 


.TS
tab(|);
l1 l.
  + HELP | - List supported commands.
  + USER | - Verify user name.
  + PASS | - Verify password for the user.
  + QUIT | - Quit the session.
  + LIST | - List out contents of a directory.
  + NLST | - List directory contents using a concise format.
  + RETR | - Retrieve a file.
  + STOR | - Store a file.
  + CWD  | - Change working directory.
  + XCWD | - Change working directory.
  + CDUP | - Change to parent directory.
  + XCUP | - Change to parent directory.
  + TYPE | - Change the data representation type.
  + PORT | - Change the port number.
  + PWD  | - Get the name of current working directory.
  + XPWD | - Get the name of current working directory.
  + STRU | - Change file structure settings.
  + MODE | - Change file transfer mode.
  + ALLO | - Reserver sufficient storage.
  + ACCT | - Identify the user's account.
  + PASV | - Make the server listen on a port for data connection.
  + NOOP | - Do nothing.
  + DELE | - Delete a file
  + MKD  | - Make a directory
  + XMKD | - Make a directory
  + RMD  | - Remove a directory
  + XRMD | - Remove a directory
  + RNFR | - File to rename
  + RNTO | - a new name for a file that is being renamed
    SIZE | - get file size in bytes
    MDTM | - get file modification date and time
  + SYST | - report type of operating system
    FEAT | - report additional features
.TE
 
The ftpdDelete() routine will disable the FTP server until restarted.
It reclaims all system resources used by the server tasks and cleanly
terminates all active sessions.
 
INTERNAL
The ftpdInit() routine spawns the primary server task ('ftpdTask') to handle
multiple FTP sessions. That task creates a separate task ('ftpdWorkTask') for
each active control connection.
 
The diagram below defines the structure chart of ftpdLib.
.CS
 
  ftpdDelete                                    ftpdInit
        |  \                                      |
        |   \                                     |
        |    \                                  ftpdTask
        |     \                                 /    |  \____________
        |      \                               /     |               \
        |       |                 ftpdSessionAdd ftpdWorkTask ftpdSessionDelete
        |       |             ______________________/     |  \
        |       |            /    /     |                 |   \
 ftpdSlotDelete | ftpdDirListGet /  ftpdDataStreamReceive |   ftpdDataStreamSend
        |       |               /       |       \         |   /      /
         \      |    __________/        |        \        |  /      /
          \     |   /                   |         ftpdDataConnGet  /
           \    |   |                   |          |   ___________/
            \   |   |                   |          |  /
         ftpdSockFree                   ftpdDataCmdSend
.CE
Known Problems:
Work task name are not guaranteed to be unique.
All file sizes are done in 32-bit, hence 3 GB max file size limitation.
Should log every anonymous access to logMsg or log file.
 
When connecting multiple clients at the same time, be sure to
adjust the following:
        NUM_FILES = ftpsMaxClients * 3 + 20,
        NUM_SYS_128 = 2 * ftpsMaxClients
        NUM_SYS_256 = 3 * ftpsMaxClients
        NUM_SYS_CL_BLKS = NUM_SYS_64 + NUM_SYS_128 +
                        NUM_SYS_256 + NUM_SYS_512
otherwise, the network stack may run out of buffers.  Adjustment
of NUM_(64/128/256/512/1024/2048) should also be made depending
on the amount and size of data being sent or received at the same
time.
 
INCLUDE FILES: ftpdLib.h
 
SEE ALSO:
ftpLib, netDrv,
.I "RFC-959 File Transfer Protocol"
*/
 


#include "vxWorks.h"
#include "ioLib.h"
#include "taskLib.h"
#include "lstLib.h"
#include "stdio.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "netinet/tcp.h"
#include "errno.h"
#include "version.h"
#include "string.h"
#include "stdlib.h"
#include "iosLib.h"
#include "inetLib.h"
#include "dirent.h"
#include "sys/stat.h"
#include "sys/times.h"
#include "sockLib.h"
#include "logLib.h"
#include "unistd.h"
#include "pathLib.h"
#include "sysLib.h"
#include "ftpdLib.h"
#include "ctype.h"
#include "time.h"
#include "loginLib.h"
#include "fioLib.h"
#include "dosFsLib.h"   /* for attributes definition */
#include "stdarg.h"
#include "selectLib.h"
#include "dllLib.h"
#include "private/iosLibP.h"
#include "rebootLib.h"
#include "msgQLib.h"
#include "net/protosw.h"
#include "tickLib.h"
#include "readdir_stat.h"

#ifdef FTPD_DEBUG
#undef LOCAL
#define LOCAL
#endif
#define FTPD_DEBUG_ERROR        (ftpdDebug & 0x0001)
#define FTPD_DEBUG_BASIC        (ftpdDebug & 0x0002)
#define FTPD_DEBUG_CMD          (ftpdDebug & 0x0004)
#define FTPD_DEBUG_PATH         (ftpdDebug & 0x0008)
#define FTPD_DEBUG_RESP         (ftpdDebug & 0x0010)
#define FTPD_DEBUG_GET          (ftpdDebug & 0x0020)
#define FTPD_DEBUG_ACCESS       (ftpdDebug & 0x0040)
#define FTPD_DEBUG_LINGER       (ftpdDebug & 0x0080)
int ftpdDebug                   = 0x0;
 
 
/* Representation Type - byte 0 */
#define FTPD_BINARY_TYPE        0x0001
#define FTPD_ASCII_TYPE         0x0002
#define FTPD_EBCDIC_TYPE        0x0004
#define FTPD_LOCAL_BYTE_TYPE    0x0008
 
/* Transfer mode - byte 1 */
#define FTPD_STREAM_MODE        0x0010
#define FTPD_BLOCK_MODE         0x0020
#define FTPD_COMPRESSED_MODE    0x0040
 
/* File structure - byte 2 */
#define FTPD_NO_RECORD_STRU     0x0100
#define FTPD_RECORD_STRU        0x0200
#define FTPD_PAGE_STRU          0x0400
 
/* Session Status - byte 3 */
#define FTPD_USER_OK            0x1000
#define FTPD_ANONYMOUS          0x2000
#define FTPD_PASSIVE            0x4000
 
/* LIST flags */
#define FTPD_LIST_GROUP         0x01
#define FTPD_LIST_ALL           0x02
#define FTPD_LIST_MARK          0x04
#define FTPD_LIST_LONG          0x08
 
/* Macros to obtain correct parts of the status code */
 
#define FTPD_REPRESENTATION(slot)       ((slot)->status & 0xff)
 
#ifdef  _unused_
#define FTPD_TRANS_MODE(slot)           (((slot)->status >> 8)  & 0xff)
#define FTPD_FILE_STRUCTURE(slot)       (((slot)->status >> 16) & 0xff)
#define FTPD_STATUS(slot)               (((slot)->status >> 24) & 0xff)
#endif
 
/* Well known port definitions -- someday we'll have getservbyname */
 
#define FTP_DATA_PORT           20
#define FTP_DAEMON_PORT         21
 
/* Free socket indicative */
 
#define FTPD_SOCK_FREE          -1
 
/* Arbitrary limits for the size of the FTPD work task name */
 
#define FTPD_WORK_TASK_NAME_LEN 40
 
/* Arbitrary limits hinted by Unix FTPD in waing for a new data connection */
 
#define FTPD_WAIT_MAX           90
#define FTPD_WAIT_INTERVAL      5
#define FTPD_CONN_TIMEOUT       200
#define FTPD_LINGER_SECONDS     3
 
/* Macro to get the byte out of an int */
 
#define FTPD_UC(ch)             (((int) (ch)) & 0xff)
 
/* Bit set in FTP reply code to indicate multi-line reply.
 * Used internally by ftpdCmdSend() where codes are less than
 * 1024 but are 32-bit integers.  [Admittedly a hack, see
 * ftpdCmdSend().]
 */
 
#define FTPD_MULTI_LINE         0x10000000
 
#define FTPD_ANONYMOUS_USER_1   "anonymous"
#define FTPD_ANONYMOUS_USER_2   "ftp"
#define FTPD_ANONYMOUS_USER_3   "guest"
 
#define CLOSE                   (OK + 1)
 
/* globals */
int ftpdLingerTicks             = 60 * 3;
int ftpdTaskOptions             = 0;
int ftpdWorkTaskOptions         = 0;
int ftpdTaskPriority            = 56;
int ftpdWorkTaskPriority        = 152;
int ftpdWorkTaskStackSize       = 1024 * 6; /* 6k is more then enough */
int ftpsMaxClients              = 8;        /* Default max upped to 8 for MS Client compatibility */
int ftpsCurrentClients;
FUNCPTR loginVerifyRtn;
FUNCPTR ftpdPathAccessVerifyRtn = NULL;
int ftpdDefaultPort             = FTP_DAEMON_PORT;
int ftpdClientSessionTimeout    = 900; /* 900 seconds is typical timeout */
int ftpdLoginToCurrentDirectory = 0;
/* locals */
 
LOCAL BOOL ftpsActive = FALSE;  /* Server started? */
LOCAL BOOL ftpsShutdownFlag;    /* Server halt requested? */
LOCAL MSG_Q_ID ftpdCloseQ;
 
/*
 * The FTP server keeps track of active client sessions in a linked list
 * of the following FTPD_SESSION_DATA data structures. That structure
 * contains all the variables which must be maintained separately
 * for each client so that the code shared by every secondary
 * task will function correctly.
 */
 
typedef struct
    {
    NODE                node;           /* for link-listing */
    int                 status;         /* see various status bits above */
    int                 byteCount;      /* bytes transferred */
    int                 cmdSock;        /* command socket */
    STATUS              cmdSockError;   /* Set to ERROR on write error */
    int                 dataSock;       /* data socket */
    int                 listenSock;     /* socket to listen for connections */
    int                 taskId;
    int                 ftpdTaskNum;
    unsigned long       clientIp;
    unsigned short      clientPort;
    short               listen[6];
    char                *newDirName;    /* directory name place holder */
    char                *dirName;       /* directory name place holder */
    char                buf     [BUFSIZE]; /* multi-purpose buffer per session */
    char                curDir  [MAX_FILENAME_LENGTH+1]; /* active directory */
    char                renFile [MAX_FILENAME_LENGTH+1];
    char                currPath[MAX_FILENAME_LENGTH+1];
    char                newPath [MAX_FILENAME_LENGTH+1];
    char                usrName [MAX_LOGIN_NAME_LEN+1]; /* current user */
    char                cmdbuf  [BUFSIZE];
    } FTPD_SESSION_DATA;
 
typedef struct FTPD_SOCKET FTPD_SOCKET;
struct FTPD_SOCKET
    {
    int fd;
    unsigned long closeTick;
    };
 
typedef struct FTPD_COMMAND_TABLE FTPD_COMMAND_TABLE;
struct FTPD_COMMAND_TABLE 
    {
    STATUS (*cmd)(FTPD_SESSION_DATA *pSlot, const char *cmd);
    char command[5];            /* COMMAND NAME - MAX 4 chars */
    char requiresSession;
    };
 
LOCAL STATUS unixList( FTPD_SESSION_DATA *, char *, struct stat *, int );
 
LOCAL STATUS ftpdProcessAllo (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessDele (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessFeat (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessHelp (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessList (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessNoop (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessMode (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessPass (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessPasv (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessPort (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessQuit (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessRetr (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessRnfr (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessRnto (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessStor (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessStru (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessSyst (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessType (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessUser (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessXcwd (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessXmkd (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessXpwd (FTPD_SESSION_DATA *, const char *cmd);
LOCAL STATUS ftpdProcessXrmd (FTPD_SESSION_DATA *, const char *cmd);
 
/* NOTE: THIS TABLE MUST BE SORTED FOR THE BINARY SEARCH TO WORK */
const FTPD_COMMAND_TABLE ftpdCommandTable[] = {
  { ftpdProcessQuit, "ABOR", 0 }, /* Treat ABOR as QUIT for Microsoft Client */
  { ftpdProcessAllo, "ACCT", 1 },
  { ftpdProcessAllo, "ALLO", 1 },
  { ftpdProcessXcwd, "CDUP", 1 },
  { ftpdProcessXcwd, "CWD",  1 },
  { ftpdProcessDele, "DELE", 1 },
  { ftpdProcessFeat, "FEAT", 1 },
  { ftpdProcessHelp, "HELP", 0 },
  { ftpdProcessList, "LIST", 1 },
  { ftpdProcessRetr, "MDTM", 1 },
  { ftpdProcessXmkd, "MKD",  1 },
  { ftpdProcessMode, "MODE", 1 },
  { ftpdProcessList, "NLST", 1 },
  { ftpdProcessNoop, "NOOP", 1 },
  { ftpdProcessPass, "PASS", 0 },
  { ftpdProcessPasv, "PASV", 1 },
  { ftpdProcessPort, "PORT", 0 },
  { ftpdProcessXpwd, "PWD",  1 },
  { ftpdProcessQuit, "QUIT", 0 },
  { ftpdProcessRetr, "RETR", 1 },
  { ftpdProcessXrmd, "RMD",  1 },
  { ftpdProcessRnfr, "RNFR", 1 },
  { ftpdProcessRnto, "RNTO", 1 },
  { ftpdProcessRetr, "SIZE", 1 },
  { ftpdProcessStor, "STOR", 1 },
  { ftpdProcessStru, "STRU", 1 },
  { ftpdProcessSyst, "SYST", 0 },
  { ftpdProcessType, "TYPE", 0 },
  { ftpdProcessUser, "USER", 0 },
  { ftpdProcessXcwd, "XCUP", 1 },
  { ftpdProcessXcwd, "XCWD", 1 },
  { ftpdProcessXmkd, "XMKD", 1 },
  { ftpdProcessXpwd, "XPWD", 1 },
  { ftpdProcessXrmd, "XRMD", 1 }
};
 
static const char *monthNames[] = {"Jan", "Feb", "Mar", "Apr",
                                   "May", "Jun", "Jul", "Aug",
                                   "Sep", "Oct", "Nov", "Dec"};
 
#define FTPD_NUM_CMDS (sizeof (ftpdCommandTable) / sizeof (ftpdCommandTable[0]))
 
LOCAL int ftpdTaskId            = -1;
LOCAL int ftpdServerSock        = FTPD_SOCK_FREE;
 
LOCAL LIST              ftpsSessionList;
LOCAL SEM_ID            ftpsMutexSem;
LOCAL SEM_ID            ftpsSignalSem;
 
/* bit mask for active task id's 32*8 = 256 MAX!!! */
 
LOCAL int  ftpdTaskMask[8];
 
/* anonymous root directory */
 
LOCAL char rootDirName[MAX_FILENAME_LENGTH+1] = {0};
LOCAL int  rootDirNameLength = 0;   /* length of rootDirName */
 
/* anonymous upload directory */
 
LOCAL char writeDirName[MAX_FILENAME_LENGTH+1] = {0};
 
#define FTPD_TASK_MASK_SIZE (sizeof (ftpdTaskMask)/sizeof (ftpdTaskMask[0]))
 
/* Various messages to be told to the clients */
 
LOCAL const char *messages [] =
    {
    "Can't open passive connection", /* 0 */
    "Parameter not accepted",
    "Data connection error",
    "Directory non existent or syntax error",
    "Local resource failure: %s",
    "Tornado-vxWorks (%s) FTP server ready",
    "Password required",
    "User logged in",
    "Bye...see you later",
    "USER and PASS required",
    "No files found or invalid directory or permission problem", /* 10 */
    "Transfer complete",
    "File \"%s\" not found or permission problem",
    "Cannot create file \"%s\" or permission problem",
    "Changed directory to \"%s\"",
    "Type set to I, binary mode",
    "Type set to A, ASCII mode",
    "Port set okay",
    "\"%s\" is the current directory",
    "File structure set to NO RECORD",
    "Stream mode okay",         /* 20 */
    "Allocate and Account not required",
    "Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
    "NOOP -- did nothing as requested...hah!",
    "Command not recognized",
    "Error in input file",
    "Unimplemented TYPE %d",
    "You could at least say goodbye.",
    "The following commands are recognized:",
    "End of command list.",
    "File deleted successfully.", /* 30 */
    "Login failed.",
    "File exists, ready for destination name.",
    "File renamed successfuly.",
    "\"%s\" directory created",
    "Directory %s removed",
    "%s: not a regular file",
    "UNIX Type: L8 Version: TRIMA (%s)",
    "\"%s\" directory could not be created",
    "Timeout (%d seconds): closing connection.",
    "Guest login ok, send your complete e-mail address as password.", /* 40 */
    "Guest login ok, access restrictions apply.",
    "Guest login ok, upload directory is %s.",
    "Guest access denied.",
    "%s: not a directory",
    "Extensions supported:",
    "End Extensions List",
    "FTP Server has timed out."
    };
 
/* Indexes to the messages [] array */
 
#define MSG_PASSIVE_ERROR       0
#define MSG_PARAM_BAD           1
#define MSG_DATA_CONN_ERROR     2
#define MSG_DIR_NOT_PRESENT     3
#define MSG_LOCAL_RESOURCE_FAIL 4
#define MSG_SERVER_READY        5
#define MSG_PASSWORD_REQUIRED   6
#define MSG_USER_LOGGED_IN      7
#define MSG_SEE_YOU_LATER       8
#define MSG_USER_PASS_REQ       9
#define MSG_DIR_ERROR           10
#define MSG_TRANS_COMPLETE      11
#define MSG_FILE_ERROR          12
#define MSG_CREATE_ERROR        13
#define MSG_CHANGED_DIR         14
#define MSG_TYPE_BINARY         15
#define MSG_TYPE_ASCII          16
#define MSG_PORT_SET            17
#define MSG_CUR_DIR             18
#define MSG_FILE_STRU           19
#define MSG_STREAM_MODE         20
#define MSG_ALLOC_ACCOUNT       21
#define MSG_PASSIVE_MODE        22
#define MSG_NOOP_OKAY           23
#define MSG_BAD_COMMAND         24
#define MSG_INPUT_FILE_ERROR    25
#define MSG_TYPE_ERROR          26
#define MSG_NO_GOOD_BYE         27
#define MSG_COMMAND_LIST_BEGIN  28
#define MSG_COMMAND_LIST_END    29
#define MSG_DELE_OK             30
#define MSG_USER_LOGIN_FAILED   31
#define MSG_RNFR_OK             32
#define MSG_RNTO_OK             33
#define MSG_MKD_OK              34
#define MSG_RMD_OK              35
#define MSG_FILE_NOTREG         36
#define MSG_SYST_REPLY          37
#define MSG_MKD_ERROR           38
#define MSG_CONN_TIMEOUT        39
#define MSG_GUEST_PASS          40
#define MSG_GUEST_OK            41
#define MSG_GUEST_UPLOAD_OK     42
#define MSG_GUEST_ACCESS        43
#define MSG_FILE_NOTDIR         44
#define MSG_FEAT_LIST           45
#define MSG_FEAT_LIST_END       46
#define MSG_SERVER_TIMEOUT      47
 
LOCAL char *ftpdCommandList =
/*        1         2         3         4         5         6
 123456789012345678901234567890123456789012345678901234567890
*/
"ABOR    ACCT    ALLO    CWD     DELE    FEAT    HELP    LIST\r\n"
"MDTM    MKD     MODE    NLST    NOOP    PASS    PASV    PORT\r\n"
"PWD     QUIT    RETR    RMD     RNFR    RNTO    SIZE    STOR\r\n"
"STRU    SYST    TYPE    USER    XCWD    XMKD    XPWD    XRMD\r\n";
 
LOCAL char *ftpdExtendedFeatList =
" SIZE\r\n"
" MDTM\r\n"
;
 
/* forward declarations */
 
LOCAL FTPD_SESSION_DATA *ftpdSessionAdd (void);
LOCAL void ftpdSessionDelete (FTPD_SESSION_DATA *);
LOCAL STATUS ftpdWorkTask (FTPD_SESSION_DATA *);
LOCAL STATUS ftpdCmdSend (FTPD_SESSION_DATA *, int, int, const char *, ...);
LOCAL STATUS ftpdDataConnGet (FTPD_SESSION_DATA *);
LOCAL void ftpdDataStreamSend (FTPD_SESSION_DATA *, char *);
LOCAL STATUS ftpdDataStreamReceive (FTPD_SESSION_DATA *, int);
LOCAL void ftpdSockFree (int *sock);
LOCAL STATUS ftpdDirListGet (FTPD_SESSION_DATA *, char *, BOOL);
LOCAL void ftpdDebugMsg (char *, ...);
LOCAL void unImplementedType (FTPD_SESSION_DATA *pSlot);
LOCAL void dataError (FTPD_SESSION_DATA *pSlot);
LOCAL void fileError (FTPD_SESSION_DATA *pSlot);
LOCAL STATUS transferOkay (FTPD_SESSION_DATA *pSlot);
 
/*******************************************************************************
*
* ftpdTask - FTP server daemon task
*
* This routine monitors the FTP control port for incoming requests from clients
* and processes each request by spawning a secondary server task after
* establishing the control connection. If the maximum number of connections is
* reached, it returns the appropriate error to the requesting client. The
* routine is the entry point for the primary FTP server task and should only
* be called internally.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* INTERNAL:
* The server task is deleted by the server shutdown routine. Adding a newly
* created client session to the list of active clients is performed atomically
* with respect to the shutdown routine. However, accepting control connections
* is not a critical section, since closing the initial socket used in the
* listen() call also closes any later connections which are still open.
*
* NOMANUAL
*/
 
LOCAL void ftpdTask (void)
    {
    int         i,j;
    int         newSock;
    FAST FTPD_SESSION_DATA *pSlot;
    int         on = 1;
    int         addrLen;
    struct sockaddr_in  addr;
    char        taskNameBuf[40];
    char        a_ip_addr [INET_ADDR_LEN];  /* ascii ip address of client */
 
    memset (&ftpdTaskMask[0], 0, sizeof (ftpdTaskMask));
 
    /* The following loop halts if this task is deleted. */
 
    FOREVER
        {
 
        /* Wait for a new incoming connection. */
 
        addrLen = sizeof (struct sockaddr);
 
        if (FTPD_DEBUG_BASIC)
          ftpdDebugMsg ("(%d) waiting for a new client connection...\n", __LINE__);
 
        newSock = accept (ftpdServerSock, (struct sockaddr *) &addr, &addrLen);
        if (newSock < 0)
            {
              if (FTPD_DEBUG_BASIC)
                ftpdDebugMsg ("(%d) cannot accept a new connection\n",__LINE__);
            break;
            }
 
        /*
         * Register a new FTP client session. This process is a critical
         * section with the server shutdown routine. If an error occurs,
         * the reply must be sent over the control connection to the peer
         * before the semaphore is released. Otherwise, this task could
         * be deleted and no response would be sent, possibly causing
         * the new client to hang indefinitely.
         */
 
        semTake (ftpsMutexSem, WAIT_FOREVER);
 
        setsockopt (newSock, SOL_SOCKET, SO_KEEPALIVE, (char *) &on,
                    sizeof (on));
 
        /* Set the control connection to be NO_DELAY */
 
        setsockopt (newSock, IPPROTO_TCP, TCP_NODELAY,
                    (char *) &on, sizeof (on));
 
        inet_ntoa_b (addr.sin_addr, a_ip_addr);
        if ( FTPD_DEBUG_BASIC )
            ftpdDebugMsg ("(%d) accepted a new client connection from %s\n",__LINE__, a_ip_addr);
 
        /* Create a new session entry for this connection, if possible. */
 
        pSlot = ftpdSessionAdd ();
        if (pSlot == NULL) /* Maximum number of connections reached. */
            {
 
            /* Send transient failure report to client. */
 
            ftpdCmdSend (pSlot, newSock, 421,
                         "Session limit reached, closing control connection");
            if (FTPD_DEBUG_BASIC)
                ftpdDebugMsg ("(%d) cannot get a new connection slot\n",__LINE__);
            close (newSock);
            semGive (ftpsMutexSem);
            continue;
            }
 
        /* Create a task name. */
 
        pSlot->ftpdTaskNum = 0;
 
        for (j = 0 ; j < FTPD_TASK_MASK_SIZE ; j++)
            {
            for (i = 1 ; i != 0 ; i = i << 1)
                {
                if ((i & ftpdTaskMask[j]) == 0)
                    {
                    ftpdTaskMask[j] |= i;
                    j = FTPD_TASK_MASK_SIZE; /* To break the outer loop */
                    break;
                    }
                pSlot->ftpdTaskNum++;
                }
            }
 
        pSlot->cmdSock = newSock;
 
        /* Save the control address and assign the default data address. */
 
        pSlot->clientIp = ((struct sockaddr_in)addr).sin_addr.s_addr;
        pSlot->clientPort = htons(FTP_DATA_PORT);
 
        sprintf (taskNameBuf, "tFtpdServ%d", pSlot->ftpdTaskNum);
 
        /* Spawn a secondary task to process FTP requests for this session. */
 
        if (FTPD_DEBUG_BASIC)
          ftpdDebugMsg ("(%d) creating a new server task %s...\n",__LINE__,
                      (int) taskNameBuf);
 
        pSlot->taskId = taskSpawn (taskNameBuf, ftpdWorkTaskPriority,
                                   ftpdWorkTaskOptions, ftpdWorkTaskStackSize,
                                   ftpdWorkTask, (int) pSlot,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0);
 
        if (pSlot->taskId == ERROR)
            {
 
            /* Send transient failure report to client. */
 
            ftpdCmdSend (pSlot, newSock, 421,
                         "Service not available, closing control connection");
            ftpdSessionDelete (pSlot);
            if (FTPD_DEBUG_BASIC)
              ftpdDebugMsg ("(%d) cannot create a new work task\n",__LINE__);
            semGive (ftpsMutexSem);
            continue;
            }
 
        if (FTPD_DEBUG_BASIC)
            ftpdDebugMsg ("(%d) done.\n",__LINE__);
 
        /* Session added - end of critical section with shutdown routine. */
 
        semGive (ftpsMutexSem);
        }
 
    /* Fatal error - update state of server. */
 
    ftpsActive = FALSE;
 
    return;
    }
 
/*******************************************************************************
*
* ftpdCloseTask - FTP server daemon task to close sockets with x second linger
*
* This routine accepts 8-byte messages off a message queue.  The messages are
* a file descriptor number (32 bytes) and a value for vxTicks() at which time
* the file descriptor should be closed.  This function assumes that shutdown()
* was called on the socket prior to it being placed in the queue.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/
 
LOCAL void ftpdCloseTask (void)
    {
    const struct linger so_linger = { 1, 0 }; /* non-zero linger */
    int waitTime = WAIT_FOREVER;
    FTPD_SOCKET sock;
    ULONG pauseTicks;


    /* 4k message-Q 512 messages, bytes each */


    ftpdCloseQ = msgQCreate (512, 8, MSG_Q_FIFO); 
 
    while (msgQReceive (ftpdCloseQ, (void *)&sock, sizeof(sock), 
                        waitTime) == sizeof(sock))
        {
        if (sock.fd == -1)
            {
            /* Wait only 2 seconds for tasks to request socket close... */


            waitTime = sysClkRateGet () * 2; 
            continue;
            }


        /* Only pause if not in a shutdown situation */ 


        if (ftpsShutdownFlag == FALSE) 
            {
            
            /* If pauseTicks is > 5 seconds, the linger
             * expired while in the queue
             */
           
            if ((pauseTicks = sock.closeTick - tickGet ()) <= ftpdLingerTicks )
                {
                taskDelay ( pauseTicks );
                }
            }
        setsockopt (sock.fd, SOL_SOCKET, SO_LINGER, 
                    (char *)&so_linger, sizeof (so_linger));
        close (sock.fd);
        if (FTPD_DEBUG_LINGER)
            ftpdDebugMsg ("closed %d at %d\n", sock.fd, tickGet ());
        if (ftpsShutdownFlag && (msgQNumMsgs (ftpdCloseQ) <= 0))
            {
            break;
            }
        }
    msgQDelete (ftpdCloseQ);
    }
        
 


/*******************************************************************************
*
* ftpdInit - initialize the FTP server task
*
* This routine spawns the FTP server task,
* and establishes a control connection for it on the well-known
* FTP service port, or on another port if a non-zero value is
* specified with the <port> argiment.
*
* It is called during system startup if INCLUDE_FTP_SERVER is defined. 
* The primary server task supports simultaneous client sessions, up to the limit 
* specified by the global variable 'ftpsMaxClients'. The default value allows a 
* maximum of four simultaneous connections. The <stackSize> argument specifies 
* the stack size for the primary server task. It is set to the value specified 
* in the 'ftpdWorkTaskStackSize' global variable by default.
*
* RETURNS:
* OK if server started, or ERROR otherwise.
*
* ERRNO: N/A
*/
 
STATUS ftpdInit
    (
    FUNCPTR     pLoginFunc,     /* Pointer to a login function or NULL */
    int         stackSize       /* task stack size, or 0 for default */
    )
    {
    int                 on = 1;
    struct sockaddr_in  ctrlAddr;
 
    if (pLoginFunc != NULL)
        loginVerifyRtn = pLoginFunc;
 
    if (ftpsActive)
        return (OK);
 
    ftpsShutdownFlag = FALSE;
    ftpsCurrentClients = 0;
 
    /* Create the FTP server control socket. */
 
    ftpdServerSock = socket (AF_INET, SOCK_STREAM, 0);
    if (ftpdServerSock < 0)
        return (ERROR);
 
    /* Create data structures for managing client connections. */
 
    lstInit (&ftpsSessionList);
    ftpsMutexSem = semMCreate (SEM_Q_FIFO | SEM_DELETE_SAFE);
    if (ftpsMutexSem == NULL)
        {
        close (ftpdServerSock);
        return (ERROR);
        }
 
    ftpsSignalSem = semBCreate (SEM_Q_FIFO, SEM_EMPTY);
    if (ftpsSignalSem == NULL)
        {
        close (ftpdServerSock);
        semDelete (ftpsMutexSem);
        return (ERROR);
        }
 
    /* Setup control connection for client requests. */
 
    ctrlAddr.sin_family = AF_INET;
    ctrlAddr.sin_addr.s_addr = INADDR_ANY;
    ctrlAddr.sin_port = htons (ftpdDefaultPort);
 
    if (setsockopt (ftpdServerSock, SOL_SOCKET, SO_REUSEADDR,
                    (char *) &on, sizeof (on)) < 0)
        {
        close (ftpdServerSock);
        semDelete (ftpsMutexSem);
        semDelete (ftpsSignalSem);
        return (ERROR);
        }
 
    if (bind (ftpdServerSock, (struct sockaddr *) &ctrlAddr,
              sizeof (ctrlAddr)) < 0)
        {
        close (ftpdServerSock);
        semDelete (ftpsMutexSem);
        semDelete (ftpsSignalSem);
        return (ERROR);
        }
 
    /* Update the max number of clients to the bitmask size */
 
    if (ftpsMaxClients > (FTPD_TASK_MASK_SIZE * 32))
        {
        ftpsMaxClients = FTPD_TASK_MASK_SIZE * 32;
        }
 
    if (listen (ftpdServerSock, ftpsMaxClients) < 0)
        {
        close (ftpdServerSock);
        semDelete (ftpsMutexSem);
        semDelete (ftpsSignalSem);
        return (ERROR);
        }
 
 
    if (taskSpawn( "tFtpdCloseTask", 20, ftpdTaskOptions,
                   stackSize == 0 ? ftpdWorkTaskStackSize : stackSize,
                   (FUNCPTR) ftpdCloseTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
                   == ERROR )
        {
        close (ftpdServerSock);
        semDelete (ftpsMutexSem);
        semDelete (ftpsSignalSem);
        return (ERROR);
        }
 
    /* Create a FTP server task to receive client requests. */


    ftpdTaskId = taskSpawn ("tFtpdTask", ftpdTaskPriority - 1, ftpdTaskOptions,
                            stackSize == 0 ? ftpdWorkTaskStackSize : stackSize,
                            (FUNCPTR) ftpdTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
 
    if (ftpdTaskId == ERROR)
        {
        if (FTPD_DEBUG_BASIC)
            ftpdDebugMsg ("(%d) ERROR: ftpdTask cannot be created\n",__LINE__);
        close (ftpdServerSock);
        semDelete (ftpsMutexSem);
        semDelete (ftpsSignalSem);
        return (ERROR); /* errno set by taskSpawn() */
        }
 
    ftpsActive = TRUE;
 
    ftpdLingerTicks = sysClkRateGet() * FTPD_LINGER_SECONDS;
 
    rebootHookAdd (ftpdDelete);
 
    if (FTPD_DEBUG_BASIC)
        ftpdDebugMsg ("(%d) ftpdTask created\n",__LINE__);
    return (OK);
    }
 
/*******************************************************************************
*
* ftpdDelete - terminate the FTP server task
*
* This routine halts the FTP server and closes the control connection. All
* client sessions are removed after completing any commands in progress.
* When this routine executes, no further client connections will be accepted
* until the server is restarted. This routine is not reentrant and must not
* be called from interrupt level.
*
* NOTE: If any file transfer operations are in progress when this routine is
* executed, the transfers will be aborted, possibly leaving incomplete files
* on the destination host.
*
* RETURNS: OK if shutdown completed, or ERROR otherwise.
*
* ERRNO: N/A
*
* INTERNAL
* This routine is synchronized with the deletion routine for a client session
* so that the exit of the client tasks can be detected. It also shares a
* critical section with the creation of client sessions to prevent orphaned
* tasks, which would occur if a session were added after this routine had
* shut down all known clients.
*/
 
STATUS ftpdDelete (void)
    {
    BOOL serverActive = FALSE;
    FTPD_SESSION_DATA *pData;
 
    if (!ftpsActive) /* Automatic success if server is not running. */
        return (OK);
 
    /*
     * Remove the FTP server task to prevent additional sessions from starting.
     * The exclusion semaphore guarantees a stable list of active clients.
     */
    semTake (ftpsMutexSem, WAIT_FOREVER);
 
    taskDelete (ftpdTaskId);
    ftpdTaskId = -1;
 
    if (ftpsCurrentClients != 0)
        serverActive = TRUE;
 
    /*
     * Set the shutdown flag so that any secondary server tasks will exit
     * as soon as possible. Once the FTP server has started, this routine is
     * the only writer of the flag and the secondary tasks are the only
     * readers. To avoid unnecessary blocking, the secondary tasks do not
     * guard access to this flag when checking for a pending shutdown during
     * normal processing. Those tasks do protect access to this flag during
     * their cleanup routine to prevent a race condition which would result
     * in incorrect use of the signalling semaphore.
     */
 
    ftpsShutdownFlag = TRUE;
 
    /*
     * Close the command sockets of any active sessions to prevent further
     * activity. If the session is waiting for a command from a socket,
     * the close will trigger the session exit.
     */
 
    while ((pData = (FTPD_SESSION_DATA *)lstFirst (&ftpsSessionList)) != NULL)
        {
        ftpdCmdSend (pData, pData->cmdSock, 221, messages [MSG_SERVER_TIMEOUT], 
                     0, 0, 0, 0, 0, 0);
        ftpdSessionDelete (pData);
        }
 
    semGive (ftpsMutexSem);  
 
    /* Wait for all secondary tasks to exit. */
 
    if (serverActive)
        {
        /*
         * When a shutdown is in progress, the cleanup routine of the last
         * client task to exit gives the signalling semaphore.
         */
 
        semTake (ftpsSignalSem, WAIT_FOREVER);
        }
 
    /*
     * Remove the original socket - this occurs after all secondary tasks
     * have exited to avoid prematurely closing their control sockets.
     */
 
    close (ftpdServerSock);
 
    /* Remove the protection and signalling semaphores and list of clients. */
 
    lstFree (&ftpsSessionList); /* Sanity check - should already be empty. */
 
    semDelete (ftpsMutexSem);
 
    semDelete (ftpsSignalSem);
 
    ftpsActive = FALSE;
 
    return (OK);
    }
 
/*******************************************************************************
*
* ftpdSessionAdd - add a new entry to the ftpd session slot list
*
* Each of the incoming FTP sessions is associated with an entry in the
* FTP server's session list which records session-specific context for each
* control connection established by the FTP clients. This routine creates and
* initializes a new entry in the session list, unless the needed memory is not
* available or the upper limit for simultaneous connections is reached.
*
* RETURNS: A pointer to the session list entry, or NULL of none available.
*
* ERRNO: N/A
*
* NOMANUAL
*
* INTERNAL
* This routine executes within a critical section of the primary FTP server
* task, so mutual exclusion is already present when adding entries to the
* client list and updating the shared variables indicating the current number
* of connected clients.
*/
 
LOCAL FTPD_SESSION_DATA *ftpdSessionAdd (void)
    {
    FAST FTPD_SESSION_DATA *pSlot;
 
    if (ftpsCurrentClients >= ftpsMaxClients)
        return (NULL);
 
    if (ftpsCurrentClients >= (FTPD_TASK_MASK_SIZE * 32))
        {
        /* Update the max number of clients to the bitmask size */
        ftpsMaxClients = FTPD_TASK_MASK_SIZE * 32;
        return (NULL);
        }
 
    /* get memory for the new session entry */
 
    pSlot = (FTPD_SESSION_DATA *) malloc (sizeof (FTPD_SESSION_DATA));
    if (pSlot == NULL)
        {
        return (NULL);
        }
    memset (pSlot, 0, sizeof (pSlot));
 
    /* initialize key fields in the newly acquired slot */
 
    pSlot->dataSock = FTPD_SOCK_FREE;
    pSlot->cmdSock = FTPD_SOCK_FREE;
    pSlot->listenSock = FTPD_SOCK_FREE;
    pSlot->cmdSockError = OK;
    pSlot->status = FTPD_STREAM_MODE | FTPD_BINARY_TYPE | FTPD_NO_RECORD_STRU;
    pSlot->byteCount = 0;
 
    /* assign the default directory for this guy */
 
    strcpy(pSlot->curDir, "/");
 
    semTake (ftpsMutexSem, WAIT_FOREVER);
 
    /* Add new entry to the list of active sessions. */
 
    lstAdd (&ftpsSessionList, &pSlot->node);
    ftpsCurrentClients++;
    semGive (ftpsMutexSem);
 
    return (pSlot);
    }
 
/*******************************************************************************
*
* ftpdSessionDelete - remove an entry from the FTP session list
*
* This routine removes the session-specific context from the session list
* after the client exits or a fatal error occurs.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*
* INTERNAL
* Unless an error occurs, this routine is only called in response to a
* pending server shutdown, indicated by the shutdown flag. Even if the
* shutdown flag is not detected and this routine is called because of an
* error, the appropriate signal will still be sent to any pending shutdown
* routine. The shutdown routine will only return after any active client
* sessions have exited.
*/
 
LOCAL void ftpdSessionDelete
    (
    FAST FTPD_SESSION_DATA *pSlot       /* pointer to the slot to be deleted */
    )
    {
    int i,j;
    if (pSlot == NULL)                  /* null slot? don't do anything */
        return;
 
    /*
     * The deletion of a session entry must be an atomic operation to support
     * an upper limit on the number of simultaneous connections allowed.
     * This mutual exclusion also prevents a race condition with the server
     * shutdown routine. The last client session will always send an exit
     * signal to the shutdown routine, whether or not the shutdown flag was
     * detected during normal processing.
     */
 
    semTake (ftpsMutexSem, WAIT_FOREVER);
    i = pSlot->ftpdTaskNum / 32;
    j = pSlot->ftpdTaskNum % 32;
    ftpdTaskMask[i] &= ~(1 << j);
    --ftpsCurrentClients;
    lstDelete (&ftpsSessionList, &pSlot->node);
 
    ftpdSockFree (&pSlot->dataSock);
    ftpdSockFree (&pSlot->cmdSock);
    ftpdSockFree (&pSlot->listenSock);
 
    free (pSlot);
 
    /* Send required signal if all sessions are closed. */
 
    if (ftpsShutdownFlag)
        {
        if (ftpsCurrentClients == 0)
            semGive (ftpsSignalSem);
        }
    semGive (ftpsMutexSem);
 
    return;
    }
 
/*******************************************************************************
*
* pathcmp - String compare ignore case.
*
* Check to see if s2 is a sub-directory of s1.
*
* RETURNS: difference in strings
*
* RETURNS: OK
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS pathcmp
    (
    char *s1,
    char *s2
    )
    {
    int checkCase = 1;
    int rootCheckFlag = 1; 


    if ( FTPD_DEBUG_ACCESS )
        ftpdDebugMsg ("pathcmp (<%s>, <%s>)\n", s1, s2 );


    if (*s1 == '/' && strlen (s1) == 1)
        rootCheckFlag = 1;


    FOREVER
        {
        /*
         * If we reach the end of the first string, then
         * that is all we care about...
         */
        if ( *s1 == 0 )
            {
            /*
             * here we check that the path is the same path or a
             * subdirectory.  Without this check, they might be
             * able to write to "/hd0/foobar" even though the
             * write path is "/hd0/foo"
             */
            if (rootCheckFlag == 1 && (strlen (s2) >= 1))
                return 0;
            if ( *s2 == 0 || *s2 == '/' )
                return 0;
            else
                return ERROR;
            }
 
        /* If strings differ, return ERROR  */
        if ( checkCase && *s1 != *s2 )
            return ERROR;
        else if ( toupper( *s1 ) != toupper( *s2 ) )
            return ERROR;
 


        /* Advance to the next character to compare */
        s1++;
        s2++;
 
        /*
         * In vxWorks, device names are case-sensitive, however,
         * DOS FS is not.  So, while /hd/foo and /hd/FOO are the
         * same file, /hd/foo and /HD/foo are not.  Go Figure...
         */
        if ( *s1 == '/' )
            {
            checkCase = 0;
            }
        }
    }
 
/*******************************************************************************
*
* ftpPathAccessVerify - verify client access to a path
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpPathAccessVerify
    (
    FTPD_SESSION_DATA *pSlot,
    char * path,
    int mode
    )
    {
 
    /* If user supplied path verify installed, use it for non guest */
    if ( ftpdPathAccessVerifyRtn != NULL && (pSlot->status & FTPD_ANONYMOUS) == 0 )
        {
        if ( FTPD_DEBUG_ACCESS )
            ftpdDebugMsg ("PathAccessVerify Calling user verify function\n");
 
        return (*ftpdPathAccessVerifyRtn)(pSlot->usrName, path, mode);
        }
 
    /* If explicit login was required, and this is not a guest user, allow */
    if( ( loginVerifyRtn != NULL ) && (pSlot->status & FTPD_ANONYMOUS) == 0 )
        {
        if ( FTPD_DEBUG_ACCESS )
            ftpdDebugMsg ("PathAccessVerify OK for non guest\n");
        return OK;
        }
 
    /* Read access always allowed */
    if( mode == O_RDONLY )
        {
        if ( FTPD_DEBUG_ACCESS )
            ftpdDebugMsg ("PathAccessVerify OK for read only\n");
        return OK;
        }
 
    /* Writing not allowed unless specified */
    if (writeDirName[0] == EOS)
        {
        if ( FTPD_DEBUG_ACCESS )
            ftpdDebugMsg ("PathAccessVerify fails - no write dir\n");
        return ERROR;
        }
 
    return pathcmp( writeDirName, path );
    }
 
 
 
/*******************************************************************************
*
* ftpStripDblSlah - Strip the leading double-slash from text containing a path
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL void ftpStripDblSlash (char * buf)
    {
    int fr = 0, to = 0, done = 0;
    if (!buf) 
        return;
    do
        {
        if (strncmp (&buf[fr], "file://", 2) == 0)
           ++fr;
        done = (buf[fr] == EOS);
        buf[to++] = buf[fr++];
        }
    while (!done);
    }
 
/*******************************************************************************
*
* ftpPathNormalize - process path provided by client for use
*
* This routine now prepends the anonymous root directory if the session is
* anonymous (this allows for a true "virtual" root directory for anonymous
* users).
*
* Parameters:
*    pSlot    - current FTP session context
*    path     - path to be appended to current path
*    currPath - buffer to copy current (absolute) path into
*    newPath  - buffer to copy new normalized (absolute) path into
*    pAbsPath - pointer to placeholder for new absolute path
*    pRelPath - pointer to placeholder for new relative path
*                 (i.e. portion of path visible to anonymous user -
*                  equivalent to pAbsPath if user not anonymous)
*
* The user may leave pAbsPath and/or pRelPath NULL if they are of no use.
* (Note: the reason for passing in currPath and newPath, even though they
* may not be useful to the user, is so that this function does not have to
* allocate and deallocate hundreds of bytes of stack space each time this
* function is called.)
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*
*
*/
LOCAL void ftpPathNormalize
    (
    FTPD_SESSION_DATA   *pSlot,
    const char *        argPath,
    char *        		currPath,
    char *              newPath,
    char **             pAbsPath,
    char **             pRelPath
    )
    {
    int relOffset = 0;
 
    /* If you're wondering why this code walks on eggshells around anonymous
       paths, I'll explain.  The anonymous user always needs to believe that
       he is rooted off the root directory, that is, by virtue of 'cd /', he
       should find himself in the anonymous root directory, as specified by
       ftpdAnonymousAllow().  A simple example of an implementation of
       anonymous access in a target is to set the base of a file system as
       the anonymous root, such as "/tffs0/" in the case of TrueFFS.
 
       One doesn't want to set the root directory to "/tffs0", as this will
       generate an error (try ll "/tffs0" on a TrueFFS-enabled system and see).
       However, "/tffs0/foo" (assuming foo exists) is a valid root directory,
       and yet it's difficult to generate a consistent virtual path (what
       if you're in the anonymous root - where does the "/" come from?).
 
       To take care of this, ftpdAnonymousAllow() automatically adds a slash
       to the end of whatever anonymous root is given.  This makes composing
       anonymous paths easier; when the user is in the anonymous root (i.e.
       pSlot->curDir == "/"), the absolute path is just rootDirName, with the
       relative path pointing at the slash at the end.  when composing more
       complex pathnames, or any non-anonymous pathname, pathCat composes
       correct pathnames.  For non-anonymous accesses, special restrictions
       really aren't that necessary, since the user can specify any
       absolute or relative path they want.  Whew.  -- DLR */
 

	 /* if we're requesting the current directory, leave path blank */
	 const char * path = ( strcmp(argPath, ".") == 0 ) ? "\0" : argPath;
 
    /* the anonymous case */
    if (pSlot->status & FTPD_ANONYMOUS)
        {
        /* First, compose our current path. */
        strcpy (currPath, rootDirName);
        if (strcmp (pSlot->curDir, "/") != 0)
           {
           strcat (currPath, pSlot->curDir);
           pathCondense (currPath);
           ftpStripDblSlash (currPath);
           }
        /* Let's find out if we're in, or have reached, the root of the
           anonymous file system (using currPath as a workspace). */
        pathCat (pSlot->curDir, (char *)path, newPath);
        pathCondense (newPath);
        if (strcmp (newPath, "/") == 0)
            {
            strcpy (newPath, rootDirName);
            }
        else
            {
            strcpy (newPath, currPath);
            strcat (newPath, "/");
            strcat (newPath, path);
            pathCondense (newPath);
            ftpStripDblSlash (newPath);
            }
        relOffset = rootDirNameLength - 1; /* leave out final slash */
        }
    else
        {
 
        if (path[0] == '/')
            {
            strcpy (newPath, path);
            }
        else
            {
            strcpy (currPath, pSlot->curDir);
            pathCat (currPath, (char *)path, newPath);
            }
        pathCondense (newPath);
        }
 
    if (pAbsPath)
        *pAbsPath = newPath;
    if (pRelPath)
        *pRelPath = newPath + relOffset;
    }
 
/*******************************************************************************
*
* ftpdProcessUser - Process the USER command
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessUser
    (
    FTPD_SESSION_DATA   *pSlot, /* pointer to the active slot to be handled */
    const char *cmd             /* index where command was found in table */
    )
    {
    char *pBuf = &pSlot->buf[0];
 
    /* check user name */
 
    /* 
     * Actually copy the user name into a buffer and save it 
     * till the password comes in. Name is located one space 
     * character after USER string 
     */
 
    if (*(pBuf + 4) == '\0')
        pSlot->usrName[0] = '\0';       /* NOP user for null user */
    else
        strncpy (pSlot->usrName, pBuf+5, MAX_LOGIN_NAME_LEN);
 
    pSlot->status &= ~(FTPD_USER_OK | FTPD_ANONYMOUS);
 
    if ((strcmp (pSlot->usrName, FTPD_ANONYMOUS_USER_1) ==0) ||
        (strcmp (pSlot->usrName, FTPD_ANONYMOUS_USER_2) ==0) ||
        (strcmp (pSlot->usrName, FTPD_ANONYMOUS_USER_3) ==0)    )
        {
        pSlot->status |= FTPD_ANONYMOUS ;       /* tentative */
        ftpdCmdSend (pSlot, pSlot->cmdSock, 331, messages [MSG_GUEST_PASS]);
        return OK;
        }
 
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 331, messages [MSG_PASSWORD_REQUIRED]);
    }
 
/*******************************************************************************
*
* ftpdProcessPass - Process the PASS command
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessPass
    (
    FTPD_SESSION_DATA   *pSlot, /* pointer to the active slot to be handled */
    const char *cmd             /* index where command was found in table */
    )
    {
    char *pBuf = &pSlot->buf[0];
 
    /* check user passwd */
    if (pSlot->status & FTPD_ANONYMOUS)
    {
      /* current dir is relative to rootDirName for Anonymous */
      strcpy (pSlot->curDir, "/");
 
      if ((rootDirName[0] == EOS) || (pBuf[5]==EOS))
        {
        ftpdCmdSend (pSlot, pSlot->cmdSock, 530, messages [MSG_USER_LOGIN_FAILED]);
        return OK;
        }
 
      pSlot->status |= FTPD_USER_OK;
 
      if (rootDirNameLength > strlen (writeDirName))
          pSlot->newDirName = writeDirName;
      else
          pSlot->newDirName = writeDirName + rootDirNameLength - 1;
 
      if (writeDirName[0] == EOS)
          ftpdCmdSend (pSlot, pSlot->cmdSock, 230, messages [MSG_GUEST_OK]);
      else
          ftpdCmdSend (pSlot, pSlot->cmdSock, 230, messages [MSG_GUEST_UPLOAD_OK],
                       pSlot->newDirName);
      return OK;
    }
    if ( ftpdLoginToCurrentDirectory )
        ioDefPathGet( pSlot->curDir );
    else
        strcpy (pSlot->curDir, "/");
 
    /* Actually check it against earlier supplied user name */
    if (loginVerifyRtn != (FUNCPTR) NULL)
        {
        if (FTPD_DEBUG_BASIC)
            ftpdDebugMsg ("(%d) Verifying login for user %s\n",__LINE__,
                          pSlot->usrName);
        if ((*loginVerifyRtn)(1, pSlot->usrName, pBuf+5) != OK)
            {
            if (ftpdCmdSend (pSlot, pSlot->cmdSock, 530, 
                             messages [MSG_USER_LOGIN_FAILED]) == ERROR)
                return ERROR;
            pSlot->status &= ~FTPD_USER_OK;
            return OK;
            }
        }
 
    /* 
     * if there is no login verification call, but anonymous logins
     * are enabled, accept anonymous connections _only_.
     */
    else if (rootDirName[0] != EOS)
    {
        if (ftpdCmdSend (pSlot, pSlot->cmdSock, 530, 
                         messages [MSG_USER_LOGIN_FAILED]) == ERROR)
            return ERROR;
        pSlot->status &= ~FTPD_USER_OK;
        return OK;
    }
 
    pSlot->status |= FTPD_USER_OK;
 
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 230, messages [MSG_USER_LOGGED_IN]);
    }
 
/*******************************************************************************
*
* ftpdProcessQuit - Process the QUIT command
*
* RETURNS: CLOSE
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessQuit
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    /* Send Goodby for QUIT, just close for ABOR */
    if ( strcmp(cmd,"QUIT") == 0 )
        ftpdCmdSend (pSlot, pSlot->cmdSock, 221, messages [MSG_SEE_YOU_LATER]);
    return CLOSE;
    }
 
/*******************************************************************************
*
* ftpdProcessSyst - Process the SYST command
*
* RETURNS: CLOSE
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessSyst
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    ftpdCmdSend (pSlot, pSlot->cmdSock, 215, messages [MSG_SYST_REPLY], getenv("TRIMA_BUILD"));
    return OK;
    }
 
/*******************************************************************************
*
* ftpdProcessHelp - Process the HELP command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessHelp
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    /* send list of supported commands with multiple line response */
 
    if (ftpdCmdSend (pSlot, pSlot->cmdSock, FTPD_MULTI_LINE | 214,
                     messages [MSG_COMMAND_LIST_BEGIN]) == ERROR)
    return ERROR;
 
    if (write (pSlot->cmdSock, ftpdCommandList,
               strlen (ftpdCommandList)) <= 0)
        return ERROR;
 
    /* this signifies the end of the multiple line response */
 
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 214, messages [MSG_COMMAND_LIST_END]);
    }
 
/*******************************************************************************
*
* ftpdProcessFeat - Process the FEAT command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessFeat
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
 
    /* send list of supported commands with multiple line response */
 
    if (ftpdCmdSend (pSlot, pSlot->cmdSock, FTPD_MULTI_LINE | 221,
                     messages [MSG_FEAT_LIST]) == ERROR)
        return ERROR;
 
    if (write (pSlot->cmdSock, ftpdExtendedFeatList,
               strlen (ftpdExtendedFeatList)) <= 0)
        return ERROR;
 
    /* this signifies the end of the multiple line response */
 
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 221, messages [MSG_FEAT_LIST_END]);
}
 
/*******************************************************************************
*
* ftpdProcessNoop - Process the NOOP command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessNoop
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    /* don't do anything */
 
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 200, messages [MSG_NOOP_OKAY]);
    }
 
/*******************************************************************************
*
* ftpdProcessPasv - Process the PASV command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessPasv
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    /* we need to open a socket and start listening on it
     * to accommodate his request.
     */
    if (pSlot->listenSock == FTPD_SOCK_FREE)
        {
        int addrLen = sizeof (struct sockaddr);
        struct sockaddr_in dataAddr;
 
        pSlot->listenSock = socket (AF_INET, SOCK_STREAM, 0);
        getsockname (pSlot->cmdSock, (struct sockaddr *) &dataAddr, &addrLen);
        dataAddr.sin_port = htons (0);
 
        addrLen = sizeof (struct sockaddr_in);
        bind (pSlot->listenSock, (struct sockaddr *)&dataAddr, sizeof (struct sockaddr_in));
        getsockname (pSlot->listenSock,(struct sockaddr *) &dataAddr, &addrLen);
        listen (pSlot->listenSock, 1);
 
        pSlot->listen[0] = ((u_char *)&(dataAddr.sin_addr.s_addr))[0];
        pSlot->listen[1] = ((u_char *)&(dataAddr.sin_addr.s_addr))[1];
        pSlot->listen[2] = ((u_char *)&(dataAddr.sin_addr.s_addr))[2];
        pSlot->listen[3] = ((u_char *)&(dataAddr.sin_addr.s_addr))[3];
 
        /* Separate port number into bytes. */
 
        pSlot->listen[4] = ((u_char *)&dataAddr.sin_port)[0];
        pSlot->listen[5] = ((u_char *)&dataAddr.sin_port)[1];
        }
    pSlot->status |= FTPD_PASSIVE;
 
    /* tell the client to which port to connect */
 
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 227, messages [MSG_PASSIVE_MODE],
                        pSlot->listen[0], pSlot->listen[1],
                        pSlot->listen[2], pSlot->listen[3],
                        pSlot->listen[4], pSlot->listen[5]);
    }
 
/*******************************************************************************
*
* ftpdProcessAllo - Process the ALLO and ACCT commands
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessAllo
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 202, messages [MSG_ALLOC_ACCOUNT]);
    }
 
/*******************************************************************************
*
* ftpdProcessXpwd - Process the PWD and XPWD commands
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessXpwd
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    ftpPathNormalize (pSlot, ".", pSlot->currPath, pSlot->newPath, NULL, &pSlot->newDirName);
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 257, messages [MSG_CUR_DIR], pSlot->newDirName);
    }
 
/*******************************************************************************
*
* ftpdProcessMode - Process the MODE command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessMode
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    /* specify transfer mode */
 
    /* we only support stream mode -- no block or compressed mode */
 
    if (toupper (pSlot->buf[5]) == 'S')
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 200, messages [MSG_STREAM_MODE]);
        }
    else
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 504, messages [MSG_PARAM_BAD]);
        }
    }
 
/*******************************************************************************
*
* ftpdProcessStru - Process the STRU command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessStru
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    /* specify the file structure */
 
    /* we only support normal byte stream oriented files;
     * we don't support IBM-ish record block oriented files 
     */
 
    if (toupper (pSlot->buf[5]) == 'F')
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 200, messages [MSG_FILE_STRU]);
        }
    else
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 504, messages [MSG_PARAM_BAD]);
        }
    }
 
/*******************************************************************************
*
* ftpdProcessList - Process the LIST and NLST commands
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessList
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    STATUS retVal;
    char *pBuf = &pSlot->buf[5];
    int flags = (strcmp (cmd,"NLST") ? FTPD_LIST_LONG : 0);
 
    if (pBuf[0] == '-') 
        {
        pBuf++; /* Skip flags indicator */
 
        while (*pBuf != '\0')
            {
            if (isspace (((unsigned char)(*pBuf))))
  {
                pBuf++;
                continue;
                }
 
            switch (*pBuf) 
                {
                case 'l':
                flags |= FTPD_LIST_LONG;
                break;
 
                case 'g':
                flags |= FTPD_LIST_GROUP;
                break;
 
                case 'a':
                flags |= FTPD_LIST_ALL;
                break;
 
                case 'F':
                flags |= FTPD_LIST_MARK;
                break;
 
                default:
                if (FTPD_DEBUG_PATH)
                    ftpdDebugMsg ("Unimplemented LIST flag '%c'\n", *pBuf);
                }
            pBuf++;
            }
        } 
    else
        {
        flags |= FTPD_LIST_GROUP | FTPD_LIST_ALL;
        }
 
    /* client wants to list out the contents of a directory */
  
    ftpPathNormalize (pSlot, pBuf, pSlot->currPath, pSlot->newPath, &pSlot->dirName, NULL);
 
    if (FTPD_DEBUG_PATH)
        ftpdDebugMsg ("(%d) listing %s\n",__LINE__, pSlot->dirName);
 
    if (ftpPathAccessVerify (pSlot, pSlot->dirName, O_RDONLY) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_GUEST_ACCESS]);
        }
 
    /* 
     * get a new data socket connection for the transmission of
     * the directory listing data
     */
 
    if (ftpdDataConnGet (pSlot) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 426, messages [MSG_DATA_CONN_ERROR]);
        }
 
    /* print out the directory contents over the data connection */
 
    if (ftpdDirListGet (pSlot, pSlot->dirName, flags) == ERROR)
        {
        retVal = ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_DIR_ERROR]);
        ftpdSockFree (&pSlot->dataSock);
        }
    else
        {
        retVal = transferOkay(pSlot);   /* We sent data, so linger must be non-zero */
        }
 
    return retVal;
    }
 
/*******************************************************************************
*
* ftpdProcessRetr - Process the RETR, SIZE and MDTM commands
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessRetr
    (
    FTPD_SESSION_DATA   *pSlot, /* pointer to the active slot to be handled */
    const char *cmd                     /* index where command was found in table */
    )
    {
    char                *pFileName;
    struct stat fileStat;
 
    /* retrieve a file */
 
    /* open the file to be sent to the client */
 
    ftpPathNormalize (pSlot, &pSlot->buf[5], pSlot->currPath, pSlot->newPath, &pFileName, NULL);
 
    printf ("FTP RETR <%s>\n", pFileName);

    if (FTPD_DEBUG_GET)
        ftpdDebugMsg ("(%d) accessing <%s>\n",__LINE__, pFileName);
 
    if (strcmp (pFileName,"/") == 0 && strcmp (cmd, "RETR"))
        {
        if (strcmp (cmd, "SIZE") == 0)
            {
            return ftpdCmdSend (pSlot, pSlot->cmdSock, 213, "512");
            }
        else
            {
            return ftpdCmdSend (pSlot, pSlot->cmdSock, 213, "19700101000000");
            }
        }
 
    if (ftpPathAccessVerify (pSlot, pFileName, O_RDONLY) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_GUEST_ACCESS]);
        }
 
    if (stat (pFileName, &fileStat) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_FILE_ERROR], (&pSlot->buf[5]));
        }
 
    if (!S_ISREG (fileStat.st_mode))
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_FILE_NOTREG], (&pSlot->buf[5]));
        }
 
    /* decide what to do with the file according to the actual cmd */
 
    if (strcmp (cmd, "RETR") == 0)
        {
        /* ship it away */
        ftpdDataStreamSend (pSlot, pFileName);
        }
    else if (strcmp (cmd, "SIZE") == 0)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 213, "%lu", fileStat.st_size);
        }
    else if (strcmp (cmd, "MDTM") == 0)
        {
        struct tm fileDate;
        localtime_r (&fileStat.st_mtime, &fileDate);
 
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 213, "%04d%02d%02d%02d%02d%02d",
                            fileDate.tm_year+1900,
                            fileDate.tm_mon,
                            fileDate.tm_mday,
                            fileDate.tm_hour,
                            fileDate.tm_min,
                            fileDate.tm_sec);
        }
    return OK;
    }
 
/*******************************************************************************
*
* ftpdProcessStor - Process the STOR command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessStor
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    int         fd;
    char        *pFileName;
    STATUS  retVal ;
 
    /* store a file */
 
    ftpPathNormalize (pSlot, &pSlot->buf[5], pSlot->currPath, pSlot->newPath, 
                      &pFileName, NULL);
 
    printf ("FTP STOR %s\n", pFileName);

    if (FTPD_DEBUG_BASIC)
        ftpdDebugMsg ("(%d) STOR %s\n",__LINE__, pFileName);
 
    if (ftpPathAccessVerify (pSlot, pFileName, O_WRONLY) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_GUEST_ACCESS]);
        }
 
    /* create a local file */
    if ((fd = open (pFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0 )
        {
        ftpdDebugMsg ("(%d) STOR %s failed, errno = %x\n",__LINE__, 
                      pFileName, errno );
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 553, messages[MSG_CREATE_ERROR], 
                            (&pSlot->buf[5]));
        }
 
    /* receive the file */
 
    retVal = ftpdDataStreamReceive (pSlot, fd);
    close (fd);
 
    /* remove lame file */
 
    if (retVal == ERROR)
        {
        unlink (pFileName);
        }
    return OK;
    }
 
/*******************************************************************************
*
* ftpdProcessRnfr - Process the RNFR command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessRnfr
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    char *pFileName;
    struct stat st;     /* for stat () system call */
 
    pSlot->renFile[0] = EOS ;
 
    ftpPathNormalize (pSlot, &pSlot->buf[5], pSlot->currPath, pSlot->newPath, &pFileName, NULL);
 
    if (FTPD_DEBUG_BASIC)
        ftpdDebugMsg ("(%d) RNFR %s\n",__LINE__, pFileName);
 
    if (ftpPathAccessVerify (pSlot, pFileName, O_RDWR) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_GUEST_ACCESS]);
        }
 
    if (stat (pFileName, &st) != OK)
       {
       return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_FILE_ERROR], pFileName);
       }
 
    /* store file name for the next command which should be RNTO */
    strncpy (pSlot->renFile, pFileName, MAX_FILENAME_LENGTH);
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 350, messages [MSG_RNFR_OK]);
    }
 
/*******************************************************************************
*
* ftpdProcessRnto - Process the RNTO command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessRnto
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    char *pFileName;
 
    ftpPathNormalize (pSlot, &pSlot->buf[5], pSlot->currPath, pSlot->newPath, 
                      &pFileName, NULL);
 
    if (FTPD_DEBUG_BASIC)
        ftpdDebugMsg ("(%d) RNTO %s\n",__LINE__, pFileName);
 
    if (ftpPathAccessVerify (pSlot, pFileName, O_WRONLY) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_GUEST_ACCESS]);
        }
 
    if (rename (pSlot->renFile, pFileName) != OK)
        {
        pSlot->renFile[0] = EOS ;
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_DIR_ERROR], 
                            &pSlot->buf[5]);
        }
 
    pSlot->renFile[0] = EOS ;
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 250, messages [MSG_RNTO_OK]);
}
 
/*******************************************************************************
*
* ftpdProcessDele - Process the DELE command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessDele
    (
    FTPD_SESSION_DATA   *pSlot, /* pointer to the active slot to be handled */
    const char *cmd             /* index where command was found in table */
    )
    {
    char                *pFileName;
    struct stat fileStat;
 
    ftpPathNormalize (pSlot, &pSlot->buf[5], pSlot->currPath, pSlot->newPath, &pFileName, NULL);
    if (FTPD_DEBUG_BASIC)
        ftpdDebugMsg ("(%d) DELE %s\n",__LINE__, pFileName);
 
    if (ftpPathAccessVerify (pSlot, pFileName, O_WRONLY) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_GUEST_ACCESS]);
        }
 
    if (stat (pFileName, &fileStat) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_FILE_ERROR], (&pSlot->buf[5]));
        }
 
    if (! S_ISREG (fileStat.st_mode))
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_FILE_NOTREG], (&pSlot->buf[5]));
        }
    if (remove (pFileName) != OK)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_FILE_ERROR], &pSlot->buf[5]);
        }
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 250, messages [MSG_DELE_OK]);
    }
 
/*******************************************************************************
*
* ftpdProcessXmkd - Process the MKD and XMKD commands
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessXmkd
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    char *pFileName;
 
    ftpPathNormalize (pSlot, &pSlot->buf[ strlen (cmd) + 1 ], pSlot->currPath, 
                      pSlot->newPath, &pFileName, NULL);
    if (FTPD_DEBUG_BASIC)
        ftpdDebugMsg ("(%d) MKD %s\n",__LINE__, pFileName);
 
    if (ftpPathAccessVerify (pSlot, pFileName, O_WRONLY) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_GUEST_ACCESS]);
        }
 
    if (mkdir (pFileName) != OK)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_MKD_ERROR],
                            &pSlot->buf[4]);
        }
    if (strcmp (cmd,"MKD") == 0)
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 200, messages [MSG_MKD_OK],
                            &pSlot->buf[4]);
    if (strcmp (cmd,"XMKD") == 0)
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 200, messages [MSG_MKD_OK],
                            &pSlot->buf[5]);
    return (ERROR); /* Invalid cmd option */
    }
 
/*******************************************************************************
*
* ftpdProcessXrmd - Process the RMD and XRMD commands
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessXrmd
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    struct stat fileStat;
    char *pFileName;
    char *pDir;
    pDir = &pSlot->buf[ strlen (cmd) + 1];
 
    ftpPathNormalize (pSlot, pDir, pSlot->currPath, pSlot->newPath, &pFileName, NULL);
    if (FTPD_DEBUG_BASIC)
        ftpdDebugMsg ("(%d) RMD %s\n",__LINE__, pFileName);
 
    if (ftpPathAccessVerify (pSlot, pFileName, O_WRONLY) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_GUEST_ACCESS]);
        }
 
    if (stat (pFileName, &fileStat) == ERROR)
        {
        if (strcmp (cmd, "RMD") == 0)
            return ftpdCmdSend (pSlot, pSlot->cmdSock, 550,
                                messages [MSG_FILE_ERROR], (&pSlot->buf[4]));
        else if (strcmp (cmd, "XRMD") == 0)
            return ftpdCmdSend (pSlot, pSlot->cmdSock, 550,
                                messages [MSG_FILE_ERROR], (&pSlot->buf[5]));
        }
 
    if (! S_ISDIR (fileStat.st_mode))
        {
        if (strcmp (cmd, "RMD") == 0)
            return ftpdCmdSend (pSlot, pSlot->cmdSock, 550,
                                messages [MSG_FILE_NOTDIR], (&pSlot->buf[4]));
        else if (strcmp (cmd, "XRMD") == 0)
            return ftpdCmdSend (pSlot, pSlot->cmdSock, 550,
                                messages [MSG_FILE_NOTDIR], (&pSlot->buf[5]));
        }
 
    if (rmdir (pFileName) != OK)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_FILE_ERROR], pDir);
        }
 
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 250, messages [MSG_RMD_OK], pDir);
}
 
/*******************************************************************************
*
* ftpdProcessXcwd - Process the CWD, CDUP, XCWD and XCUP commands
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessXcwd
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    struct stat st;     /* for stat() system call */
    /* change directory */
 
    if (strcmp (cmd, "CWD") == 0)
        ftpPathNormalize (pSlot, &pSlot->buf[4], pSlot->currPath, pSlot->newPath, &pSlot->dirName, &pSlot->newDirName);
    else if (strcmp (cmd, "XCWD") == 0)
        ftpPathNormalize (pSlot, &pSlot->buf[5], pSlot->currPath, pSlot->newPath, &pSlot->dirName, &pSlot->newDirName);
    else
        ftpPathNormalize (pSlot, "..", pSlot->currPath, pSlot->newPath, &pSlot->dirName, &pSlot->newDirName);
 
    if (ftpPathAccessVerify (pSlot, pSlot->dirName, O_RDONLY) == ERROR)
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_GUEST_ACCESS]);
        }
 
    /* verify that this is indeed a directory we can handle */
 
    if (strcmp (pSlot->dirName, "/") && ((stat (pSlot->dirName, &st) != OK) || !S_ISDIR (st.st_mode)))
        {
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 501, messages[MSG_DIR_NOT_PRESENT]);
        }
 
    /* remember where we are */
    (void) strncpy (pSlot->curDir, pSlot->newDirName, MAX_FILENAME_LENGTH);
 
    /* notify successful chdir */
 
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 250, messages [MSG_CHANGED_DIR], pSlot->newDirName);
}
 
/*******************************************************************************
*
* ftpdProcessType - Process the TYPE command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessType
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    char mode = toupper (pSlot->buf[5]);
 
    /* we only support BINARY and ASCII representation types */
 
    if (mode == 'I' || mode == 'L')
    {
        pSlot->status |= FTPD_BINARY_TYPE;
        pSlot->status &= ~FTPD_ASCII_TYPE;
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 200, messages [MSG_TYPE_BINARY]);
    }
    else if (mode == 'A')
        {
        pSlot->status |= FTPD_ASCII_TYPE;
        pSlot->status &= ~FTPD_BINARY_TYPE;
        return ftpdCmdSend (pSlot, pSlot->cmdSock, 200, messages [MSG_TYPE_ASCII]);
        }
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 504, messages [MSG_PARAM_BAD]);
}
 
/*******************************************************************************
*
* ftpdProcessPort - Process the PORT command
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL STATUS ftpdProcessPort
    (
    FTPD_SESSION_DATA *pSlot, /* pointer to the active slot to be handled */
    const char *cmd           /* index where command was found in table */
    )
    {
    int portNum[6];
 
    /* client specifies the port to be used in setting up
     * active data connections later on (see ftpdDataConnGet ()).
     * format:  first four decimal digits separated by commas
     * indicate the internet address; the last two decimal
     * digits separated by a comma represents hi and low
     * bytes of a port number.
     */
 
    (void) sscanf (&pSlot->buf [5], "%d,%d,%d,%d,%d,%d",
                   &portNum[0], &portNum[1], &portNum[2],
                   &portNum[3], &portNum[4], &portNum[5]);
 
    /* convert port number to network byte order */
 
    pSlot->clientPort = htons ( portNum [4] * 256 + portNum [5]);
 
    /* Set remote host to given value. */
 
    pSlot->clientIp = 
        htonl ((portNum [0] << 24) | 
               (portNum [1] << 16) | 
               (portNum [2] << 8)  | 
                portNum [3]);
 
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 200, messages [MSG_PORT_SET]);
    }
 
/*******************************************************************************
*
* ftpdExecuteCommand - Locate the command in the table, and execute it.
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
 
LOCAL STATUS ftpdExecuteCommand 
    (
    FTPD_SESSION_DATA *pSlot
    ) 
    {
    int bFirst;
    int bMid;
    int bSize;
    int i;
    char cmd[5];
 
    for (i = 0; i < 5 ; i++)
        {
        if ( pSlot->buf[i] <= ' ' )
            {
            cmd[i] = '\0';
            break;
            }
        cmd[i] = toupper (pSlot->buf[i]);
        }
 


    bFirst = 0;
    bSize = FTPD_NUM_CMDS;
 
    /* 
     * Moved here to time-slice once per command, instead of once per byte
     * as the original server did.  This is much more efficient 
     */
 
    taskDelay (0);
 
    /* Using a binary search,  find the command and execute it */
 
    while( bSize > 0 )
        {
        bMid = bFirst + (bSize / 2);
        i = strcmp( cmd, ftpdCommandTable[bMid].command );
        if ( i == 0 )
            {
            /*
             * Found the command.  Now, check that USER_OK
             * is set if command requires a session
             */
            if (ftpdCommandTable[bMid].requiresSession &&
                (pSlot->status & FTPD_USER_OK) == 0)
                {
                  return ftpdCmdSend (pSlot, pSlot->cmdSock, 530, messages [MSG_USER_PASS_REQ]);
                }
            return (*ftpdCommandTable[bMid].cmd) (pSlot, cmd);
            }
        else if ( i < 0 )
            {
            bSize = bSize / 2;
            }
        else
            {
              bFirst = bMid + 1;
              bSize = (bSize - 1) / 2;
            }
        }
    return ftpdCmdSend (pSlot, pSlot->cmdSock, 500, messages [MSG_BAD_COMMAND]);
    }
 


/*******************************************************************************
*
* ftpdWorkTask - main protocol processor for the FTP service
*
* This function handles all the FTP protocol requests by parsing
* the request string and performing appropriate actions and returning
* the result strings.  The main body of this function is a large
* FOREVER loop which reads in FTP request commands from the client
* located on the other side of the connection.  If the result of
* parsing the request indicates a valid command, ftpdWorkTask () will
* call appropriate functions to handle the request and return the
* result of the request.  The parsing of the requests are done via
* a list of strncmp routines for simplicity.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*
* INTERNAL
* To handle multiple simultaneous connections, this routine and all secondary
* routines which process client commands must be re-entrant. If the server's
* halt routine is started, the shutdown flag is set, causing this routine to
* exit after completing any operation already in progress.
*/
 
LOCAL STATUS ftpdWorkTask 
    (
    FTPD_SESSION_DATA *pSlot
    )   
    {
    unsigned char       tmpBuf[80];
    FAST char           *pBuf, *pBufEnd; /* pointer to session specific buffer */
    FAST int            numRead, index;
    FAST int            result;
    int                 gotLine;
    fd_set              readFds;
    struct timeval      timeOut;
 
    timeOut.tv_sec = ftpdClientSessionTimeout;
    timeOut.tv_usec = 0;
 
    pBuf = &pSlot->buf[0];      /* use session specific buffer area */
    pBufEnd = &pSlot->buf[ sizeof (pSlot->buf) - 1 ];
 
    if ( ftpsShutdownFlag ||
         (loginVerifyRtn && (*loginVerifyRtn)(0) != OK) )
        {
 
        /* Server halt in progress - send abort message to client. */
 
        ftpdCmdSend (pSlot, pSlot->cmdSock, 421,
                     "Service not available, closing control connection");
        ftpdSessionDelete (pSlot);
        return (OK);
        }
 
    /* tell the client we're ready to rock'n'roll */
 
    if (ftpdCmdSend (pSlot, pSlot->cmdSock, 220, messages [MSG_SERVER_READY], vxWorksVersion) == ERROR)
        {
        ftpdSessionDelete (pSlot);
        return (ERROR);
        }
 
    /* Clean the index & count for out local command buffer */
 
    index = 0;
    numRead = 0;
 
    FOREVER 
        {
 
        /* Check error in writting to the control socket */
        pSlot->byteCount = 0 ;
 
        /*
         * Stop processing client requests if a server halt is in progress.
         * These tests of the shutdown flag are not protected with the
         * mutual exclusion semaphore to prevent unnecessary synchronization
         * between client sessions. Because the secondary tasks execute at
         * a lower priority than the primary task, the worst case delay
         * before ending this session after shutdown has started would only
         * allow a single additional command to be performed.
         */
        if ( ftpsShutdownFlag ||
		       (loginVerifyRtn && (*loginVerifyRtn)(0) != OK) )
            break;

        /* Set pointer to copy characters */
        pBuf = &pSlot->buf [0];
        memset( pBuf, 0, 80 );
 
        /* get a request command */
        gotLine = FALSE;
 
        FOREVER
            {
 
            /*
             * We want to call select() instead of read() because read does not
             * timeout like select does.  Also, we have set the command connection
             * to be non-blocking, again, makeing select() necessary to avoid
             * polling.  However, we do not want to call select/read on a per-character
             * basis, so we have a little 80-byte buffer.  First we empty any data left
             * the buffer before proceeding to select()/read().  The client may have
             * sent multiple commands without waiting for a responce 
             */
 
            while( index < numRead )
            {
                /* Skip Telnet flags, we are not running under telnetd */
                if (tmpBuf[index] == '\r' || tmpBuf[index] > 0x0F0 )
                    {
                    index++;
                    continue;
                    }
                /* 
                 * End Of Command delimeter. exit loop and process command 
                 * also check buffer bounds 
                 */
 
                if ((tmpBuf[index] == '\n') || (pBuf == pBufEnd)) 
                    {
                    *pBuf = EOS;
                    index++;
                    gotLine = TRUE;
                    break;
                    }
                *pBuf++ = tmpBuf[index++]; /* Advance to next character to read */
            }
 
            if ( gotLine )
                break;
 
            FD_ZERO (&readFds);
            FD_SET  (pSlot->cmdSock, &readFds);
            index = 0;
            numRead = 0;
 
            /* 
             * if no message is received from client in the timeout period
             * the select will unpend and this ftp server task self-terminates 
             */
 
            if ((select (FD_SETSIZE, &readFds, NULL, NULL, &timeOut)) != 0) 
                {
 
                if ( FD_ISSET(pSlot->cmdSock, &readFds) )
                    {
                    if ( (numRead = read (pSlot->cmdSock, tmpBuf, sizeof(tmpBuf) )) <= 0 )
                        {
                        /*
                         * The primary server task will close the control connection
                         * when a halt is in progress, causing an error on the socket.
                         * In this case, ignore the error and exit the command loop
                         * to send a termination message to the connected client.
                         */
 
								if ( ftpsShutdownFlag ||
									  (loginVerifyRtn && (*loginVerifyRtn)(0) != OK) )
                            {
                            *pBuf = EOS;
                            break;
                            }
 
                        /*
                         * Send a final message if the control socket
                         * closed unexpectedly.
                         */
 
                        if (numRead == 0)
                            ftpdCmdSend (pSlot, pSlot->cmdSock, 221, messages [MSG_NO_GOOD_BYE]);
 
                        ftpdSessionDelete (pSlot);
                        return ERROR;
                        }
                    }
                } 
            else 
                {
                ftpdCmdSend (pSlot, pSlot->cmdSock, 221, 
                messages [MSG_SERVER_TIMEOUT], 0, 0, 0, 0, 0, 0);
                ftpdSessionDelete (pSlot);
                return OK;
                }
            } /* FOREVER - read line */
 
        /*  Reset Buffer Pointer before we use it */
 
        pBuf = &pSlot->buf [0];
 
        /*
         * Send an abort message to the client if a server
         * shutdown was started while reading the next command.
         */
 
        if ( ftpsShutdownFlag ||
		       (loginVerifyRtn && (*loginVerifyRtn)(0) != OK) )
            {
            ftpdCmdSend (pSlot, pSlot->cmdSock, 421,
                         "Service not available, closing control connection");
            break;
            }
 
        if (FTPD_DEBUG_CMD)
            ftpdDebugMsg ("FTP COMMAND: %s\n", pBuf);
 
        if ((result = ftpdExecuteCommand (pSlot)) == CLOSE) 
            break;
 
        if ((result == ERROR) || (pSlot->cmdSockError == ERROR))
            {
            if (FTPD_DEBUG_BASIC)
                ftpdDebugMsg ("(%d) FTP Control connection error, closed.\n", __LINE__);
            ftpdSessionDelete (pSlot);
            return (ERROR);
            }
        }
    /*
     * Processing halted due to pending server shutdown.
     * Remove all resources and exit.
     */
 
    ftpdSessionDelete (pSlot);
    return (OK);
    }
 
/*******************************************************************************
*
* ftpdDataConnGet - get a fresh data connection socket for FTP data transfer
*
* FTP uses upto two connections per session (as described above) at any
* time.  The command connection (cmdSock) is maintained throughout the
* FTP session to pass the request command strings and replies between
* the client and the server.  For commands that require bulk data transfer
* such as contents of a file or a list of files in a directory, FTP
* sets up dynamic data connections separate from the command connection.
* This function, ftpdDataConnGet, is responsible for creating
* such connections.
*
* Setting up the data connection is performed in two ways.  If the dataSock
* is already initialized and we're in passive mode (as indicated by the
* FTPD_PASSIVE bit of the status field in the FTPD_SESSION_SLOT) we need to
* wait for our client to make a connection to us -- so we just do an accept
* on this already initialized dataSock.  If the dataSock is already
* initialized and we're not in passive mode, we just use the already
* existing connection.  Otherwise, we need to initialize a new socket and
* make a connection to the the port where client is accepting new
* connections.  This port number is in general set by "PORT" command (see
* ftpdWorkTask ()).
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
 
LOCAL STATUS ftpdDataConnGet
    (
    FTPD_SESSION_DATA   *pSlot          /* pointer to the work slot */
    )
    {
    int                 newSock;
    int                 addrLen;        /* to be used with accept */
    struct sockaddr_in  addr;           /* to be used with accept */
    int                 retry = 0;      /* retry counter initialized to zero */
    int                 on = 1;
 
    /* command socket is invalid, return immediately */
 
    if (pSlot->cmdSock == FTPD_SOCK_FREE)
        return (ERROR);
 
    pSlot->byteCount = 0;
 
    /* 
     * are we being passive? (should we wait for client to connect
     * to us rather than connecting to the client?)
     */
 
    if (pSlot->status & FTPD_PASSIVE)
        {
 
        if (pSlot->dataSock != FTPD_SOCK_FREE)
            ftpdSockFree (&pSlot->dataSock);
 


        addrLen = sizeof (struct sockaddr);
        bzero ((void *)&addr, sizeof (addr));
 
        if (FTPD_DEBUG_BASIC)
            ftpdDebugMsg ("(%d) doing accept for passive fd=%d\n",__LINE__, 
                          pSlot->listenSock);
 
        if ((newSock = accept (pSlot->listenSock, 
            (struct sockaddr *) &addr, &addrLen)) < 0) 
            {
 
            /* we can't be passive no more */
 
            pSlot->status &= ~FTPD_PASSIVE;
 
            ftpdCmdSend (pSlot, pSlot->cmdSock, 425, "Can't open data connection");
            return ERROR;
 
            }
 
        /*
         * Enable the keep alive option to prevent misbehaving clients
         * from locking the server.
         */
 
        setsockopt (newSock, SOL_SOCKET, SO_KEEPALIVE, (char *) &on, sizeof (on));
 
        pSlot->dataSock = newSock;
 
        /* N.B.: we stay passive */
 
        if (ftpdCmdSend (pSlot, pSlot->cmdSock, 150, "Opening %s mode data connection",
            pSlot->status & FTPD_ASCII_TYPE ? "ASCII" : "BINARY") == ERROR)
            {
            ftpdSockFree (&pSlot->dataSock);
            return (ERROR);
            }
 
        return (OK);
        }
    else 
    if (pSlot->dataSock != FTPD_SOCK_FREE)
        {
        /* reuse the old connection -- it's still useful */
 
        if (ftpdCmdSend (pSlot, pSlot->cmdSock, 125, "Using existing data connection") == ERROR)
            {
            ftpdSockFree (&pSlot->dataSock);
            return (ERROR);
            }
        return (OK);
        }
    else
        {
 
        /* Set socket address to PORT command values or default. */
 
        pSlot->dataSock = socket (AF_INET, SOCK_STREAM, 0);
 
        if (pSlot->dataSock < 0)
            {
            return (ERROR);
            }
 
        bzero ((char *)&addr, sizeof (addr));
 
        addr.sin_addr.s_addr = pSlot->clientIp;
        addr.sin_port = pSlot->clientPort;
        addr.sin_family = AF_INET;
 
        /* try until we get a connection to the client's port */
 
        while (connect (pSlot->dataSock,
               (struct sockaddr *) &addr, sizeof (addr)) < 0)
            {
            if ((errno & 0xffff) == EADDRINUSE && retry < FTPD_WAIT_MAX)
                {
                taskDelay (FTPD_WAIT_INTERVAL * sysClkRateGet ());
                retry += FTPD_WAIT_INTERVAL;
                continue;
                }
 
            /* timeout -- we give up */
 
            ftpdCmdSend (pSlot, pSlot->cmdSock, 425, "Can't build data connection");
            ftpdSockFree (&pSlot->dataSock);
            return (ERROR);
 
            }
 
        /*
         * Enable the keep alive option to prevent misbehaving clients
         * from locking the secondary task during file transfers.
         */


        if (setsockopt (pSlot->dataSock, SOL_SOCKET, SO_KEEPALIVE,
            (char *) &on, sizeof (on)) != 0)
            {
            ftpdSockFree (&pSlot->dataSock);
            return (ERROR);
            }
 
        if (ftpdCmdSend (pSlot, pSlot->cmdSock, 150, "Opening %s mode data connection",
            pSlot->status & FTPD_ASCII_TYPE ? "ASCII" : "BINARY") == ERROR)
            {
            ftpdSockFree (&pSlot->dataSock);
            return (ERROR);
            }
        }
 
    return (OK);
    }
 
/*******************************************************************************
*
* ftpdDataStreamSend - send FTP data over data connection
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* INTERNAL: 
* When our FTP client does a "RETR" (send me a file) and we find an existing
* file, ftpdWorkTask() will call us to perform the actual shipment of the
* file in question over the data connection.
*
* We do the initialization of the new data connection ourselves here
* and make sure that everything is fine and dandy before shipping the
* contents of the file.  Special attention is given to the type of
* the file representation -- ASCII or BINARY.  If it's binary, we
* don't perform the prepending of "\r" character in front of each
* "\n" character.  Otherwise, we have to do this for the ASCII files.
*
* SEE ALSO:
* ftpdDataStreamReceive() which is symmetric to this function.
*
* NOMANUAL
*/
 
LOCAL void ftpdDataStreamSend
    (
    FTPD_SESSION_DATA   *pSlot,         /* pointer to our session slot */
    char                *fileName       /* pointer to the input file stream */
    )
    {
    FAST int    netFd;                  /* output socket */
    FAST int    fileFd;                 /* input file descriptor */
    FAST int    cnt;                    /* number of chars read/written */
    int         retval = 0;
 
    /* get a fresh connection or reuse the old one */
 
    if (ftpdDataConnGet (pSlot) == ERROR)
        {
        dataError (pSlot);
        return;
        }
 
    /* should not be a problem, cause we already did stat() on this file */
 
    if ((fileFd = open (fileName, O_RDONLY, 0)) == ERROR)
        {
        fileError (pSlot);
        return;
        }
 
    netFd = pSlot->dataSock;
 
    if (pSlot->status & FTPD_ASCII_TYPE)        /* ASCII representation */
        {
        char        ch;             /* character holder */
        int         fCnt;
        int         dCnt = 1;
 
        /* write out the contents of the file and do the '\r' prepending */
 
        while ( (fCnt = read (fileFd,&ch,1)) == 1)
            {
            pSlot->byteCount++;
            if ( (pSlot->byteCount % 1000) == 0 )
				{
					taskDelay(0);
				}
 
            /* if '\n' is encountered, we prepend a '\r' */
 
            if (ch == '\n')
                {
                if ( (dCnt = write (netFd,"\n",1)) != 1 )
                    {
                    break;
                    }
                }


            if ( (dCnt = write (netFd,&ch,1)) != 1)
                {
                break;
                }
 
            /* Abort the file transfer if a shutdown is in progress. */
 
            if (ch == '\n' && ftpsShutdownFlag)
                {
                break;
                }
            }
 
        /* everything is okay */
        if ( fCnt == 0 && dCnt == 1 )
            transferOkay (pSlot);
        else if ( dCnt != 1 )
            dataError (pSlot);
        else
            fileError (pSlot);
        }
    else if (pSlot->status & FTPD_BINARY_TYPE)  /* BINARY representation */
        {
 
        while ((cnt = read (fileFd, pSlot->buf, sizeof(pSlot->buf) )) > 0 &&
               (retval = write (netFd, pSlot->buf, cnt)) == cnt)
            {
            pSlot->byteCount += cnt;
            taskDelay(0);
 
            if (ftpsShutdownFlag)
                {
                /* Abort the file transfer if a shutdown is in progress. */
 
                cnt = 1;
                break;
                }
            }
 
        /* cnt should be zero if the transfer ended normally */
 
        if (cnt != 0)
            {
            if (cnt < 0)
                {
                fileError (pSlot);
                }
            else
                {
                if (FTPD_DEBUG_BASIC)
                    ftpdDebugMsg ("(%d) data xfer failed: read %d bytes, wrote %d bytes\n",
                                  __LINE__, cnt, retval);
 
                dataError (pSlot);
                }
            }
        else
            {
            transferOkay (pSlot); /* We sent data, so linger must be non-zero */
            }
        }
    else
        unImplementedType (pSlot);      /* invalide representation type */
 
    close (fileFd);
 
    }
 
/*******************************************************************************
*
* ftpdDataStreamReceive - receive FTP data over data connection
*
* RETURNS: OK or ERROR.
*
* ERRNO: N/A
*
* NOMANUAL
*
* INTERNAL:
* When our FTP client requests "STOR" command and we were able to
* create a file requested, ftpdWorkTask() will call ftpdDataStreamReceive
* to actually carry out the request -- receiving the contents of the
* named file and storing it in the new file created.
*
* We do the initialization of the new data connection ourselves here
* and make sure that everything is fine and dandy before receiving the
* contents of the file.  Special attention is given to the type of
* the file representation -- ASCII or BINARY.  If it's binary, we
* don't perform the handling of '\r' character in front of each
* '\n' character.  Otherwise, we have to do this for the ASCII files.
*
* SEE ALSO:
* ftpdDataStreamSend which is symmetric to this function.
*/
 
LOCAL STATUS ftpdDataStreamReceive
    (
    FTPD_SESSION_DATA   *pSlot,
    int         fileFd
    )
    {
    FAST char   *pBuf;          /* pointer to the session buffer */
    char        ch;             /* character holder */
    FAST int    netFd;          /* network file descriptor */
    FAST BOOL   dontPutc;       /* flag to prevent bogus chars */
    FAST int    cnt;            /* number of chars read/written */
 
    /* get a fresh data connection or reuse the old one */
 
    if (ftpdDataConnGet (pSlot) == ERROR)
        goto data_err;
 
    pBuf = &pSlot->buf [0];
    /* get a raw descriptor for output file stream */
 
    netFd  = pSlot->dataSock;
 
    if (pSlot->status & FTPD_ASCII_TYPE)
        {
        /* ASCII representation */
 
        while ( read( netFd, &ch, 1) == 1 )
            {
            dontPutc = FALSE;
 
            pSlot->byteCount++;
            
            /* a rather strange handling of sequences of '\r' chars */
 
            while (ch == '\r')
                {
 
                /* replace bogus chars between '\r' and '\n' chars with '\r' */
                cnt = read(netFd, &ch, 1);
                if ( ch != '\n' )
                    {
                    if ( write( fileFd, "\r", 1 ) != 1 )
                        goto data_err ;
 
                    if (ch == '\0' || cnt == 0)
                        {
                        dontPutc = TRUE;
                        break;
                        }
                    }
                }
 
            if (dontPutc == FALSE)
                if ( write( fileFd, &ch, 1 ) != 1 )
                    goto data_err ;
 
            /* Abort file transfer if a shutdown is in progress. */
 
            if (ch == '\n' && ftpsShutdownFlag)
                {
                goto data_err ;
                }
            } /* end of while getc */
 
        /* we've succeeded! */
 
        transferOkay (pSlot);   /* We received the data, so allow PCB to be cleaned immediatly */
        return OK ;
        }
    else if (pSlot->status & FTPD_BINARY_TYPE)
        {
        /* BINARY representation */
 
        /* perform non-buffered block I/O between network and file */
        /* lrn: replaced read () with fioRead so that file write in big chunks */
 
        while ((cnt = fioRead (netFd, pBuf, sizeof (pSlot->buf))) > 0)
            {
            if (write (fileFd, pBuf, cnt) != cnt)
                goto file_err ;
 
            pSlot->byteCount += cnt;
            taskDelay(0);
 
            /* Abort the file transfer if a shutdown is in progress. */
 
            if (ftpsShutdownFlag)
                {
                cnt = -1;
                break;
                }
            }
 
        if (cnt < 0)
            goto data_err ;
 
        /* we've done it */
 
        transferOkay (pSlot);   /* We received the data, so allow PCB to be cleaned immediately */
        return OK ;
        }
    else
        {
        unImplementedType (pSlot);      /* invalid representation type */
        return ERROR;
        }
 
file_err:
    fileError (pSlot);
    return ERROR;
 
data_err:
    dataError (pSlot);
    return ERROR;
    }
 
/*******************************************************************************
*
* ftpdSockFree - release a socket
*
* This function is used to examine whether or not the socket pointed
* by pSock is active and release it if it is.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
 
LOCAL void ftpdSockFree
    (
    int *pSock
    )
    {
    FTPD_SOCKET sock;
 
    /* Check to make sure not already closed... */
    if (*pSock == FTPD_SOCK_FREE)
        return;
 
    shutdown (*pSock, 2);  /* shutdown */
 
    sock.fd = *pSock;
    sock.closeTick = tickGet () + ftpdLingerTicks;      /* Close socket in FTPD_LINGER_SECONDS seconds */
 
    if (msgQSend (ftpdCloseQ, (void *)&sock, sizeof(sock), WAIT_FOREVER, MSG_PRI_NORMAL) != OK)
        {
        close (*pSock);
        }
    else if (FTPD_DEBUG_LINGER)
        {
        ftpdDebugMsg ("schedule close %d at %d\n", sock.fd, sock.closeTick);
        }
 
    *pSock = FTPD_SOCK_FREE;
    }
/*******************************************************************************
*
* ftpDirListPattern - match file name pattern with an actual file name
*
* RETURNS: TRUE or FALSE 
*
* ERRNO: N/A
*
* NOMANUAL
*
* INTERNAL:
* <pat> is a pattern consisting with '?' and '*' wildcard characters,
* which is matched against tha <name> filename, and TRUE is returned
* if they match, or FALSE if do not match. Both arguments must be
* nil terminated strings.
* The pattern matching is case inseisitive.
*
* NOTE: we're using EOS as integral part of the pattern and name.
*/
LOCAL BOOL ftpDirListPattern (char * pat, char * name)
    {
    FAST char * pPat, *pNext ;
    const char anyOne = '?' ;
    const char anyMany = '*' ;
 
    pPat = pat ;
    for (pNext = name ; * pNext != EOS ; pNext ++)
        {
        /* DEBUG-  logMsg ("pattern %s, name %s\n", pPat, pNext, 0,0,0,0);*/
        if (*pPat == anyOne)
            {
            pPat ++ ;
            }
        else if (*pPat == anyMany)
            {
            if (pNext[0] == '.')        /* '*' dont match . .. and .xxx */
                return FALSE ;
            if (toupper (pPat[1]) == toupper (pNext[1]))
                pPat ++ ;
            else if (toupper (pPat[1]) == toupper (pNext[0]))
                pPat += 2 ;
            else
                continue ;
            }
        else if (toupper (*pPat) != toupper (*pNext))
            {
            return FALSE ;
            }
        else
            {
            pPat ++ ;
            }
        }
 
    /* loop is done, let's see if there is anything left */
 
    if ((*pPat == EOS) || (pPat[0] == anyMany && pPat[1] == EOS))
        return TRUE ;
    else
        return FALSE ;
    }
 


/*******************************************************************************
*
* ftpFormatTime - Create a string containing the time
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*
*/
LOCAL void ftpdFormatTime 
    (
    time_t *fileTime, 
    char *buffer
    )
    {
    time_t      now;            /* current clock */
    struct tm   nowDate;        /* current date & time */
    struct tm   fileDate;       /* file date/time      (long listing) */
 
    /* Break down file modified time */
 
    time (&now);
 
    localtime_r (&now, &nowDate);
    localtime_r (fileTime, &fileDate);
 
    /* 
     * print date, note HH:MM is used when file modified in last 11 months,
     * not when modified in current year.... 
     */
 
    if (fileDate.tm_year == nowDate.tm_year ||
        ((fileDate.tm_year + 1 == nowDate.tm_year) && 
         (fileDate.tm_mon > nowDate.tm_mon)))
        {
        sprintf (buffer, "%s %02d %02d:%02d",
                 monthNames [fileDate.tm_mon],/* month */
                 fileDate.tm_mday,            /* day of month */
                 fileDate.tm_hour,            /* hour */
                 fileDate.tm_min              /* minute */
                 );
        }
    else
        {
        sprintf (buffer, "%s %02d  %04d",
                 monthNames [fileDate.tm_mon],/* month */
                 fileDate.tm_mday,            /* day of month */
                 fileDate.tm_year+1900        /* year */
                );
        }
    }
 
/*******************************************************************************
*
* unixList - list one file or directory in UNIX style
*
* RETURNS: TRUE or FALSE
*
* ERRNO: N/A
*
* NOMANUAL
*
* INTERNAL
* we dont check results of each fdprintf here, just the last one, which
* should be enough in case the connection broke.
*/
LOCAL STATUS unixList
    (
    FTPD_SESSION_DATA *pSlot,   /* file descriptor for output */
    char *      fileName,       /* file name */
    struct stat * fileStat,     /* stat() structure */
    int         flags           /* do Long listing format */
    )
    {
    char                *fileMark = "";
    char                dateBuf[13];
    char                modeBuf[11];
    char fType ;
    int fmt;
 


    fmt = fileStat->st_mode & S_IFMT;
 
    if (fileStat->st_attrib & DOS_ATTR_RDONLY)
        fileStat->st_mode &= ~(S_IWUSR|S_IWGRP|S_IWOTH);
 
    switch (fmt)
        {
        case S_IFIFO:
            fType = 'f';
            break;
        case S_IFCHR:
            fType = 'c';
            break;
        case S_IFDIR:
            fType = 'd';
            if (flags & FTPD_LIST_MARK)
                {
                fileMark = "/";
                }
            break;
        case S_IFBLK:
            fType = 'b';
            break;
        case S_IFLNK:
            fType = 'l';
            if (flags & FTPD_LIST_MARK)
                {
                fileMark = "@";
                }
            break;
        case S_IFSOCK:
            fType = 's';
            break;
        case S_IFREG:
        default:
            fType = '-';
            if (flags & FTPD_LIST_MARK)
                {
                if (fileStat->st_mode & (S_IXUSR|S_IXGRP|S_IXOTH))
                        {
                        fileMark = "*";
                        }
                }
            break;
        }
 
    /* print file mode */
 
    sprintf (modeBuf, "%c%c%c%c%c%c%c%c%c%c",
             fType,
             ((fileStat->st_mode & S_IRUSR) ? 'r' : '-'),
             ((fileStat->st_mode & S_IWUSR) ? 'w' : '-'),
             ((fileStat->st_mode & S_IXUSR) ? 'x' : '-'),
             ((fileStat->st_mode & S_IRGRP) ? 'r' : '-'),
             ((fileStat->st_mode & S_IWGRP) ? 'w' : '-'),
             ((fileStat->st_mode & S_IXGRP) ? 'x' : '-'),
             ((fileStat->st_mode & S_IROTH) ? 'r' : '-'),
             ((fileStat->st_mode & S_IWOTH) ? 'w' : '-'),
             ((fileStat->st_mode & S_IXOTH) ? 'x' : '-')
             );
 
    ftpdFormatTime (&fileStat->st_mtime, dateBuf);

    /* Fake nlink==1 for debug */
 
    if (FTPD_DEBUG_PATH)
        ftpdDebugMsg ("%s   1    %s %s      %9lu %s %s\r\n", modeBuf,
                      pSlot->usrName, ((flags & FTPD_LIST_GROUP) ? "trima" : ""), fileStat->st_size, dateBuf, fileName);
    if (fdprintf (pSlot->dataSock, "%s %2d %s %s %9lu %s %s\r\n",
                  modeBuf, fileStat->st_nlink,
                  "trima", ((flags & FTPD_LIST_GROUP) ? "trima" : ""),
                  fileStat->st_size, dateBuf, fileName) == ERROR)
        return ERROR;

    return OK;
    }
 
/*******************************************************************************
*
* ftpdListRootDevices - List root devices
*
* RETURNS: TRUE or FALSE
*
* ERRNO: N/A
*
* NOMANUAL
*/
LOCAL STATUS ftpdListRootDevices
    (
    FTPD_SESSION_DATA *pSlot, /* socket descriptor to write on */
    int                flags  /* if TRUE, do long listing */
    )
    {
    DEV_HDR *pDevHdr;
    DIR     *pDir;            /* ptr to directory descriptor */
 
    for (pDevHdr = (DEV_HDR *) DLL_FIRST (&iosDvList); pDevHdr != NULL;
         pDevHdr = (DEV_HDR *) DLL_NEXT (&pDevHdr->node)) 
        {
        if (pDevHdr->name[0] == '/' && strcmp (pDevHdr->name, "/tgtsvr")) 
            {
            pDir = opendir (pDevHdr->name);
            if (pDir != NULL) 
                {
                closedir (pDir);
                if (flags & FTPD_LIST_LONG) 
                    {
                        if (fdprintf (pSlot->dataSock, 
                                      "dr-xr-xr-x  1 %s %s 512 Jan 01  1970 %s\r\n",
                                      "trima", ((flags & FTPD_LIST_GROUP) ? "trima" : ""), &pDevHdr->name[1]) == ERROR)
                            return ERROR;
                    } 
                else 
                    {
                    if (fdprintf (pSlot->dataSock, "%s\r\n", &pDevHdr->name[1]) == ERROR) 
                        return ERROR;
                    }
                }
            }
        }
    return OK;
    }

/*******************************************************************************
*
* ftpdDirListGet - list files in a directory for FTP client
*
* This function performs the client's request to list out all
* the files in a given directory.  The VxWorks implementation of
* stat() does not work on RT-11 filesystem drivers, it is simply not supported.
*
* This command is similar to UNIX ls.  It lists the contents of a directory
* in one of two formats.  If <doLong> is FALSE, only the names of the files
* (or subdirectories) in the specified directory are displayed.  If <doLong>
* is TRUE, then the file name, size, date, and time are displayed.  If
* doing a long listing, any entries that describe subdirectories will also
* be flagged with a "DIR" comment.
*
* The <dirName> parameter specifies the directory to be listed.  If
* <dirName> is omitted or NULL, the current working directory will be
* listed.
*
* Empty directory entries and MS-DOS volume label entries are not
* reported.
*
* INTERNAL
* Borrowed from ls() in usrLib.c.
*
* RETURNS:  OK or ERROR.
*
* SEE ALSO: ls(), stat()
*/
 
LOCAL STATUS ftpdDirListGet
    (
    FTPD_SESSION_DATA *pSlot,   /* socket descriptor to write on */
    char        *dirName,       /* name of the directory to be listed */
    int         flags           /* if TRUE, do long listing */
   )
    {
    FAST STATUS         status;         /* return status */
    FAST DIR            *pDir;          /* ptr to directory descriptor */
    FAST struct dirent  *pDirEnt;       /* ptr to dirent */
    struct stat         fileStat;       /* file status info    (long listing) */
    BOOL                firstFile;      /* first file flag     (long listing) */
    char                *pPattern;      /* wildcard pattern */
    char                fileName [MAX_FILENAME_LENGTH];
    int						fileCount = 0;
 
    if (strcmp (dirName, "/") == 0)
        {
        return ftpdListRootDevices (pSlot, flags);
        }
 
    /* If no directory name specified, use "." */
 
    if (dirName == NULL)
        dirName = ".";
 
    pDir = NULL ; pPattern = NULL ;
 
    /* Open dir */
 
    pDir = opendir (dirName) ;
 
    /* last component may be a wildcard  pattern */
    if (pDir == NULL && (pPattern = rindex (dirName, '/')) != NULL)
        {
        *pPattern++ = EOS ;
        pDir = opendir (dirName) ;
        }
 
    if( pDir == NULL )
        {
        fdprintf (pSlot->dataSock, "Can't open \"%s\".\r\n", dirName);
        return (ERROR);
        }
 
    /* List files */
    if (FTPD_DEBUG_PATH)
        ftpdDebugMsg ("(%d) listing dir %s pattern %s\n", __LINE__, dirName,pPattern);
 
    status = OK;
    firstFile = TRUE;
 
    do  
        {
        /* reduce processor load during long listings */
        if ( fileCount++ > 20 )
		  {
			fileCount = 0;
			taskDelay(1);
		  }
 
        errno = OK;
        pDirEnt = readdir_stat(pDir, &fileStat);
 
        if (pDirEnt != NULL) 
            {
            if (FTPD_DEBUG_PATH)
                ftpdDebugMsg ("(%d) dirEnt <%s>\n", __LINE__, pDirEnt->d_name);
 
            if (pPattern != NULL && ftpDirListPattern (pPattern, pDirEnt->d_name) == FALSE)
                continue;


            if (flags & FTPD_LIST_LONG) 
                {                               /* if doing long listing */
                if ( unixList(pSlot, pDirEnt->d_name, &fileStat, flags) == ERROR)
                  return (ERROR | closedir (pDir));
                }
            else if (fdprintf (pSlot->dataSock, "%s\r\n", pDirEnt->d_name) == ERROR) 
                {
                return (ERROR | closedir (pDir));
                }
            }
        else                                    /* readdir returned NULL */
            {
            if (errno != OK)                    /* if real error, not dir end */
                {
                if (fdprintf (pSlot->dataSock, "error reading entry: %x\r\n", errno) == ERROR)
                    return (ERROR | closedir (pDir));
                status = ERROR;
                }
            }
 
        } while (pDirEnt != NULL);              /* until end of dir */
 
 
 
    /* Close dir */
 
    status |= closedir (pDir);
 
    return (status);
 
    }
 
/*******************************************************************************
*
* unImplementedType - send FTP invalid representation type error reply
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/
 
LOCAL void unImplementedType
    (
    FTPD_SESSION_DATA   *pSlot
    )
    {
    ftpdCmdSend (pSlot, pSlot->cmdSock, 550, messages [MSG_TYPE_ERROR],
                 FTPD_REPRESENTATION(pSlot));
    ftpdSockFree (&pSlot->dataSock);
    }
 
/*******************************************************************************
*
* dataError - send FTP data connection error reply
*
* Send the final error message about connection error and delete the session.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/
 
LOCAL void dataError
    (
    FTPD_SESSION_DATA   *pSlot
    )
    {
    ftpdCmdSend (pSlot, pSlot->cmdSock, 426, messages [MSG_DATA_CONN_ERROR]);
    ftpdSockFree (&pSlot->dataSock);
    }
 
/*******************************************************************************
*
* fileError - send FTP file I/O error reply
*
* Send the final error message about file error and delete the session.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/
 
LOCAL void fileError
    (
    FTPD_SESSION_DATA  *pSlot
    )
    {
    ftpdCmdSend (pSlot, pSlot->cmdSock, 551, messages [MSG_INPUT_FILE_ERROR]);
    ftpdSockFree (&pSlot->dataSock);
    }
 
/*******************************************************************************
*
* transferOkay - send FTP file transfer completion reply
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/
 
LOCAL STATUS transferOkay
    (
    FTPD_SESSION_DATA   *pSlot
    )
    {
    STATUS retVal;
 
    retVal = ftpdCmdSend (pSlot, pSlot->cmdSock, 226, messages [MSG_TRANS_COMPLETE]);
    ftpdSockFree (&pSlot->dataSock);
    return retVal;
    }
/*******************************************************************************
*
* ftpdCmdSend - send a FTP command reply
*
* In response to a request, we send a reply containing completion
* status, error status, and other information over a command connection.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/
 
LOCAL STATUS ftpdCmdSend
    (
    FTPD_SESSION_DATA  *pSlot,         /* pointer to the session slot */
    int                 controlSock,    /* control socket for reply */
    int                 code,           /* FTP status code */
    const char         *format,        /* printf style format string */
    ...
    )
    {
    va_list vaList;        /* traverses argument list */
    int i;
    int         buflen;
    FAST char   *pBufStart;
    FAST char   *pBuf;

    BOOL        lineContinue = (code & FTPD_MULTI_LINE) == FTPD_MULTI_LINE;
 
    /*
     * If this routine is called before a session is established, the
     * pointer to session-specific data is NULL. Otherwise, exit with
     * an error if an earlier attempt to send a control message failed.
     */
 
    if ((pSlot != NULL) && (pSlot->cmdSockError == ERROR))
        return (ERROR);

	 /*
	  * If no session has been established, we must allocated our own buffer
	  */
	 pBufStart = (!pSlot) ? (char *)malloc(BUFSIZE) : pSlot->cmdbuf;
	 pBuf = pBufStart;

    code &= ~FTPD_MULTI_LINE;   /* strip multi-line bit from reply code */
 
    /* embed the code first */
 
    i = sprintf (pBuf, "%d%c", code, lineContinue ? '-' : ' ');
    pBuf = &pBuf[i];
 
    /* Now the variable arguments */
 
    va_start (vaList, format);
    i = vsprintf (pBuf, format, vaList );
    pBuf = &pBuf[i];
    va_end (vaList);
 
    /* telnet style command terminator */
 
    if (FTPD_DEBUG_RESP)
        ftpdDebugMsg ("ftpdCmdSend: <%s%>\n", pBufStart);
 
    /* 
     * Add new-line, and null terminate.  We do it this way simply because
     * we are avoiding extra calls to strlen() 
     */
 
    *pBuf++ = '\r';
    *pBuf++ = '\n';
    *pBuf = '\0';
 
    /* calculate strlen */
 
    buflen = pBuf - pBufStart;
 
    /* send the command */
 
    errno = 0;
    pBuf = pBufStart;
    for(;;)
        {
        i = write (controlSock, pBuf, buflen);
        if (i == buflen)
            {
            break;
            }
        else if ( i >= 0 && i != buflen)
            {
            if (errno==EWOULDBLOCK)
                {
                buflen -= i;
                pBuf = &pBuf[i];
                taskDelay(1);
                continue;
                }
            }
        if (pSlot != NULL)
            pSlot->cmdSockError = ERROR;

        if (FTPD_DEBUG_BASIC)
            ftpdDebugMsg ("(%d) sent %s Failed on write\n",__LINE__, pBufStart);

		   
		  if ( !pSlot )	free(pBufStart);
        return (ERROR);   /* Write Error */
        }
 
    if (FTPD_DEBUG_BASIC)
        ftpdDebugMsg ("(%d) sent %s\n", __LINE__,pBufStart);

	 if ( !pSlot )	free(pBufStart);
    return (OK);                        /* Command Sent Successfully */
    }
 
/*******************************************************************************
*
* ftpdDebugMsg - print out FTP command request and replies for debugging.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/
 
LOCAL void ftpdDebugMsg
    (
    char *format,
    ...
    )
    {
    va_list vaList;        /* traverses argument list */
    int args[6];
    int i;
 
    if (ftpdDebug != FALSE) 
        {
        va_start (vaList, format);
 
        for (i = 0 ; i < (sizeof (args)/sizeof (args[0])) ; i++)
            args[i] = va_arg (vaList, int);
 
        va_end (vaList);
 
        logMsg (format, args[0], args[1], args[2], args[3], args[4], args[5]);
 
        taskDelay (10);
 
        }
    }

