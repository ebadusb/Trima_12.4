/**
 * Copyright (C) 2016 Terumo BCT, Inc.  All rights reserved.
 *
 * @file   ctl_hw_intf_map.hxx
 *
 * @author mra1usb
 * @date   May 4, 2016
 *
 * @brief  Hardware interface mapping for Trima's control driver
 *
 * Defines the map of generic port IDs (HwPortId) to board-specific registers (HwPortReg).
 * The HW_INTF_DB() macro is defined as necessary to extract the elements of interest.
 * Used for defining the contents of an enum, and initializing a static map of constants.
 */

/* HwPortId=0 vvvvvvvvvvvvvvvvvvvvvv, ISA Port Registers          , idxCCA, idxBAR, PCI Port Registers */
HW_INTF_DB( C1_PWR_SUPPLY           , ISA_C1_PWR_SUPPLY           , CCA_0 , BAR_0 , CTL3_IOP_PWR_SUPPLY )
HW_INTF_DB( IOP_INLET_PWM           , ISA_IOP_INLET_PWM           , CCA_0 , BAR_0 , CTL3_IOP_INLET_PWM )
HW_INTF_DB( IOP_PLATELET_PWM        , ISA_IOP_PLATELET_PWM        , CCA_0 , BAR_0 , CTL3_IOP_PLATELET_PWM )
HW_INTF_DB( IOP_PLASMA_PWM          , ISA_IOP_PLASMA_PWM          , CCA_0 , BAR_0 , CTL3_IOP_PLASMA_PWM )
HW_INTF_DB( IOP_AC_PWM              , ISA_IOP_AC_PWM              , CCA_0 , BAR_0 , CTL3_IOP_AC_PWM )
HW_INTF_DB( IOP_RETURN_PWM          , ISA_IOP_RETURN_PWM          , CCA_0 , BAR_0 , CTL3_IOP_RETURN_PWM )
HW_INTF_DB( IOP_SOUND_LEVEL         , ISA_IOP_SOUND_LEVEL         , CCA_0 , BAR_0 , CTL3_IOP_SOUND_LEVEL )
HW_INTF_DB( INPW_INLET_ENCODER      , ISA_INPW_INLET_ENCODER      , CCA_0 , BAR_0 , CTL3_INPW_INLET_ENCODER )
HW_INTF_DB( INPW_PLATELET_ENCODER   , ISA_INPW_PLATELET_ENCODER   , CCA_0 , BAR_0 , CTL3_INPW_PLATELET_ENCODER )
HW_INTF_DB( INPW_PLASMA_ENCODER     , ISA_INPW_PLASMA_ENCODER     , CCA_0 , BAR_0 , CTL3_INPW_PLASMA_ENCODER )
/* HwPortId=10 vvvvvvvvvvvvvvvvvvvvvv */
HW_INTF_DB( INPW_AC_ENCODER         , ISA_INPW_AC_ENCODER         , CCA_0 , BAR_0 , CTL3_INPW_AC_ENCODER )
HW_INTF_DB( INPW_RETURN_ENCODER     , ISA_INPW_RETURN_ENCODER     , CCA_0 , BAR_0 , CTL3_INPW_RETURN_ENCODER )
HW_INTF_DB( INPW_CTL_HW_REVISION    , ISA_INPW_CTL_HW_REVISION    , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( INP_BUTTONS             , ISA_INP_BUTTONS             , CCA_0 , BAR_0 , CTL3_INP_BUTTONS )
HW_INTF_DB( INP_VALVES              , ISA_INP_VALVES              , CCA_0 , BAR_0 , CTL3_INP_VALVES )
HW_INTF_DB( INP_POWER_STATUS        , ISA_INP_POWER_STATUS        , CCA_0 , BAR_0 , CTL3_INP_POWER_STATUS )
HW_INTF_DB( IOPW_CENT_SPEED         , ISA_IOPW_CENT_SPEED         , CCA_0 , BAR_0 , CTL3_IOPW_CENT_SPEED )
HW_INTF_DB( IOP_ADC_MUX             , ISA_IOP_ADC_MUX             , CCA_0 , BAR_0 , CTL3_IOP_ADC_MUX )
HW_INTF_DB( IOP_VALVE               , ISA_IOP_VALVE               , CCA_0 , BAR_0 , CTL3_IOP_VALVE )
HW_INTF_DB( IOP_ALARM               , ISA_IOP_ALARM               , CCA_0 , BAR_0 , CTL3_IOP_ALARM )
/* HwPortId=20 vvvvvvvvvvvvvvvvvvvvvv */
HW_INTF_DB( IOP_DOOR                , ISA_IOP_DOOR                , CCA_0 , BAR_0 , CTL3_IOP_DOOR )
HW_INTF_DB( IOP_PUMP_ENABLE         , ISA_IOP_PUMP_ENABLE         , CCA_0 , BAR_0 , CTL3_IOP_PUMP_ENABLE )
HW_INTF_DB( IOP_CENT_ENABLE         , ISA_IOP_CENT_ENABLE         , CCA_0 , BAR_0 , CTL3_IOP_CENT_ENABLE )
HW_INTF_DB( WATCHDOG_F0_REG         , ISA_WATCHDOG_F0_REG         , CCA_0 , BAR_0 , CTL3_IO_WATCHDOG_F0_REG )
HW_INTF_DB( WATCHDOG_A5_REG         , ISA_WATCHDOG_A5_REG         , CCA_0 , BAR_0 , CTL3_IO_WATCHDOG_A5_REG )
HW_INTF_DB( WATCHDOG_STATUS_REG     , ISA_WATCHDOG_STATUS_REG     , CCA_0 , BAR_0 , CTL3_IN_WATCHDOG_STATUS )
HW_INTF_DB( WATCHDOG_STATUS_CHK_REG , ISA_WATCHDOG_STATUS_CHK_REG , CCA_0 , BAR_0 , CTL3_IN_WATCHDOG_CHECK )
HW_INTF_DB( INPW_US_HIGH            , ISA_INPW_US_HIGH            , CCA_0 , BAR_0 , CTL3_INW_UPPER_AIR_AND_FLUID )
HW_INTF_DB( INPW_US_LOW             , ISA_INPW_US_LOW             , CCA_0 , BAR_0 , CTL3_INW_LOWER_AIR_AND_FLUID )
HW_INTF_DB( INP_CENT_COMM           , ISA_INP_CENT_COMM           , CCA_0 , BAR_0 , CTL3_INP_CENT_COMM )
/* HwPortId=30 vvvvvvvvvvvvvvvvvvvvvv */
HW_INTF_DB( INP_ODDS_AND_ENDS       , ISA_INP_ODDS_AND_ENDS       , CCA_0 , BAR_0 , CTL3_INP_ODDS_AND_ENDS )
HW_INTF_DB( INPW_ADC_VALUE          , ISA_INPW_ADC_VALUE          , CCA_0 , BAR_0 , CTL3_INPW_ADC_VALUE )
HW_INTF_DB( C1_FPGA_FW_REVISION     , ISA_C1_FPGA_FW_REVISION     , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( C1_FPGA_INTF_REVISION   , ISA_C1_FPGA_INTF_REVISION   , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( C1_FPGA_ID_REVISION     , ISA_C1_FPGA_ID_REVISION     , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( C2_FPGA_FW_REVISION     , ISA_C2_FPGA_FW_REVISION     , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( C2_FPGA_INTF_REVISION   , ISA_C2_FPGA_INTF_REVISION   , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( C2_FPGA_CCA_REVISION    , ISA_C2_FPGA_CCA_REVISION    , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( C2_FPGA_ID_REVISION     , ISA_C2_FPGA_ID_REVISION     , CCA_NA, BAR_NA, REG_NA )
HW_INTF_DB( OUT_CCA_RESET           , ISA_REG_NA                  , CCA_0 , BAR_0 , CTL3_OUT_CCA_RESET )
/* HwPortId=40 vvvvvvvvvvvvvvvvvvvvvv */
HW_INTF_DB( INP_CCA_ID              , ISA_REG_NA                  , CCA_0 , BAR_0 , CTL3_INP_CCA_ID )
