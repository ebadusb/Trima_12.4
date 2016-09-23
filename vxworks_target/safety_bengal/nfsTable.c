/* includes */ 
#include <vxWorks.h> 
#include <unistd.h> 
#include <xdr_nfs.h> 
#include <string.h> 
#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <memLib.h> 
#include <nfsdLib.h> 
#include <ctype.h> 
#include <sys/stat.h> 
#include <semLib.h>
#include <private/nfsHashP.h>

/* defines */ 
#define MAX_DEVS 6 
#define MAX_MNTS 6 
#define MAX_FILES 200

typedef struct tblDesc     
{ 
  int      refCnt;        /* reference count */ 
  SEM_ID  sem;            /* protection lock */ 
  char      nmBuf[MAX_FILES][NFS_MAXPATHLEN]; /* file names */ 
}TBL_DESC; 

/* Mapping from a file system device id to corresponding hash table descriptor. 
* There is one hash table per file system device if there are nfs exported 
*  directories in that file system 
*/ 

typedef struct devIdPair 
{ 
  int          dev; 
  TBL_DESC *  pTbl; 
} DEV_ID_PAIR ; 


/* Mapping from the nfs volume mount id to the hash table descriptor. 
* The nfs volume id is generated in the nfsExport () routine and is 
* maintained within the nfs file handle. 
*/ 
typedef struct mntIdPair 
{ 
  int          mntId; 
  TBL_DESC *  pTbl; 
} MNT_ID_PAIR; 


/* locals */ 

/* device to hash table descriptor table */ 
LOCAL DEV_ID_PAIR   devTbl[MAX_DEVS]; 
/* mount volume     to hash table descriptor table */ 
LOCAL MNT_ID_PAIR   mntTbl[MAX_MNTS]; 



/*  insertMntId - insert entry into mount id based table  
* 
* Inserts pointer <pTbl> to a hash file table descriptor in the mount id 
* based table for nfs mount point <mntId> 
* 
* NOMANUAL 
* 
* RETURNS: N/A. 
*/ 

LOCAL void insertMntId( TBL_DESC *pTbl, int mntId ) 
{ 
  MNT_ID_PAIR *pMnt; 

  for (pMnt = mntTbl; pMnt < mntTbl + MAX_MNTS; ++pMnt) 
  { 
    if (pMnt->mntId == 0) 
    { 
      pMnt->mntId = mntId; 
      pMnt->pTbl = pTbl; 
      return; 
    } 
  } 
} 

/* getTblEnt - get entry from mnt table  
* 
* Returns ptr to table descriptor for give mount id <id>. 
* 
* RETURNS: ptr to entry or NULL 
*/ 

LOCAL TBL_DESC *tblDescGet( int id )   /* mount id */ 
{ 
  int ix; 

  for (ix = 0; ix < MAX_MNTS; ++ix) 
  { 
    if (mntTbl[ix].mntId ==  id) 
      return (mntTbl[ix].pTbl); 
  } 

  return (NULL); 
} 


/* nfsHashTblSet - set up nfs hash table 
* 
* Set up an nfs hash table for exported directory <pDir> with mount voleume id 
* <mntId> and file system device id <devId>. If a hash fiel already exists 
* for <devId> then we simply need to setup a ptr to the existing hash table 
* descriptor alse a new hash file must be allocated adn initialized. 
* 
* RETURNS: N/A 
*/ 
void nfsHashTblSet( char *pDir, int  mntId, u_long devId ) 
{ 
  DEV_ID_PAIR *pDev; 

  for (pDev = devTbl; pDev < devTbl + MAX_DEVS; ++pDev) 
  { 
    if (pDev->dev != 0 && pDev->dev == devId) 
      break; 
  } 
  if (pDev >= devTbl + MAX_DEVS) 
  { 
    for (pDev = devTbl; pDev < devTbl + MAX_DEVS; ++pDev) 
    { 
      if (pDev->dev == 0) 
      { 
    pDev->dev = devId; 
    pDev->pTbl = (TBL_DESC *)malloc( sizeof( TBL_DESC ) ); 
    memset( pDev->pTbl, 0, sizeof( TBL_DESC ) ); 
    pDev->pTbl->refCnt = 0; 
    pDev->pTbl->sem = semMCreate (SEM_Q_PRIORITY | SEM_INVERSION_SAFE); 
    break; 
      } 
    } 
  } 
  pDev->pTbl->refCnt++; 
  insertMntId (pDev->pTbl, mntId); 
} 


/* nfsHashTblUnset - clean up  nfs hash table 
* 
* Clean up an nfs hash table for exported directory <pDir> with mount volume id 
* <mntId> and file system device id <devId>. Delete the allocated table  
* descriptor if there are no other references to it.  
* 
* RETURNS: N/A 
*/ 
void nfsHashTblUnset( char *pDir, int mntId ) 
{ 
  DEV_ID_PAIR *      pDev; 
  MNT_ID_PAIR *      pMnt; 
  TBL_DESC    *      pTbl; 

  for (pMnt = mntTbl; mntTbl < mntTbl + MAX_MNTS; ++pMnt) 
  { 
    if (pMnt->mntId  == mntId) 
      break; 
  } 
  if (pMnt >= mntTbl + MAX_MNTS) 
  { 
    return; 
  } 
  else 
  { 
    pTbl = pMnt->pTbl; 
    pTbl->refCnt--; 
    if (pTbl->refCnt == 0) 
    { 
      semDelete (pTbl->sem); 
      for (pDev = devTbl; pDev < devTbl + MAX_DEVS; ++pDev) 
      { 
    if (pDev->pTbl == pTbl) 
    { 
      pDev->pTbl = 0; 
      pDev->dev = 0; 
    } 
      } 
      free( pTbl ); 
    } 
    pMnt->mntId = 0; 
    pMnt->pTbl = 0; 
  } 
} 

/* nfsFhLkup - find name corresponding to file handle  
* 
* using file handle <fh> as the key get corresponding file name 
* in <pName>. 
* 
* RETURNS: OK or ERROR if name not found 
*/ 
STATUS nfsFhLkup( NFS_FILE_HANDLE *pFh, char *pName ) 
{ 
  TBL_DESC *pT; 

  pT = tblDescGet (pFh->volumeId); 
  if (pT == NULL) 
  { 
    return (ERROR); 
  } 

  if ( pFh->inode >= 0 && pFh->inode < MAX_FILES ) 
  { 
    if ( pT->nmBuf[pFh->inode][0] ) 
    { 
      strcpy( pName, pT->nmBuf[pFh->inode] ); 
      return OK; 
    } 
  } 
  return (ERROR); 
} 


/* nfsNmLkupIns - find fh corresponding to file name 
* 
* Find the file handle     corresponding to the given file name  <Pname> with 
* nfs mount volume id <mntId> and create and insert an entry in the table 
* if not found. 
* 
* RETURNS: file handle or ERROR; 
* NOMANUAL 
*/ 


int nfsNmLkupIns( int mntId, char *pName ) 
{ 
  TBL_DESC *    pT; 
  int        i; 

  pT = tblDescGet (mntId); 
  if (pT == NULL) 
  { 
    return (ERROR); 
  } 

  for ( i = 0 ; i < MAX_FILES ; i++ ) 
  { 
    if ( pT->nmBuf[i][0] == 0 ) 
    { 
      semTake( pT->sem, WAIT_FOREVER ); 
      if ( pT->nmBuf[i][0] == 0 ) /* double check before inserting */ 
      { 
    strcpy( pT->nmBuf[i], pName ); 
    semGive( pT->sem ); 
    return i; 
      } 
      /* Some other task beat us to this entry, so give the semaphore 
       * and run the loop again */ 
      semGive( pT->sem ); 
    } 
    /* Do not use else if here, other task may have just entered 
     * this file into the table..... 
     */ 
    if ( !strcmp( pName, pT->nmBuf[i] ) ) 
    { 
      return i; 
    } 
  } 
  return (ERROR); 
} 

