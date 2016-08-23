// SplitPath.cpp: implementation of the CSplitPath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ap2 predictionater.h"
#include "SplitPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSplitPath::CSplitPath( LPCTSTR lpszPath )
{
    // Initialize our objects
    memset( path_buffer, 0, sizeof( path_buffer ) );
    memset( drive, 0, sizeof( drive ) );
    memset( dir, 0, sizeof( dir ) );
    memset( fname, 0, sizeof( fname ) );
    memset( ext, 0, sizeof( ext ) );

    // If we were given a path, split it
    if ( lpszPath ) {
        Split( lpszPath );
    }
}

BOOL CSplitPath::Split( LPCTSTR lpszPath )
{
    // If we weren't given a path, fail
    if ( lpszPath == NULL ) {
        // Return failure
        return FALSE;
    }

    // Copy the path
    _tcsncpy( path_buffer, lpszPath, sizeof( path_buffer ) - 1 );
    // Split the given path
    _tsplitpath( path_buffer, drive, dir, fname, ext );

    return TRUE;
}
