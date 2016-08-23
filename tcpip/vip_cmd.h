/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      vip.h
 *             External interface process
 *
 * AUTHOR:     Bruce Dietrich
 *
 * ABSTRACT:   This file acts on the external service requests.  It is
 *            the bridge between the external computer and trima messaging.
 *
 ******************************************************************************/

#ifndef INCLUDE_VIP_CMD
#define INCLUDE_VIP_CMD

#include "sockinet.h"

#include "vipif.h"
#include "xif_cds.h"

#include "alarms.h"     // For ALARM return values.

class VipCmdProcess
{
public:
   VipCmdProcess();
   virtual ~VipCmdProcess();

   //
   // Initialize the class.
   //
   ALARM_VALUES Init (void);


   //
   // Block on the receive socket and process the received messages.
   //
   void Process (void);


   //
   // Cleanup does a gracefull shutdown of the class.
   // Attached in the main program to handle SIGINT, SIGPWR
   //
   void Cleanup (int sig = 0);


private:
   VipCmdProcess(const VipCmdProcess& otherVipCmdProcess);

   VipCmdProcess& operator = (const VipCmdProcess& otherVipCmdProcess);

   //
   // Routine initializes, and waits (blocks) for a socket connection.
   //
   void acceptConnection (void);


   //
   // Routine close, Vista socket connection.
   //
   void closeVistaCmdSocket (bool updatGUI);
   //
   // This routine handles the validation, execution and response
   // of all the Commands sent by Vista.
   //
   int ProcessMessage (char* msg, int sizeInBytes);


   //
   // This routine writes the donor bitmap to a file.
   //
   bool writeDonorPicture (const unsigned char* bitmap, const unsigned long& crc);


   //
   // This routine processes the Set Trima Time command.
   //
   int SetTrimaTime (const trimaVistaHeader& hdr, int sizeInBytes);

   //
   // This routine processes the Start Periodic Status command.
   //
   int StartPeriodicStatus (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Start Machine Status command.
   //
   int StartMachineStatus (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Set Offline Configuration command.
   //
   int SetOfflineConfig (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Download Donor Info command.
   //
   int DownloadDonorInfo (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Request File List command.
   //
   int RequestFileList (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Request File List By Age command.
   //
   int RequestFileListBySequenceNumber (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Upload File command.
   //
   int UploadFile (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Ping command.
   //
   int Ping (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Barcode Response command.
   //
   int BarcodeResponse (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Get Interface State command
   //
   int GetInterfaceState (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Download Biometric Signatures file command
   //
   int DownloadBiometricSignatures (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Biometric Signature Response command
   //
   int BiometricSignatureResponse (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Download Catalog List command
   //
   int DownloadCatalogList (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Get Optional Settins Values command
   //
   int GetOptionalSettingsValues (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Get Regionalziation Feature command
   //
   int GetRegionalizationFeature (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Get Regionalziation Constant command
   //
   int GetRegionalizationConstant (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Get Regionalziation Confirmation Code command
   //
   int GetRegionalizationConfirmationCode (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine processes the Set Regionalziation Feature command
   //
   int SetRegionalizationFeature (const trimaVistaHeader& hdr, int sizeInBytes);


   //
   // This routine sends a PIN response msg to GUI.
   //
   int SendPinResponse (int sizeInBytes, const biometricSignatureResponseMsg& msg);



   void Send      (void* msg, int size);
   void SendAck   (const trimaVistaHeader& hdr, int status, const char* msg = 0);
   void SendReply (void* hdr, int size);

// Data Members
private:
   // CDS instances
   External_IF_CDS _vipshm;

   // Vista Message Interface
   VipIf _vipif;

   // Receive socket for Vista I/F
   // The RxListen socket is a TCP blocking server socket.  It listens connections from "clients".
   // The Vista socket is a TCP blocking server socket.  It is the actual connection to a "client".
   sockinetaddr   _addr;
   int            _listenSocket;
   int            _vistaSocket;

   struct timeval _timeoutValue;   // Socket timeout value

   // member var used to send back file requests.
   char* _replyBuffer;

   // Flag to shutdown program.
   bool _shutdown;

};

#endif

/* FORMAT HASH 926f7133f0f5c97b30f291f2442a1603 */
