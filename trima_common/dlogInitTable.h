/*
* Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
*
* TITLE:      dlogInitTable.h
*
*****************************************************
This is the table of all the info the dlog needs to
initialize Trima logging.  It used to be a part of
trima_datalog_levels.h, but I moved it here because I
found it aggrivating that every time I wanted to enable
a debug log level, it required a massive recompile of
the whole software.  This way, most of the software
doesn't depend on this table.  --Dan x4664
*****************************************************/

#ifndef __DLOG_INIT_TABLE_H__
#define __DLOG_INIT_TABLE_H__

#define TRIMA_DATALOG_LEVELS_INIT_TABLE

#include "trima_datalog_levels.h"

struct DataLog_TrimaInitTable
{
   DataLog_Level*  level;
   DataLog_Handle* handle;
   const char*     name;
   bool            enabled;
};


static DataLog_TrimaInitTable trimaInitTable[] =
{
   { &log_level_startup_info, &log_handle_startup_info, "startup_info", true},
   { &log_level_startup_error, &log_handle_startup_error, "startup_error", true},
   { &log_level_startup_detail, &log_handle_startup_detail, "startup_detail", true},
   { &log_level_startup_external, &log_handle_startup_external, "startup_external", true},

   { &log_level_alarm_handler_info, &log_handle_alarm_handler_info, "alarm_handler_info", true},
   { &log_level_alarm_handler_error, &log_handle_alarm_handler_error, "alarm_handler_error", true},
   { &log_level_alarm_handler_external, &log_handle_alarm_handler_external, "alarm_handler_external", true},

   { &log_level_proc_info, &log_handle_proc_info, "proc_info", true},
   { &log_level_proc_error, &log_handle_proc_error, "proc_error", true},
   { &log_level_proc_debug, &log_handle_proc_debug, "proc_debug", false},
   { &log_level_proc_external, &log_handle_proc_external, "proc_external", true},

   { &log_level_datfile_info, &log_handle_datfile_info, "datfile_info", true},
   { &log_level_datfile_error, &log_handle_datfile_error, "datfile_error", true},
   { &log_level_datfile_debug, &log_handle_datfile_debug, "datfile_debug", false},
   { &log_level_datfile_external, &log_handle_datfile_external, "datfile_external", true},

   { &log_level_proc_alarm_monitor_info, &log_handle_proc_alarm_monitor_info, "proc_alarm_monitor_info", true},
   { &log_level_proc_alarm_monitor_error, &log_handle_proc_alarm_monitor_error, "proc_alarm_monitor_error", true},
   { &log_level_proc_alarm_monitor_external, &log_handle_proc_alarm_monitor_external, "proc_alarm_monitor_external", true},

   { &log_level_proc_pfr_info, &log_handle_proc_pfr_info, "proc_pfr_info", true},
   { &log_level_proc_pfr_error, &log_handle_proc_pfr_error, "proc_pfr_error", true},
   { &log_level_proc_pfr_external, &log_handle_proc_pfr_external, "proc_pfr_external", true},

   { &log_level_proc_pqi_info, &log_handle_proc_pqi_info, "proc_pqi_info", true},
   { &log_level_proc_pqi_error, &log_handle_proc_pqi_error, "proc_pqi_error", true},
   { &log_level_proc_pqi_debug, &log_handle_proc_pqi_debug, "proc_pqi_debug", false},
   { &log_level_proc_pqi_external, &log_handle_proc_pqi_external, "proc_pqi_external", true},

   { &log_level_retoccmon_info, &log_handle_retoccmon_info, "retoccmon_info", true},
   { &log_level_retoccmon_error, &log_handle_retoccmon_error, "retoccmon_error", true},
   { &log_level_retoccmon_external, &log_handle_retoccmon_external, "retoccmon_external", true},

   { &log_level_proc_mss_info,  &log_handle_proc_mss_info, "proc_mss_info", true},
   { &log_level_proc_mss_error, &log_handle_proc_mss_error, "proc_mss_error", true},
   { &log_level_proc_mss_debug, &log_handle_proc_mss_debug, "proc_mss_debug", false},
   { &log_level_proc_mss_external, &log_handle_proc_mss_external, "proc_mss_external", true},

   { &log_level_gui_info,          &log_handle_gui_info,          "gui_info", true},
   { &log_level_gui_debug,         &log_handle_gui_debug,        "gui_debug", false},
   { &log_level_gui_error,         &log_handle_gui_error,         "gui_error", true},
   { &log_level_gui_external,      &log_handle_gui_external,      "gui_external", true},
   { &log_level_gui_startup_debug, &log_handle_gui_startup_debug, "gui_startup_debug", false},

   { &log_level_gui_barcode_info,  &log_handle_gui_barcode_info,  "gui_barcode_info", true},
   { &log_level_gui_barcode_error, &log_handle_gui_barcode_error, "gui_barcode_error", true},
   { &log_level_gui_barcode_external, &log_handle_gui_barcode_external, "gui_barcode_external", true},

   { &log_level_gui_cfr11_info,    &log_handle_gui_cfr11_info,    "gui_cfr11_info", true},
   { &log_level_gui_cfr11_error,   &log_handle_gui_cfr11_error,   "gui_cfr11_error", true},
   { &log_level_gui_cfr11_external,   &log_handle_gui_cfr11_external,   "gui_cfr11_external", true},

   { &log_level_gui_mss_info,      &log_handle_gui_mss_info,      "gui_mss_info", true},
   { &log_level_gui_mss_error,     &log_handle_gui_mss_error,     "gui_mss_error", true},
   { &log_level_gui_mss_debug,     &log_handle_gui_mss_debug,     "gui_mss_debug", false},
   { &log_level_gui_mss_external,     &log_handle_gui_mss_external,     "gui_mss_external", true},

   { &log_level_gui_statline_info, &log_handle_gui_statline_info, "gui_statline_info", true},
   { &log_level_gui_statline_debug, &log_handle_gui_statline_debug, "gui_statline_debug", false},

   { &log_level_gui_mempool_info, &log_handle_gui_mempool_info, "gui_mempool_info", true},
   { &log_level_gui_mempool_error, &log_handle_gui_mempool_error, "gui_mempool_error", true},
   { &log_level_gui_mempool_debug, &log_handle_gui_mempool_debug, "gui_mempool_debug", false},

   { &log_level_barcode_reader_info,   &log_handle_barcode_reader_info,  "barcode_reader_info", true},
   { &log_level_barcode_reader_error,  &log_handle_barcode_reader_error, "barcode_reader_error", true},
   { &log_level_barcode_reader_debug,  &log_handle_barcode_reader_debug, "barcode_reader_debug", false},
   { &log_level_barcode_reader_external,  &log_handle_barcode_reader_external, "barcode_reader_external", true},

   { &log_level_predict_debug, &log_handle_predict_debug, "predict_debug", true},
   { &log_level_predict_info, &log_handle_predict_info, "predict_info", true},
   { &log_level_predict_error, &log_handle_predict_error, "predict_error", true},
   { &log_level_predict_external, &log_handle_predict_external, "predict_external", true},

   { &log_level_ctl_msg_info, &log_handle_ctl_msg_info, "ctl_msg_info", true},
   { &log_level_ctl_msg_error, &log_handle_ctl_msg_error, "ctl_msg_error", true},
   { &log_level_ctl_msg_external, &log_handle_ctl_msg_external, "ctl_msg_external", true},

   { &log_level_ctl_drv_info, &log_handle_ctl_drv_info, "ctl_drv_info", true},
   { &log_level_ctl_drv_error, &log_handle_ctl_drv_error, "ctl_drv_error", true},
   { &log_level_ctl_drv_external, &log_handle_ctl_drv_external, "ctl_drv_external", true},

   { &log_level_cfastmsg_info, &log_handle_cfastmsg_info, "cfastmsg_info", true},
   { &log_level_cfastmsg_error, &log_handle_cfastmsg_error, "cfastmsg_error", true},
   { &log_level_cfastmsg_external, &log_handle_cfastmsg_external, "cfastmsg_external", true},

   { &log_level_rbc_info, &log_handle_rbc_info, "rbc_info", true},
   { &log_level_rbc_error, &log_handle_rbc_error, "rbc_error", true},
   { &log_level_rbc_external, &log_handle_rbc_external, "rbc_external", true},

   { &log_level_sounds_info, &log_handle_sounds_info, "sounds_info", true},
   { &log_level_sounds_error, &log_handle_sounds_error, "sounds_error", true},
   { &log_level_sounds_external, &log_handle_sounds_external, "sounds_external", true},

   { &log_level_safe_msg_info, &log_handle_safe_msg_info, "safe_msg_info", true},
   { &log_level_safe_msg_error, &log_handle_safe_msg_error, "safe_msg_error", true},
   { &log_level_safe_msg_debug, &log_handle_safe_msg_debug, "safe_msg_debug", false},
   { &log_level_safe_msg_external, &log_handle_safe_msg_external, "safe_msg_external", true},

   { &log_level_safe_drv_info, &log_handle_safe_drv_info, "safe_drv_info", true},
   { &log_level_safe_drv_error, &log_handle_safe_drv_error, "safe_drv_error", true},
   { &log_level_safe_drv_debug, &log_handle_safe_drv_debug, "safe_drv_debug", false},
   { &log_level_safe_drv_external, &log_handle_safe_drv_external, "safe_drv_external", true},

   { &log_level_sfastmsg_info, &log_handle_sfastmsg_info, "sfastmsg_info", true},
   { &log_level_sfastmsg_error, &log_handle_sfastmsg_error, "sfastmsg_error", true},
   { &log_level_sfastmsg_debug, &log_handle_sfastmsg_debug, "sfastmsg_debug", false},
   { &log_level_sfastmsg_external, &log_handle_sfastmsg_external, "sfastmsg_external", true},

   { &log_level_safe_exec_info, &log_handle_safe_exec_info, "safe_exec_info", true},
   { &log_level_safe_exec_error, &log_handle_safe_exec_error, "safe_exec_error", true},
   { &log_level_safe_exec_debug, &log_handle_safe_exec_debug, "safe_exec_debug", false},
   { &log_level_safe_exec_external, &log_handle_safe_exec_external, "safe_exec_external", true},

   { &log_level_ftp_info, &log_handle_ftp_info, "ftp_info", true},
   { &log_level_ftp_error, &log_handle_ftp_error, "ftp_error", true},
   { &log_level_ftp_external, &log_handle_ftp_external, "ftp_external", true},

   { &log_level_ted_info, &log_handle_ted_info, "ted_info", true},
   { &log_level_ted_error, &log_handle_ted_error, "ted_error", true},
   { &log_level_ted_debug, &log_handle_ted_debug, "ted_debug", false},
   { &log_level_ted_external, &log_handle_ted_external, "ted_external", true},

   { &log_level_trap_info, &log_handle_trap_info, "trap_info", true},
   { &log_level_trap_error, &log_handle_trap_error, "trap_error", true},
   { &log_level_trap_debug, &log_handle_trap_debug, "trap_debug", false},
   { &log_level_trap_external, &log_handle_trap_external, "trap_external", true},

   { &log_level_trima_broadcast_info, &log_handle_trima_broadcast_info, "trima_broadcast_info", true},
   { &log_level_trima_broadcast_error, &log_handle_trima_broadcast_error, "trima_broadcast_error", true},
   { &log_level_trima_broadcast_external, &log_handle_trima_broadcast_external, "trima_broadcast_external", true},
   { &log_level_trima_broadcast_debug, &log_handle_trima_broadcast_debug, "trima_broadcast_debug", false},

   { &log_level_vip_info, &log_handle_vip_info, "vip_info", true},
   { &log_level_vip_error, &log_handle_vip_error, "vip_error", true},
   { &log_level_vip_debug, &log_handle_vip_debug, "vip_debug", false},
   { &log_level_vip_external, &log_handle_vip_external, "vip_external", true},

   { &log_level_proc_startup_test_info, &log_handle_proc_startup_test_info, "proc_startup_test_info", true},
   { &log_level_proc_startup_test_error, &log_handle_proc_startup_test_error, "proc_startup_test_error", true},
   { &log_level_proc_startup_test_external, &log_handle_proc_startup_test_external, "proc_startup_test_external", true},

   { &log_level_proc_load_cassette_info, &log_handle_proc_load_cassette_info, "proc_load_cassette_info", true},
   { &log_level_proc_load_cassette_error, &log_handle_proc_load_cassette_error, "proc_load_cassette_error", true},
   { &log_level_proc_load_cassette_external, &log_handle_proc_load_cassette_external, "proc_load_cassette_external", true},

   { &log_level_proc_unload_cassette_info, &log_handle_proc_unload_cassette_info, "proc_unload_cassette_info", true},
   { &log_level_proc_unload_cassette_error, &log_handle_proc_unload_cassette_error, "proc_unload_cassette_error", true},
   { &log_level_proc_unload_cassette_external, &log_handle_proc_unload_cassette_external, "proc_unload_cassette_external", true},

   { &log_level_proc_disposable_test_info, &log_handle_proc_disposable_test_info, "proc_disposable_test_info", true},
   { &log_level_proc_disposable_test_error, &log_handle_proc_disposable_test_error, "proc_disposable_test_error", true},
   { &log_level_proc_disposable_test_external, &log_handle_proc_disposable_test_external, "proc_disposable_test_external", true},

   { &log_level_proc_ac_prime_info, &log_handle_proc_ac_prime_info, "proc_ac_prime_info", true},
   { &log_level_proc_ac_prime_error, &log_handle_proc_ac_prime_error, "proc_ac_prime_error", true},
   { &log_level_proc_ac_prime_external, &log_handle_proc_ac_prime_external, "proc_ac_prime_external", true},

   { &log_level_proc_blood_prime_info, &log_handle_proc_blood_prime_info, "proc_blood_prime_info", true},
   { &log_level_proc_blood_prime_error, &log_handle_proc_blood_prime_error, "proc_blood_prime_error", true},
   { &log_level_proc_blood_prime_external, &log_handle_proc_blood_prime_external, "proc_blood_prime_external", true},

   { &log_level_proc_blood_run_info, &log_handle_proc_blood_run_info, "proc_blood_run_info", true},
   { &log_level_proc_blood_run_error, &log_handle_proc_blood_run_error, "proc_blood_run_error", true},
   { &log_level_proc_blood_run_external, &log_handle_proc_blood_run_external, "proc_blood_run_external", true},

   { &log_level_proc_rinseback_info, &log_handle_proc_rinseback_info, "proc_rinseback_info", true},
   { &log_level_proc_rinseback_error, &log_handle_proc_rinseback_error, "proc_rinseback_error", true},
   { &log_level_proc_rinseback_external, &log_handle_proc_rinseback_external, "proc_rinseback_external", true},

   { &log_level_proc_donor_disconnect_info, &log_handle_proc_donor_disconnect_info, "proc_donor_disconnect_info", true},
   { &log_level_proc_donor_disconnect_error, &log_handle_proc_donor_disconnect_error, "proc_donor_disconnect_error", true},
   { &log_level_proc_donor_disconnect_external, &log_handle_proc_donor_disconnect_external, "proc_donor_disconnect_external", true},

   { &log_level_safety_donor_disconnect_info, &log_handle_safety_donor_disconnect_info, "safety_donor_disconnect_info", true},
   { &log_level_safety_donor_disconnect_error, &log_handle_safety_donor_disconnect_error, "safety_donor_disconnect_error", true},
   { &log_level_safety_donor_disconnect_debug, &log_handle_safety_donor_disconnect_debug, "safety_donor_disconnect_debug", false},
   { &log_level_safety_donor_disconnect_external, &log_handle_safety_donor_disconnect_external, "safety_donor_disconnect_external", true},

   { &log_level_proc_recovery_info, &log_handle_proc_recovery_info, "proc_recovery_info", true},
   { &log_level_proc_recovery_error, &log_handle_proc_recovery_error, "proc_recovery_error", true},
   { &log_level_proc_recovery_external, &log_handle_proc_recovery_external, "proc_recovery_external", true},

   { &log_level_config_info, &log_handle_config_info, "config_info", true},
   { &log_level_config_error, &log_handle_config_error, "config_error", true},
   { &log_level_config_external, &log_handle_config_external, "config_external", true},

   { &log_level_trima_library_info, &log_handle_trima_library_info, "trima_library_info", true},
   { &log_level_trima_library_error, &log_handle_trima_library_error, "trima_library_error", true},
   { &log_level_trima_library_external, &log_handle_trima_library_external, "trima_library_external", true},

   { &log_level_control_alarm, &log_handle_control_alarm, "control_alarm", true},
   { &log_level_safety_alarm, &log_handle_safety_alarm, "safety_alarm", true},
   { &log_level_control_alarm_detail, &log_handle_control_alarm_detail, "control_alarm_detail", true},
   { &log_level_safety_alarm_detail, &log_handle_safety_alarm_detail, "safety_alarm_detail", true},

   { &log_level_service_info,  &log_handle_service_info,  "service_info", true},
   { &log_level_service_error, &log_handle_service_error, "service_error", true},
   { &log_level_service_external, &log_handle_service_external, "service_external", true},

   { &log_level_meter_info,  &log_handle_meter_info,  "meter_info",  true},
   { &log_level_meter_error, &log_handle_meter_error, "meter_error", true},
   { &log_level_meter_external, &log_handle_meter_external, "meter_external", true},

   { &log_level_gooddog_info,  &log_handle_gooddog_info,  "gooddog_info",  true},
   { &log_level_gooddog_error, &log_handle_gooddog_error, "gooddog_error", true},
   { &log_level_gooddog_external, &log_handle_gooddog_external, "gooddog_external", true},

   { &log_level_vista_binary, &log_handle_vista_binary, "vista_binary", true},

   { &log_level_spoofer_debug, &log_handle_spoofer_debug, "spoofer_debug", false},
   { &log_level_spoofer_warning, &log_handle_spoofer_warning, "spoofer_warning", true},

   { &log_level_qa_external, &log_handle_qa_external, "qa_external", true},
   { &log_level_qa_external_conditional, &log_handle_qa_external_conditional, "qa_external_conditional", false}

#if CPU==SIMNT
#define __SIM_LEVEL_TABLE__
#include "../unit_test/gui/h/simDataLogLevels.hpp"
#undef __SIM_LEVEL_TABLE__
#endif
};

#endif // #ifndef __DLOG_INIT_TABLE_H__

/* FORMAT HASH 4af96ed35daebe520ec9a6ac777315ec */
