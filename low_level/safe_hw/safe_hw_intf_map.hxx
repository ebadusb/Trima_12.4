/**
 * Copyright (C) 2016 Terumo BCT, Inc.  All rights reserved.
 *
 * @file   safe_hw_intf_map.hxx
 *
 * @author mra1usb
 * @date   May 4, 2016
 *
 * @brief  Hardware interface mapping for Trima's safety driver
 *
 * Defines the map of generic port IDs (HwPortId) to board-specific registers (HwPortReg).
 * The HW_INTF_DB() macro is defined as necessary to extract the elements of interest.
 * Used for defining the contents of an enum, and initializing a static map of constants.
 */

/* !! Don't use these, as we need to allow for multiple inclusions in same module !!
 * #ifndef SAFE_HW_INTF_MAP_HXX_
 * #define SAFE_HW_INTF_MAP_HXX_
 */
HW_INTF_DB( INP_VALVES                , ISA_INP_VALVES                , CCA_0 , BAR_0 , SAFP_INP_VALVES )
HW_INTF_DB( INP_HALLS                 , ISA_INP_HALLS                 , CCA_0 , BAR_0 , SAFP_INP_HALLS )
HW_INTF_DB( INPW_ULTRASONICS          , ISA_INPW_ULTRASONICS          , CCA_0 , BAR_0 , SAFP_INPW_ULTRASONICS )
HW_INTF_DB( INP_CENTRIFUGE            , ISA_INP_CENTRIFUGE            , CCA_0 , BAR_0 , SAFP_INP_CENTRIFUGE )
HW_INTF_DB( INP_SWITCHES              , ISA_INP_SWITCHES              , CCA_0 , BAR_0 , SAFP_INP_SWITCHES )
HW_INTF_DB( INP_BASIN_DATA            , ISA_INP_BASIN_DATA            , CCA_0 , BAR_0 , SAFP_INP_BASIN_DATA )
HW_INTF_DB( IOP_POWER                 , ISA_IOP_POWER                 , CCA_0 , BAR_0 , SAFP_IOP_POWER )
HW_INTF_DB( OUTP_CLR_CENT_FAULT       , ISA_OUTP_CLR_CENT_FAULT       , CCA_0 , BAR_0 , SAFP_OUTP_CLR_CENT_FAULT )
HW_INTF_DB( WATCHDOG_F0_REG           , ISA_WATCHDOG_F0_REG           , CCA_0 , BAR_0 , SAFP_OUTP_WATCHDOG_F0 )
HW_INTF_DB( WATCHDOG_A5_REG           , ISA_WATCHDOG_A5_REG           , CCA_0 , BAR_0 , SAFP_OUTP_WATCHDOG_A5 )
HW_INTF_DB( WATCHDOG_STATUS_REG       , ISA_WATCHDOG_STATUS_REG       , CCA_0 , BAR_0 , SAFP_INP_WATCHDOG_STATUS )
HW_INTF_DB( WATCHDOG_STATUS_CHK_REG   , ISA_WATCHDOG_STATUS_CHK_REG   , CCA_0 , BAR_0 , SAFP_INP_WATCHDOG_CHECK )
/* ISA-only */
HW_INTF_DB( INP_SAFE_HW_REVISION      , ISA_INP_SAFE_HW_REVISION      , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( SAFETY_FPGA_CCA_REVISION  , ISA_SAFETY_FPGA_CCA_REVISION  , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( SAFETY_FPGA_FW_REVISION   , ISA_SAFETY_FPGA_FW_REVISION   , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( SAFETY_FPGA_INTF_REVISION , ISA_SAFETY_FPGA_INTF_REVISION , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( SAFETY_FPGA_ID_REVISION   , ISA_SAFETY_FPGA_ID_REVISION   , CCA_NA, BAR_NA, REG_NA )
/* PCI-only */
HW_INTF_DB( OUT_CCA_RESET             , ISA_REG_NA                    , CCA_0 , BAR_0 , SAFP_OUT_CCA_RESET )
HW_INTF_DB( INP_CCA_ID                , ISA_REG_NA                    , CCA_0 , BAR_0 , SAFP_INP_CCA_ID )
/* #endif SAFE_HW_INTF_MAP_HXX_ */
