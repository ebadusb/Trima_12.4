/*
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxworks.h>

#include "guiglobs.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"

//
//
//  This file contains the initialization needed by normal mode.  It is split out
//  from service/guiinit.cpp to avoid linking in the world for service.
//
void guiinit ()
{
   // Initialize the CFR11 logging module
   guiglobs::bioMgr().Initialize();

   // Initialize the Cassette Manager
   guiglobs::cassette_mgr = new CassetteMgr;
   guiglobs::cassette_mgr->Initialize();

   // Initialize the Aas Info Class
   guiglobs::aasRunInfo.init();

   // Call the initialization routine of the donor data object.
   // This is declared in GUIGLOBS for addressability by
   // donvital, doninfo.cpp and donordl.cpp.
   // This is started before the startup of the Commutator because
   // its messages should be registered with the dispatcher BEFORE the Powerup
   // screen is realized in the Commutator startup -- the Powerup screen sends the message
   // to PROC that GUI is up and enabled.
   guiglobs::gui_donor_data_repository.Initialize();
   guiglobs::gui_donor_data_repository.start_donor_data_repository ();

   // Call the initialization routine for the barcode repository object. This will
   // open the barcode category disk files and init. the barcode scan FIFO.
   guiglobs::barcode_repo = new Barcode_Repository ;
   if (guiglobs::barcode_repo -> initialize_repository (INIT_MODE_NORMAL) != SUCCESS_REPO)
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: failed to properly initialize in GUISTART." << endmsg;

   DataLog(log_level_gui_startup_debug) << "Initializing status bar and line." << endmsg;
   guiglobs::initStatBar();

   guiglobs::apheresis_status_line = new Status_Line;
   guiglobs::apheresis_status_line->allocate_resources();

   guiglobs::proc_status.initialize();

   DataLog(log_level_gui_startup_debug) << "GuiInit is complete." << endmsg;
}

/* FORMAT HASH a3d2b280d2b8d8c2bbae76ef961f51a5 */
