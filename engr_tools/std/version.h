/* FILENAME: version.h
 * PURPOSE: provides software version number
 * CHANGELOG:
 *   $Log: version.h $
 *   01/24/2000 - dyes - added -loop command line option
 *   Revision 1.3  2000/01/13 21:23:27  TD07711
 *   01/05/2000 - dyes 
 *      1. CLEANUP command exit code no longer matters
 *      2. CLEANUP nonzero exit no longer prevent subsequent CLEANUP commands
 *         from running.
 *      3. COPY no longer performed with -p, to avoid creating permission problems.
 *   12/08/99 - dyes - initial version, 1.0
 *   12/09/99 - dyes - added -nolock cmd line option, 1.1
 */

#ifndef __version_h__ // prevents multiple inclusion
#define __version_h__

static const char* Version = "1.3";

#endif // __version_h__
