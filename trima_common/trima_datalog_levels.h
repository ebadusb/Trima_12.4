/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/trima_common/rcs/trima_datalog_levels.h 1.33 2009/08/05 21:29:08Z dslausb Exp dslausb $
 *
 */

#ifndef _TRIMA_DATALOG_LEVELS_INCLUDE
#define _TRIMA_DATALOG_LEVELS_INCLUDE

#include <vxWorks.h> // Needed for CPU compiler directives
#include "datalog_levels.h"
#define SCREENSHOT_RecordType              0x5353  /* "SS" */
#define VISTA_RecordType                   0x5356  /* "VS" */

#define VISTA_AlarmSubType                   0x0001
#define VISTA_AlarmResponseSubType           0x0002
#define VISTA_BarcodeScanSubType             0x0003
#define VISTA_DonorRejectedSubType           0x0004
#define VISTA_ParamAdjustSubType             0x0005
#define VISTA_PFRStatusSubType               0x0006
#define VISTA_DonorVitalsSubType             0x0007
#define VISTA_RunSummarySubType_51_Only      0x0008    // Only used in 5.1
#define VISTA_AuditTrailSubType              0x0009
#define VISTA_RunSummarySubType_52_Only      0x000A    // Only used in 5.2
#define VISTA_CatalogListSubType             0x000B
#define VISTA_InterfaceStateSubType_51_60    0x000C
#define VISTA_RunSummarySubType_60_Only      0x000D    // Only used in 6.0
#define VISTA_RunSummarySubType_62_Only      0x000E
#define VISTA_InterfaceStateSubType          0x000F
#define VISTA_RunSummarySubType              0x0010
#define VISTA_RegionalizationFeatureSubType  0x0011
#define VISTA_RegionalizationConstantSubType 0x0012
#define VISTA_ConfirmationCodeSubType        0x0013
#define VISTA_BarcodeFeedbackSubType         0x0014


#ifdef TRIMA_DATALOG_LEVELS_INIT_TABLE
#define LOG_DEF
#else /* ifdef TRIMA_DATALOG_LEVELS_INIT_TABLE */
#define LOG_DEF extern
#endif /* ifdef TRIMA_DATALOG_LEVELS_INIT_TABLE */

#ifdef __cplusplus
//
// DataLog_Level class only supported in C++
//
LOG_DEF DataLog_Level log_level_startup_info;
LOG_DEF DataLog_Level log_level_startup_error;
LOG_DEF DataLog_Level log_level_startup_detail;
LOG_DEF DataLog_Level log_level_startup_external;

LOG_DEF DataLog_Level log_level_alarm_handler_info;
LOG_DEF DataLog_Level log_level_alarm_handler_error;
LOG_DEF DataLog_Level log_level_alarm_handler_external;

LOG_DEF DataLog_Level log_level_datfile_info;
LOG_DEF DataLog_Level log_level_datfile_error;
LOG_DEF DataLog_Level log_level_datfile_debug;
LOG_DEF DataLog_Level log_level_datfile_external;

LOG_DEF DataLog_Level log_level_proc_info;
LOG_DEF DataLog_Level log_level_proc_error;
LOG_DEF DataLog_Level log_level_proc_debug;
LOG_DEF DataLog_Level log_level_proc_external;

LOG_DEF DataLog_Level log_level_proc_alarm_monitor_info;
LOG_DEF DataLog_Level log_level_proc_alarm_monitor_error;
LOG_DEF DataLog_Level log_level_proc_alarm_monitor_external;

LOG_DEF DataLog_Level log_level_proc_pfr_info;
LOG_DEF DataLog_Level log_level_proc_pfr_error;
LOG_DEF DataLog_Level log_level_proc_pfr_external;

LOG_DEF DataLog_Level log_level_proc_pqi_info;
LOG_DEF DataLog_Level log_level_proc_pqi_error;
LOG_DEF DataLog_Level log_level_proc_pqi_debug;
LOG_DEF DataLog_Level log_level_proc_pqi_external;

LOG_DEF DataLog_Level log_level_retoccmon_info;
LOG_DEF DataLog_Level log_level_retoccmon_error;
LOG_DEF DataLog_Level log_level_retoccmon_external;

LOG_DEF DataLog_Level log_level_proc_mss_error;
LOG_DEF DataLog_Level log_level_proc_mss_info;
LOG_DEF DataLog_Level log_level_proc_mss_debug;
LOG_DEF DataLog_Level log_level_proc_mss_external;

LOG_DEF DataLog_Level log_level_gui_startup_debug;
LOG_DEF DataLog_Level log_level_gui_info;
LOG_DEF DataLog_Level log_level_gui_error;
LOG_DEF DataLog_Level log_level_gui_debug;
LOG_DEF DataLog_Level log_level_gui_external;

LOG_DEF DataLog_Level log_level_gui_barcode_info;
LOG_DEF DataLog_Level log_level_gui_barcode_error;
LOG_DEF DataLog_Level log_level_gui_barcode_external;

LOG_DEF DataLog_Level log_level_gui_cfr11_info;
LOG_DEF DataLog_Level log_level_gui_cfr11_error;
LOG_DEF DataLog_Level log_level_gui_cfr11_external;

LOG_DEF DataLog_Level log_level_gui_mss_info;
LOG_DEF DataLog_Level log_level_gui_mss_error;
LOG_DEF DataLog_Level log_level_gui_mss_debug;
LOG_DEF DataLog_Level log_level_gui_mss_external;

LOG_DEF DataLog_Level log_level_gui_statline_info;
LOG_DEF DataLog_Level log_level_gui_statline_debug;

LOG_DEF DataLog_Level log_level_gui_mempool_info;
LOG_DEF DataLog_Level log_level_gui_mempool_error;
LOG_DEF DataLog_Level log_level_gui_mempool_debug;

LOG_DEF DataLog_Level log_level_gui_ugl_call_debug;

LOG_DEF DataLog_Level log_level_barcode_reader_info;
LOG_DEF DataLog_Level log_level_barcode_reader_error;
LOG_DEF DataLog_Level log_level_barcode_reader_debug;
LOG_DEF DataLog_Level log_level_barcode_reader_external;

LOG_DEF DataLog_Level log_level_predict_debug;
LOG_DEF DataLog_Level log_level_predict_info;
LOG_DEF DataLog_Level log_level_predict_error;
LOG_DEF DataLog_Level log_level_predict_external;

LOG_DEF DataLog_Level log_level_ctl_msg_info;
LOG_DEF DataLog_Level log_level_ctl_msg_error;
LOG_DEF DataLog_Level log_level_ctl_msg_external;

LOG_DEF DataLog_Level log_level_ctl_drv_info;
LOG_DEF DataLog_Level log_level_ctl_drv_error;
LOG_DEF DataLog_Level log_level_ctl_drv_external;

LOG_DEF DataLog_Level log_level_cfastmsg_info;
LOG_DEF DataLog_Level log_level_cfastmsg_error;
LOG_DEF DataLog_Level log_level_cfastmsg_external;

LOG_DEF DataLog_Level log_level_rbc_info;
LOG_DEF DataLog_Level log_level_rbc_error;
LOG_DEF DataLog_Level log_level_rbc_external;

LOG_DEF DataLog_Level log_level_sounds_info;
LOG_DEF DataLog_Level log_level_sounds_error;
LOG_DEF DataLog_Level log_level_sounds_external;

LOG_DEF DataLog_Level log_level_safe_msg_info;
LOG_DEF DataLog_Level log_level_safe_msg_error;
LOG_DEF DataLog_Level log_level_safe_msg_debug;
LOG_DEF DataLog_Level log_level_safe_msg_external;

LOG_DEF DataLog_Level log_level_safe_drv_info;
LOG_DEF DataLog_Level log_level_safe_drv_error;
LOG_DEF DataLog_Level log_level_safe_drv_debug;
LOG_DEF DataLog_Level log_level_safe_drv_external;

LOG_DEF DataLog_Level log_level_sfastmsg_info;
LOG_DEF DataLog_Level log_level_sfastmsg_error;
LOG_DEF DataLog_Level log_level_sfastmsg_debug;
LOG_DEF DataLog_Level log_level_sfastmsg_external;

LOG_DEF DataLog_Level log_level_safe_exec_info;
LOG_DEF DataLog_Level log_level_safe_exec_error;
LOG_DEF DataLog_Level log_level_safe_exec_debug;
LOG_DEF DataLog_Level log_level_safe_exec_external;

LOG_DEF DataLog_Level log_level_ftp_info;
LOG_DEF DataLog_Level log_level_ftp_error;
LOG_DEF DataLog_Level log_level_ftp_external;

LOG_DEF DataLog_Level log_level_ted_info;
LOG_DEF DataLog_Level log_level_ted_error;
LOG_DEF DataLog_Level log_level_ted_debug;
LOG_DEF DataLog_Level log_level_ted_external;

LOG_DEF DataLog_Level log_level_trap_info;
LOG_DEF DataLog_Level log_level_trap_error;
LOG_DEF DataLog_Level log_level_trap_debug;
LOG_DEF DataLog_Level log_level_trap_external;

LOG_DEF DataLog_Level log_level_trima_broadcast_info;
LOG_DEF DataLog_Level log_level_trima_broadcast_error;
LOG_DEF DataLog_Level log_level_trima_broadcast_external;
LOG_DEF DataLog_Level log_level_trima_broadcast_debug;

LOG_DEF DataLog_Level log_level_vip_info;
LOG_DEF DataLog_Level log_level_vip_error;
LOG_DEF DataLog_Level log_level_vip_debug;
LOG_DEF DataLog_Level log_level_vip_external;

LOG_DEF DataLog_Level log_level_proc_startup_test_info;
LOG_DEF DataLog_Level log_level_proc_startup_test_error;
LOG_DEF DataLog_Level log_level_proc_startup_test_external;

LOG_DEF DataLog_Level log_level_proc_load_cassette_info;
LOG_DEF DataLog_Level log_level_proc_load_cassette_error;
LOG_DEF DataLog_Level log_level_proc_load_cassette_external;

LOG_DEF DataLog_Level log_level_proc_unload_cassette_info;
LOG_DEF DataLog_Level log_level_proc_unload_cassette_error;
LOG_DEF DataLog_Level log_level_proc_unload_cassette_external;

LOG_DEF DataLog_Level log_level_proc_disposable_test_info;
LOG_DEF DataLog_Level log_level_proc_disposable_test_error;
LOG_DEF DataLog_Level log_level_proc_disposable_test_external;

LOG_DEF DataLog_Level log_level_proc_ac_prime_info;
LOG_DEF DataLog_Level log_level_proc_ac_prime_error;
LOG_DEF DataLog_Level log_level_proc_ac_prime_external;

LOG_DEF DataLog_Level log_level_proc_blood_prime_info;
LOG_DEF DataLog_Level log_level_proc_blood_prime_error;
LOG_DEF DataLog_Level log_level_proc_blood_prime_external;

LOG_DEF DataLog_Level log_level_proc_blood_run_info;
LOG_DEF DataLog_Level log_level_proc_blood_run_error;
LOG_DEF DataLog_Level log_level_proc_blood_run_external;

LOG_DEF DataLog_Level log_level_proc_rinseback_info;
LOG_DEF DataLog_Level log_level_proc_rinseback_error;
LOG_DEF DataLog_Level log_level_proc_rinseback_external;

LOG_DEF DataLog_Level log_level_proc_donor_disconnect_info;
LOG_DEF DataLog_Level log_level_proc_donor_disconnect_error;
LOG_DEF DataLog_Level log_level_proc_donor_disconnect_external;

LOG_DEF DataLog_Level log_level_safety_donor_disconnect_info;
LOG_DEF DataLog_Level log_level_safety_donor_disconnect_error;
LOG_DEF DataLog_Level log_level_safety_donor_disconnect_debug;
LOG_DEF DataLog_Level log_level_safety_donor_disconnect_external;

LOG_DEF DataLog_Level log_level_proc_recovery_info;
LOG_DEF DataLog_Level log_level_proc_recovery_error;
LOG_DEF DataLog_Level log_level_proc_recovery_external;

LOG_DEF DataLog_Level log_level_config_info;
LOG_DEF DataLog_Level log_level_config_error;
LOG_DEF DataLog_Level log_level_config_external;

LOG_DEF DataLog_Level log_level_trima_library_info;
LOG_DEF DataLog_Level log_level_trima_library_error;
LOG_DEF DataLog_Level log_level_trima_library_external;

LOG_DEF DataLog_Level log_level_control_alarm;
LOG_DEF DataLog_Level log_level_safety_alarm;
LOG_DEF DataLog_Level log_level_control_alarm_detail;
LOG_DEF DataLog_Level log_level_safety_alarm_detail;

LOG_DEF DataLog_Level log_level_vista_binary;

LOG_DEF DataLog_Level log_level_service_info;
LOG_DEF DataLog_Level log_level_service_error;
LOG_DEF DataLog_Level log_level_service_external;

LOG_DEF DataLog_Level log_level_meter_info;
LOG_DEF DataLog_Level log_level_meter_error;
LOG_DEF DataLog_Level log_level_meter_external;

LOG_DEF DataLog_Level log_level_gooddog_info;
LOG_DEF DataLog_Level log_level_gooddog_error;
LOG_DEF DataLog_Level log_level_gooddog_external;

LOG_DEF DataLog_Level log_level_spoofer_debug;
LOG_DEF DataLog_Level log_level_spoofer_warning;

LOG_DEF DataLog_Level log_level_qa_external;             // Used by QA to validate software.  Always on
LOG_DEF DataLog_Level log_level_qa_external_conditional; // Used by QA to validate software.  Off by default

#endif /* ifdef __cplusplus */

LOG_DEF DataLog_Handle log_handle_startup_info;
LOG_DEF DataLog_Handle log_handle_startup_error;
LOG_DEF DataLog_Handle log_handle_startup_detail;
LOG_DEF DataLog_Handle log_handle_startup_external;

LOG_DEF DataLog_Handle log_handle_alarm_handler_info;
LOG_DEF DataLog_Handle log_handle_alarm_handler_error;
LOG_DEF DataLog_Handle log_handle_alarm_handler_external;

LOG_DEF DataLog_Handle log_handle_datfile_info;
LOG_DEF DataLog_Handle log_handle_datfile_error;
LOG_DEF DataLog_Handle log_handle_datfile_debug;
LOG_DEF DataLog_Handle log_handle_datfile_external;

LOG_DEF DataLog_Handle log_handle_proc_info;
LOG_DEF DataLog_Handle log_handle_proc_error;
LOG_DEF DataLog_Handle log_handle_proc_debug;
LOG_DEF DataLog_Handle log_handle_proc_external;

LOG_DEF DataLog_Handle log_handle_proc_alarm_monitor_info;
LOG_DEF DataLog_Handle log_handle_proc_alarm_monitor_error;
LOG_DEF DataLog_Handle log_handle_proc_alarm_monitor_external;

LOG_DEF DataLog_Handle log_handle_proc_pfr_info;
LOG_DEF DataLog_Handle log_handle_proc_pfr_error;
LOG_DEF DataLog_Handle log_handle_proc_pfr_external;

LOG_DEF DataLog_Handle log_handle_proc_pqi_info;
LOG_DEF DataLog_Handle log_handle_proc_pqi_error;
LOG_DEF DataLog_Handle log_handle_proc_pqi_debug;
LOG_DEF DataLog_Handle log_handle_proc_pqi_external;

LOG_DEF DataLog_Handle log_handle_retoccmon_info;
LOG_DEF DataLog_Handle log_handle_retoccmon_error;
LOG_DEF DataLog_Handle log_handle_retoccmon_external;

LOG_DEF DataLog_Handle log_handle_proc_mss_info;
LOG_DEF DataLog_Handle log_handle_proc_mss_error;
LOG_DEF DataLog_Handle log_handle_proc_mss_debug;
LOG_DEF DataLog_Handle log_handle_proc_mss_external;

LOG_DEF DataLog_Handle log_handle_gui_startup_debug;
LOG_DEF DataLog_Handle log_handle_gui_info;
LOG_DEF DataLog_Handle log_handle_gui_debug;
LOG_DEF DataLog_Handle log_handle_gui_error;
LOG_DEF DataLog_Handle log_handle_gui_external;

LOG_DEF DataLog_Handle log_handle_gui_barcode_info;
LOG_DEF DataLog_Handle log_handle_gui_barcode_error;
LOG_DEF DataLog_Handle log_handle_gui_barcode_external;

LOG_DEF DataLog_Handle log_handle_gui_cfr11_info;
LOG_DEF DataLog_Handle log_handle_gui_cfr11_error;
LOG_DEF DataLog_Handle log_handle_gui_cfr11_external;

LOG_DEF DataLog_Handle log_handle_gui_mss_info;
LOG_DEF DataLog_Handle log_handle_gui_mss_error;
LOG_DEF DataLog_Handle log_handle_gui_mss_debug;
LOG_DEF DataLog_Handle log_handle_gui_mss_external;

LOG_DEF DataLog_Handle log_handle_gui_statline_info;
LOG_DEF DataLog_Handle log_handle_gui_statline_debug;

LOG_DEF DataLog_Handle log_handle_gui_mempool_info;
LOG_DEF DataLog_Handle log_handle_gui_mempool_error;
LOG_DEF DataLog_Handle log_handle_gui_mempool_debug;

LOG_DEF DataLog_Handle log_handle_barcode_reader_info;
LOG_DEF DataLog_Handle log_handle_barcode_reader_error;
LOG_DEF DataLog_Handle log_handle_barcode_reader_debug;
LOG_DEF DataLog_Handle log_handle_barcode_reader_external;

LOG_DEF DataLog_Handle log_handle_predict_debug;
LOG_DEF DataLog_Handle log_handle_predict_info;
LOG_DEF DataLog_Handle log_handle_predict_error;
LOG_DEF DataLog_Handle log_handle_predict_external;

LOG_DEF DataLog_Handle log_handle_ctl_msg_info;
LOG_DEF DataLog_Handle log_handle_ctl_msg_error;
LOG_DEF DataLog_Handle log_handle_ctl_msg_external;

LOG_DEF DataLog_Handle log_handle_ctl_drv_info;
LOG_DEF DataLog_Handle log_handle_ctl_drv_error;
LOG_DEF DataLog_Handle log_handle_ctl_drv_external;

LOG_DEF DataLog_Handle log_handle_cfastmsg_info;
LOG_DEF DataLog_Handle log_handle_cfastmsg_error;
LOG_DEF DataLog_Handle log_handle_cfastmsg_external;

LOG_DEF DataLog_Handle log_handle_rbc_info;
LOG_DEF DataLog_Handle log_handle_rbc_error;
LOG_DEF DataLog_Handle log_handle_rbc_external;

LOG_DEF DataLog_Handle log_handle_sounds_info;
LOG_DEF DataLog_Handle log_handle_sounds_error;
LOG_DEF DataLog_Handle log_handle_sounds_external;

LOG_DEF DataLog_Handle log_handle_safe_msg_info;
LOG_DEF DataLog_Handle log_handle_safe_msg_error;
LOG_DEF DataLog_Handle log_handle_safe_msg_debug;
LOG_DEF DataLog_Handle log_handle_safe_msg_external;

LOG_DEF DataLog_Handle log_handle_safe_drv_info;
LOG_DEF DataLog_Handle log_handle_safe_drv_error;
LOG_DEF DataLog_Handle log_handle_safe_drv_debug;
LOG_DEF DataLog_Handle log_handle_safe_drv_external;

LOG_DEF DataLog_Handle log_handle_sfastmsg_info;
LOG_DEF DataLog_Handle log_handle_sfastmsg_error;
LOG_DEF DataLog_Handle log_handle_sfastmsg_debug;
LOG_DEF DataLog_Handle log_handle_sfastmsg_external;

LOG_DEF DataLog_Handle log_handle_safe_exec_info;
LOG_DEF DataLog_Handle log_handle_safe_exec_error;
LOG_DEF DataLog_Handle log_handle_safe_exec_debug;
LOG_DEF DataLog_Handle log_handle_safe_exec_external;

LOG_DEF DataLog_Handle log_handle_ftp_info;
LOG_DEF DataLog_Handle log_handle_ftp_error;
LOG_DEF DataLog_Handle log_handle_ftp_external;

LOG_DEF DataLog_Handle log_handle_ted_info;
LOG_DEF DataLog_Handle log_handle_ted_error;
LOG_DEF DataLog_Handle log_handle_ted_debug;
LOG_DEF DataLog_Handle log_handle_ted_external;

LOG_DEF DataLog_Handle log_handle_trap_info;
LOG_DEF DataLog_Handle log_handle_trap_error;
LOG_DEF DataLog_Handle log_handle_trap_debug;
LOG_DEF DataLog_Handle log_handle_trap_external;

LOG_DEF DataLog_Handle log_handle_trima_broadcast_info;
LOG_DEF DataLog_Handle log_handle_trima_broadcast_error;
LOG_DEF DataLog_Handle log_handle_trima_broadcast_external;
LOG_DEF DataLog_Handle log_handle_trima_broadcast_debug;

LOG_DEF DataLog_Handle log_handle_vip_info;
LOG_DEF DataLog_Handle log_handle_vip_error;
LOG_DEF DataLog_Handle log_handle_vip_debug;
LOG_DEF DataLog_Handle log_handle_vip_external;

LOG_DEF DataLog_Handle log_handle_proc_startup_test_info;
LOG_DEF DataLog_Handle log_handle_proc_startup_test_error;
LOG_DEF DataLog_Handle log_handle_proc_startup_test_external;

LOG_DEF DataLog_Handle log_handle_proc_load_cassette_info;
LOG_DEF DataLog_Handle log_handle_proc_load_cassette_error;
LOG_DEF DataLog_Handle log_handle_proc_load_cassette_external;

LOG_DEF DataLog_Handle log_handle_proc_unload_cassette_info;
LOG_DEF DataLog_Handle log_handle_proc_unload_cassette_error;
LOG_DEF DataLog_Handle log_handle_proc_unload_cassette_external;

LOG_DEF DataLog_Handle log_handle_proc_disposable_test_info;
LOG_DEF DataLog_Handle log_handle_proc_disposable_test_error;
LOG_DEF DataLog_Handle log_handle_proc_disposable_test_external;

LOG_DEF DataLog_Handle log_handle_proc_ac_prime_info;
LOG_DEF DataLog_Handle log_handle_proc_ac_prime_error;
LOG_DEF DataLog_Handle log_handle_proc_ac_prime_external;

LOG_DEF DataLog_Handle log_handle_proc_blood_prime_info;
LOG_DEF DataLog_Handle log_handle_proc_blood_prime_error;
LOG_DEF DataLog_Handle log_handle_proc_blood_prime_external;

LOG_DEF DataLog_Handle log_handle_proc_blood_run_info;
LOG_DEF DataLog_Handle log_handle_proc_blood_run_error;
LOG_DEF DataLog_Handle log_handle_proc_blood_run_external;

LOG_DEF DataLog_Handle log_handle_proc_rinseback_info;
LOG_DEF DataLog_Handle log_handle_proc_rinseback_error;
LOG_DEF DataLog_Handle log_handle_proc_rinseback_external;

LOG_DEF DataLog_Handle log_handle_proc_donor_disconnect_info;
LOG_DEF DataLog_Handle log_handle_proc_donor_disconnect_error;
LOG_DEF DataLog_Handle log_handle_proc_donor_disconnect_external;

LOG_DEF DataLog_Handle log_handle_safety_donor_disconnect_info;
LOG_DEF DataLog_Handle log_handle_safety_donor_disconnect_error;
LOG_DEF DataLog_Handle log_handle_safety_donor_disconnect_debug;
LOG_DEF DataLog_Handle log_handle_safety_donor_disconnect_external;

LOG_DEF DataLog_Handle log_handle_proc_recovery_info;
LOG_DEF DataLog_Handle log_handle_proc_recovery_error;
LOG_DEF DataLog_Handle log_handle_proc_recovery_external;

LOG_DEF DataLog_Handle log_handle_config_info;
LOG_DEF DataLog_Handle log_handle_config_error;
LOG_DEF DataLog_Handle log_handle_config_external;

LOG_DEF DataLog_Handle log_handle_trima_library_info;
LOG_DEF DataLog_Handle log_handle_trima_library_error;
LOG_DEF DataLog_Handle log_handle_trima_library_external;

LOG_DEF DataLog_Handle log_handle_control_alarm;
LOG_DEF DataLog_Handle log_handle_safety_alarm;
LOG_DEF DataLog_Handle log_handle_control_alarm_detail;
LOG_DEF DataLog_Handle log_handle_safety_alarm_detail;

LOG_DEF DataLog_Handle log_handle_service_info;
LOG_DEF DataLog_Handle log_handle_service_error;
LOG_DEF DataLog_Handle log_handle_service_external;

LOG_DEF DataLog_Handle log_handle_meter_info;
LOG_DEF DataLog_Handle log_handle_meter_error;
LOG_DEF DataLog_Handle log_handle_meter_external;

LOG_DEF DataLog_Handle log_handle_gooddog_info;
LOG_DEF DataLog_Handle log_handle_gooddog_error;
LOG_DEF DataLog_Handle log_handle_gooddog_external;

LOG_DEF DataLog_Handle log_handle_vista_binary;

LOG_DEF DataLog_Handle log_handle_spoofer_debug;
LOG_DEF DataLog_Handle log_handle_spoofer_warning;

LOG_DEF DataLog_Handle log_handle_qa_external;
LOG_DEF DataLog_Handle log_handle_qa_external_conditional;

#if CPU==SIMNT
#define __SIM_LEVEL_DEFS__
#include "../unit_test/gui/h/simDataLogLevels.hpp"
#undef __SIM_LEVEL_DEFS__
#endif

/*****************************************************
Note that if you're looking for the datalog init table,
it's moved to dlogInitTable.h.  I did this because I
found it aggrivating that every time I wanted to enable
a debug log level, it required a massive recompile of
the whole software.  This way, most of the software
doesn't depend on this table.  --Dan x4664
*****************************************************/

#undef LOG_DEF

#endif /* ifndef _TRIMA_DATALOG_LEVELS_INCLUDE */

/* FORMAT HASH dd899231bff15821550eb7a27438f689 */
