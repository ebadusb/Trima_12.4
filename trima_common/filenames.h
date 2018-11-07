/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef _FILENAMES_INCLUDE
#define _FILENAMES_INCLUDE

/*
 * vxWorks include is needed to insure that processor type is set correctly
 */
#include <vxWorks.h>

#define VXBOOT_PARTITION_NAME    "/vxboot"
#define TRIMA_PARTITION_NAME     "/trima"
#define CONFIG_PARTITION_NAME    "/config"
#define MACHINE_PARTITION_NAME   "/machine"

/*
 * On the target platform, all partitions are mounted directly on "/".  On
 * the simulator, we use the target file server "/tgtsvr".
 */
#if (CPU==SIMNT)
# define ROOT "host:C:/sim/trima/main"
#else
# define ROOT
#endif

/*
 * Define paths for each partition
 */
#define VXBOOT_PATH     ROOT VXBOOT_PARTITION_NAME
#define TRIMA_PATH      ROOT TRIMA_PARTITION_NAME
#define CONFIG_PATH     ROOT CONFIG_PARTITION_NAME
#define MACHINE_PATH    ROOT MACHINE_PARTITION_NAME

/*
 * Define others paths (for sub-directories from the root partitions)
 */
#define SOFTCRC_PATH       TRIMA_PATH   "/softcrc"
#define BOOT_LOG_PATH      MACHINE_PATH "/bootlog"
#define SYSINIT_PATH       TRIMA_PATH   "/init"
#define CONFIG_CRC_PATH    CONFIG_PATH  "/crc"
#define GRAPHICS_PATH      CONFIG_PATH  "/graphics"
#define LOG_PATH           MACHINE_PATH "/log"
#define SAFETY_BOOT_PATH   TRIMA_PATH   "/safety/boot"
#define SAVEDATA_PATH      MACHINE_PATH "/savedata"
#define TEMP_PATH          MACHINE_PATH "/tmp"
#define UPDATE_PATH        MACHINE_PATH "/update"
#define TOOLS_PATH         MACHINE_PATH "/tools"
#define TRIMA_DRIVER_PATH  TRIMA_PATH   "/low_level"
#define TEMPLATES_PATH     TRIMA_PATH   "/config/templates"
#define ARCHIVE_PATH       MACHINE_PATH "/v5_archive"
#define FILELISTS_PATH     SOFTCRC_PATH "/filelists"
#define SNAPSHOTS_PATH     MACHINE_PATH "/snap"
#define CUSTOMSPOOF_PATH   TOOLS_PATH   "/spoof"
#define INSTALL_LOG_PATH   MACHINE_PATH "/install"

/*
 * Default Trima HW drivers
 */
#define TRIMA_CONTROL_HW_DRIVER TRIMA_DRIVER_PATH "/ctl_hw.out"
#define TRIMA_SAFETY_HW_DRIVER  TRIMA_DRIVER_PATH "/safe_hw.out"

/*
 * Installation script
 */
#define INSTALL_ENTRY_POINT     updateTrima
#define TRIMA_INSTALL_SCRIPT    UPDATE_PATH "/updateTrima"


/*
 * Special Function files
 * These will not be deleted at install if the development_only flag file is present
 * Remember to add new filenames to the installSpecialFiles array (right below)
 */
#define CLINICAL_BUILD         CONFIG_PATH "/clinical_build"
#define FTP_ALLOWED            CONFIG_PATH "/ftp_with_cassette_down"
#define PUMP_STROKE_LOGGING_ON CONFIG_PATH "/pump_stroke_logging"
#define SOUND_OFF              CONFIG_PATH "/no_sound"
#define TELNET_ON              CONFIG_PATH "/telnet"
#define TEST_BUILD             CONFIG_PATH "/test_build"
#define DEVELOPMENT_ONLY       UPDATE_PATH "/development_only"
#define DLOG_OVERRIDE          TOOLS_PATH "/dlog_level_override.txt"
/*
 * Add special files to this array so Install scripts work properly
 */
#ifdef INSTALL_BUILD
#define INSTALL_SPECIAL_FILES
const static char* installSpecialFiles[] = {CLINICAL_BUILD, FTP_ALLOWED, PUMP_STROKE_LOGGING_ON, SOUND_OFF,
                                            TELNET_ON, TEST_BUILD};
const static int   numInstallSpecialFiles = (int)(sizeof(installSpecialFiles) / sizeof(installSpecialFiles[0]));
#endif

#define FILE_INSTALL_LOG "install_log.txt"

/*
 * Specific config file names
 */
#define FILE_CONFIG_DAT     "config.dat"
#define FILE_CONFIGDAT_BK   "config.dat.bk"
#define FILE_CAL_DAT        "cal.dat"
#define FILE_TCHSCRN_DAT    "touch_screen.dat"
#define FILE_HW_DAT         "hw.dat"
#define FILE_SW_DAT         "sw.dat"
#define FILE_RBC_DAT        "rbc.dat"
#define FILE_METER_DAT      "meter.dat"
#define FILE_SOUNDS_DAT     "sounds.dat"
#define FILE_CASSETTE_DAT   "cassette.dat"
#define FILE_SETCONFIG_DAT  "setconfig.dat"
#define FILE_FEATURES       "features.bin"
#define FILE_FEATURES_TMP   "features.tmp"
#define FILE_MACHINE_ID_TMP "machine.tmp"
#define FILE_MACHINE_ID     "machine.id"

#define FILE_GUI_GRAPHICS     "graphics_package.out"
#define FILE_SERVICE_GRAPHICS "service_" FILE_GUI_GRAPHICS

/*
 * Complete path/filenames for config files
 */
#define PNAME_CONFIGDAT         CONFIG_PATH "/" FILE_CONFIG_DAT
#define PNAME_CALDAT            CONFIG_PATH "/" FILE_CAL_DAT
#define PNAME_TCHSCRNDAT        CONFIG_PATH "/" FILE_TCHSCRN_DAT
#define PNAME_HWDAT             CONFIG_PATH "/" FILE_HW_DAT
#define PNAME_SWDAT             CONFIG_PATH "/" FILE_SW_DAT
#define PNAME_RBCDAT            CONFIG_PATH "/" FILE_RBC_DAT
#define PNAME_METERDAT          CONFIG_PATH "/" FILE_METER_DAT
#define PNAME_CASSETTEDAT       CONFIG_PATH "/" FILE_CASSETTE_DAT
#define PNAME_SOUNDSDAT         CONFIG_PATH "/" FILE_SOUNDS_DAT
#define PNAME_SETCONFIGDAT      CONFIG_PATH "/" FILE_SETCONFIG_DAT
#define PNAME_BARCODESYMDAT     CONFIG_PATH "/" FILE_BARCODE_SYM_DAT

#define PNAME_CASSETTEDAT_BK    PNAME_CASSETTEDAT ".bk"
#define PNAME_SETCONFIGDAT_BK   PNAME_SETCONFIGDAT ".bk"

#define PNAME_GUI_GRAPHICS      GRAPHICS_PATH "/"  FILE_GUI_GRAPHICS
#define PNAME_SERVICE_GRAPHICS  GRAPHICS_PATH "/"  FILE_SERVICE_GRAPHICS
#define PNAME_FEATURES          CONFIG_PATH "/" FILE_FEATURES
#define PNAME_FEATURES_TMP      TEMP_PATH "/" FILE_FEATURES_TMP
#define PNAME_MACHINE_ID        CONFIG_PATH "/" FILE_MACHINE_ID
#define PNAME_MACHINE_ID_TMP    TEMP_PATH "/" FILE_MACHINE_ID_TMP

#define PNAME_INSTALL_LOG       INSTALL_LOG_PATH "/" FILE_INSTALL_LOG

/*
 * Complete path/filenames for config crc files
 */
#define PNAME_CONFIGDAT_CRC         CONFIG_CRC_PATH "/" FILE_CONFIGDAT_CRC
#define PNAME_CALDAT_CRC            CONFIG_CRC_PATH "/" FILE_CALDAT_CRC
#define PNAME_HWDAT_CRC             CONFIG_CRC_PATH "/" FILE_HWDAT_CRC
#define PNAME_RBCDAT_CRC            CONFIG_CRC_PATH "/" FILE_RBCDAT_CRC
#define PNAME_CASSETTEDAT_CRC       CONFIG_CRC_PATH "/" FILE_CASSETTE_DAT_CRC
#define PNAME_SETCONFIG_CRC         CONFIG_CRC_PATH "/" FILE_SETCONFIG_CRC
#define PNAME_CASSETTEDAT_CRC_BK    PNAME_CASSETTEDAT_CRC ".bk"
#define PNAME_SETCONFIG_CRC_BK      PNAME_SETCONFIG_CRC ".bk"

/*
 * Complete path/filenames for config bk files
 */
#define PNAME_CONFIGDAT_BK      CONFIG_PATH "/" FILE_CONFIGDAT_BK
#define PNAME_CONFIGDAT_CRC_BK  CONFIG_CRC_PATH "/" FILE_CONFIGDAT_CRC_BK

/*
 * Cobe Configuration data is unique.  It contains a set of constants
 * that never change after software release.  It is located with the
 * software and is included in the software crc.  Its full path and
 * name is provided here.
 */
#define PNAME_COBECONFIGDAT  TRIMA_PATH "/config/cobe_config.dat"

/*
 * Power fail save data
 */
#define SAFETY_PFSAVE_FILE   SAVEDATA_PATH "/safety_pfr.dat"
#define CATEGORY_FILE_NAME   SAVEDATA_PATH "/gui_barcode_pfr.rfs"
#define CFR11_FILE_NAME      SAVEDATA_PATH "/cfr_11_pfr.rfs"

/*
 * Hardware CRC files, generated by sysidlog.cpp to allow proc state
 * CPUIDTest to determine if the CPU type and speed is acceptable for
 * running the Trima software.
 */
#define PNAME_CONTROL_HW_CRC  TEMP_PATH "/control_hw_crc"
#define PNAME_SAFETY_HW_CRC   TEMP_PATH "/safety_hw_crc"

/*
 * Globvars configuration files (variables in these files are written to the
 * environment at system startup and are available through the getenv()
 * function.
 */
#define GLOBVARS_FILE         CONFIG_PATH "/globvars"
#define GLOBVARS_DEFAULT_FILE TRIMA_PATH  "/config/globvars.default"

/*
 * TRAP Config Files
 */
#define TRAP_DEFAULTS_FILE   TRIMA_PATH  "/config/trap_default.dat"
#define TRAP_OVERRIDE_FILE   CONFIG_PATH "/trap_override.dat"
#define TRAP_TEMPORARY_FILE  TEMP_PATH   "/trap_temp.dat"

/*
 * Vista Interface Files
 */
#define VIP_TEMPORARY_FILE   TEMP_PATH      "/vip_temp.dat"
#define DONOR_FACE_BMP       UPDATE_PATH    "/donrface.bmp"

/*
 * Biometric Signature files
 */
#define BIO_PIN_FILE      CONFIG_PATH       "/pin.bio"
#define BIO_PIN_CRC_FILE  CONFIG_CRC_PATH   "/pin.crc"


/*
 * Procedure states file
 */
#define PROC_FILES                 ROOT         "/trima/proc"
#define PROC_STATES_FILE           PROC_FILES   "/states"
#define CENT_TESTS_STATES_FILE     PROC_FILES   "/centrifugetests"
#define CENT_SPIN_TEST_STATES_FILE PROC_FILES   "/centrifugeSpinTest"

/*
 * This string must be concatenated in front of the table entries found in the
 * bitmap_file_table below. It represents the root level directory in which all
 * bitmaps and their organizing directories are found.
 *
 */
#define GUI_DIRECTORY           TRIMA_PATH          "/gui"
#define BITMAP_DIRECTORY        GUI_DIRECTORY       "/bitmaps"
#define FONT_DIRECTORY          GUI_DIRECTORY       "/fonts"    /* This directory is for pre-loaded fonts		*/
#define DROP_IN_FONTS_DIR       CONFIG_PATH         "/fonts"    /* Whereas this directory is for drop-in fonts	*/
#define STRING_DIRECTORY        CONFIG_PATH         "/strings"
#define DATA_DIRECTORY          CONFIG_PATH         "/data"

/*
 * GUI libraries for various hardware choices
 */
#define UGL_MODULE_PATH         GUI_DIRECTORY
#define CT65545_UGL_MODULE      UGL_MODULE_PATH "/UGL_ct65545.o"
#define SIMNT_UGL_MODULE        UGL_MODULE_PATH "/UGL_simnt.o"

#define DEFAULT_FONT_FILE  "lsansd.ttf"
#define DEFAULT_FONT_PATH FONT_DIRECTORY  "/" DEFAULT_FONT_FILE

#define DEFAULT_DATA_FILE  "data_item.info"
#define DEFAULT_DATA_PATH DATA_DIRECTORY  "/" DEFAULT_DATA_FILE

/*
 * APS autocal related files
 */
#define AUTOCAL_FILE            CONFIG_PATH     "/autocal.dat"
#define AUTOCAL_FILE_CRC        CONFIG_CRC_PATH "/autocal.crc"

/*
 * System initialize modules called directly from the vxWorks image
 */
#define CONTROL_INIT_MODULE   SYSINIT_PATH "/sysinit_control.out"
#define SAFETY_INIT_MODULE    SYSINIT_PATH "/sysinit_safety.out"

/*
 * Boot files required by safety
 */
#define SAFETY_BOOTROM_IMAGE        SAFETY_BOOT_PATH "/bootrom.sys"
#define SAFETY_BOOTROM_PXE_IMAGE    SAFETY_BOOT_PATH "/bootrom_pxe.sys"

#define SAFETY_VXWORKS_IMAGE        SAFETY_BOOT_PATH "/vxWorks"
#define SAFETY_VXWORKS_PXE_IMAGE    SAFETY_BOOT_PATH "/vxWorks_pxe"

/*
 * Safety checkself CRC files
 * created by saf_exec and safe_drv
 * removed by updateTrima
 */
#define SAFETY_EXEC_CRC          SAVEDATA_PATH  "/saf_exec.crc"
#define SAFETY_DRIVER_CRC        SAVEDATA_PATH  "/safe_drv.crc"
#define SAFETY_EXEC_FILE         TRIMA_PATH     "/safety/saf_exec.out"
#define SAFETY_DRIVER_FILE       TRIMA_PATH     "/low_level/safe_drv.out"

/*
 * Pipe devices used for trapping stdout/stderr from vxWorks modules
 * to put into log file
 */
#define PIPE_LOGTRAP    "/pipe/logtrap"
#define PIPE_LOGTRAP_BUFFER 256

/*
 * Bootlog files store status information about system boot
 * and are mainly useful for troubleshooting OS startup problems.
 */
#define CONTROL_BOOTLOG      BOOT_LOG_PATH "/control"
#define SAFETY_BOOTLOG       BOOT_LOG_PATH "/safety"


#define TRIMA_PROJECT_REVISION_FILE TRIMA_PATH "/projectrevision"
#define PREDICT_VERSION_FILE        TRIMA_PATH "/predict_version"


/*
 * CRC creation files
 *
 */

/* Trima CRC file */
#define TRIMA_CRC_FILE       TRIMA_PATH "/trima.crc"
#define SAFETY_CRC_FILE      TRIMA_PATH "/safety.crc"

#define CALDAT_CRC_FILE   CONFIG_CRC_PATH "/caldat.crc"
#define MACHINE_CRC_FILE  CONFIG_CRC_PATH "/machine.crc"

#define TRIMA_PROJECT_REVISION_FILE TRIMA_PATH "/projectrevision"


/*
 * Specific config crc definition files with path
 */
#define TRIMA_CRC_LIST_FILES       FILELISTS_PATH "/trima.files"
#define CALDAT_CRC_LIST_FILES      FILELISTS_PATH "/caldat.files"
#define MACHINE_CRC_LIST_FILES     FILELISTS_PATH "/machine.files"
#define SAFETY_CRC_LIST_FILES      FILELISTS_PATH "/safety.files"
#define GRAPHICS_CRC_LIST_FILES    FILELISTS_PATH "/graphics.files"
#define STRINGS_CRC_LIST_FILES     FILELISTS_PATH "/strings.files"
#define DATA_CRC_LIST_FILES        FILELISTS_PATH "/data.files"
#define FONTS_CRC_LIST_FILES       FILELISTS_PATH "/fonts.files"
#define CONFIG_CRC_LIST_FILES      FILELISTS_PATH "/config.files"
#define HWDAT_CRC_LIST_FILES       FILELISTS_PATH "/hwdat.files"
#define RBCDAT_CRC_LIST_FILES      FILELISTS_PATH "/rbcdat.files"
#define CASSETTE_CRC_LIST_FILES    FILELISTS_PATH "/cassette.files"
#define SETCONFIG_CRC_LIST_FILES   FILELISTS_PATH "/setconfig.files"
#define REGIONALIZATION_CRC_LIST_FILES  FILELISTS_PATH "/features.files"


/*
 * Specific config crc file names
 */
#define FILE_CONFIGDAT_CRC      "config.crc"
#define FILE_CONFIGDAT_CRC_BK   "config.crc.bk"
#define FILE_CAL_CRC            "cal.crc"
#define FILE_HWDAT_CRC          "hwdat.crc"
#define FILE_RBCDAT_CRC         "rbcdat.crc"
#define FILE_CASSETTE_DAT_CRC   "cassette.crc"
#define FILE_SETCONFIG_CRC      "setconfig.crc"
#define FILE_CALDAT_CRC         "caldat.crc"
#define FILE_MACHINE_CRC        "machine.crc"

/*
 * Complete path/filenames for config crc files
 */
#define PNAME_CALDAT_CRC            CONFIG_CRC_PATH "/" FILE_CALDAT_CRC
#define PNAME_CASSETTEDAT_CRC       CONFIG_CRC_PATH "/" FILE_CASSETTE_DAT_CRC
#define PNAME_CONFIGDAT_CRC         CONFIG_CRC_PATH "/" FILE_CONFIGDAT_CRC
#define PNAME_DATA_CRC              CONFIG_CRC_PATH "/data.crc"
#define PNAME_FONT_CRC              CONFIG_CRC_PATH "/fonts.crc"
#define PNAME_GUI_GRAPHICS_CRC      CONFIG_CRC_PATH "/graphics.crc"
#define PNAME_HWDAT_CRC             CONFIG_CRC_PATH "/" FILE_HWDAT_CRC
#define PNAME_MACHINE_CRC           CONFIG_CRC_PATH "/" FILE_MACHINE_CRC
#define PNAME_RBCDAT_CRC            CONFIG_CRC_PATH "/" FILE_RBCDAT_CRC
#define PNAME_SETCONFIG_CRC         CONFIG_CRC_PATH "/" FILE_SETCONFIG_CRC
#define PNAME_STRING_CRC            CONFIG_CRC_PATH "/strings.crc"
#define PNAME_REGIONALIZATION_CRC   CONFIG_CRC_PATH "/features.crc"

#define PNAME_CASSETTEDAT_CRC_BK    PNAME_CASSETTEDAT_CRC ".bk"
#define PNAME_SETCONFIG_CRC_BK      PNAME_SETCONFIG_CRC ".bk"

/*
 * Specific temp config crc files with path
 */
#define TEMP_TRIMA_CRC_FILE     TEMP_PATH  "/trima.crc"
#define TEMP_SAFETY_CRC_FILE    TEMP_PATH  "/safety.crc"
#define TEMP_CALDAT_CRC_FILE    TEMP_PATH  "/caldata.crc"
#define TEMP_CONFIGDAT_CRC_FILE TEMP_PATH "/" FILE_CONFIGDAT_CRC
#define TEMP_HWDAT_CRC_FILE     TEMP_PATH "/" FILE_HWDAT_CRC
#define TEMP_MACHINE_CRC_FILE   TEMP_PATH "/" FILE_MACHINE_CRC
#define TEMP_RBCDAT_CRC_FILE    TEMP_PATH "/" FILE_RBCDAT_CRC

/*
 * default string.info location
 */
#define DEFAULT_STRING_INFO_FILE_NAME "string.info"
#define DEFAULT_STRING_INFO_FILE STRING_DIRECTORY "/" DEFAULT_STRING_INFO_FILE_NAME
#define DEFAULT_LANGUAGE_KEY            "english"


/*
 * Spoofer file definitions
 */
#define FILE_SPOOF_SERVER_CONTROL "spoof_server_control.out"
#define PNAME_SPOOF_SERVER_CONTROL_FILE CUSTOMSPOOF_PATH "/" FILE_SPOOF_SERVER_CONTROL


/*
 * Log file extension.  Used by External interfaces (Trap, Vista) as well as the data logger
 *
 */
#define LOG_FILE_EXTENSION ".dlog"


#define OSF_FILE  CONFIG_PATH "/osfData.dat"

#endif /* ifndef _FILENAMES_INCLUDE */

/* FORMAT HASH 7e19a42d7f834149ee6c2a3aaaedd7d1 */
