/*
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      override_alr_msg.cpp
 *
 */

#include <vxworks.h>
#include "override_alr_msg.h"

override_alr_msg::override_alr_msg()
   : _OverrideMsg(MessageBase::SEND_GLOBAL)
{
   clear();
}

override_alr_msg::override_alr_msg(const CallbackBase& cb)
   : _OverrideMsg(cb)
{
   clear();
}

override_alr_msg::~override_alr_msg()
{}

void override_alr_msg::clear ()
{
   override_alr msgData;

   msgData.Qin             = 1.0;
   msgData.Qac             = 1.0;
   msgData.Qrp             = 1.0;
   msgData.Qplasma         = 1.0;
   msgData.Qcollect        = 1.0;
   msgData.centSpeed       = true;
   msgData.centPausedSpeed = false;
   msgData.Vplt            = HAL_VALVE_UNKNOWN;
   msgData.Vrbc            = HAL_VALVE_UNKNOWN;
   msgData.Vpls            = HAL_VALVE_UNKNOWN;
   msgData.lightsOn        = HAL_ALARMLIGHTOFF;
   msgData.alarmActive     = false;
   msgData.rcvdAlarmID     = NULL_ALARM;
   msgData.respAlarmID     = NULL_ALARM;
   msgData.dispAlarmID     = NULL_ALARM;

   _OverrideMsg.setData(msgData);
}

void override_alr_msg::SendMsg ()
{
   _OverrideMsg.send();
}

HAL_VALVESTATE override_alr_msg::Vrbc () const
{
   return _OverrideMsg.getData().Vrbc;
}

void override_alr_msg::Vrbc (HAL_VALVESTATE newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.Vrbc = newVal;
   _OverrideMsg.setData(msgData);
}

HAL_VALVESTATE override_alr_msg::Vpls () const
{
   return _OverrideMsg.getData().Vpls;
}

void override_alr_msg::Vpls (HAL_VALVESTATE newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.Vpls = newVal;
   _OverrideMsg.setData(msgData);
}

HAL_VALVESTATE override_alr_msg::Vplt () const
{
   return _OverrideMsg.getData().Vplt;
}

void override_alr_msg::Vplt (HAL_VALVESTATE newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.Vplt = newVal;
   _OverrideMsg.setData(msgData);
}

bool override_alr_msg::AlarmActive (void) const
{
   return _OverrideMsg.getData().alarmActive;
}

void override_alr_msg::AlarmActive (bool newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.alarmActive = newVal;
   _OverrideMsg.setData(msgData);
}

float override_alr_msg::Qac () const
{
   return _OverrideMsg.getData().Qac;
}

void override_alr_msg::Qac (float newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.Qac = newVal;
   _OverrideMsg.setData(msgData);
}

float override_alr_msg::Qin () const
{
   return _OverrideMsg.getData().Qin;
}

void override_alr_msg::Qin (float newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.Qin = newVal;
   _OverrideMsg.setData(msgData);
}

float override_alr_msg::Qrp () const
{
   return _OverrideMsg.getData().Qrp;
}

void override_alr_msg::Qrp (float newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.Qrp = newVal;
   _OverrideMsg.setData(msgData);
}

float override_alr_msg::Qplasma () const
{
   return _OverrideMsg.getData().Qplasma;
}

void override_alr_msg::Qplasma (float newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.Qplasma = newVal;
   _OverrideMsg.setData(msgData);
}

float override_alr_msg::Qcollect () const
{
   return _OverrideMsg.getData().Qcollect;
}

void override_alr_msg::Qcollect (float newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.Qcollect = newVal;
   _OverrideMsg.setData(msgData);
}

bool override_alr_msg::centSpeed () const
{
   return _OverrideMsg.getData().centSpeed;
}

void override_alr_msg::centSpeed (bool newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.centSpeed = newVal;
   _OverrideMsg.setData(msgData);
}

bool override_alr_msg::centPausedSpeed () const
{
   return _OverrideMsg.getData().centPausedSpeed;
}

void override_alr_msg::centPausedSpeed (bool newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.centPausedSpeed = newVal;
   _OverrideMsg.setData(msgData);
}

HAL_ALARMLIGHTORDERS override_alr_msg::alarmLight (void) const
{
   return _OverrideMsg.getData().lightsOn;
}

void override_alr_msg::alarmLight (HAL_ALARMLIGHTORDERS newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.lightsOn = newVal;
   _OverrideMsg.setData(msgData);
}

ALARM_VALUES override_alr_msg::rcvdAlarmID () const
{
   return _OverrideMsg.getData().rcvdAlarmID;
}

void override_alr_msg::rcvdAlarmID (ALARM_VALUES newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.rcvdAlarmID = newVal;
   _OverrideMsg.setData(msgData);
}

ALARM_VALUES override_alr_msg::respAlarmID () const
{
   return _OverrideMsg.getData().respAlarmID;
}

void override_alr_msg::respAlarmID (ALARM_VALUES newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.respAlarmID = newVal;
   _OverrideMsg.setData(msgData);
}

ALARM_VALUES override_alr_msg::dispAlarmID () const
{
   return _OverrideMsg.getData().dispAlarmID;
}

void override_alr_msg::dispAlarmID (ALARM_VALUES newVal)
{
   override_alr msgData = _OverrideMsg.getData();
   msgData.dispAlarmID = newVal;
   _OverrideMsg.setData(msgData);
}

const override_alr& override_alr_msg::getOverrides () const
{
   return _OverrideMsg.getData();
}

bool override_alr_msg::changed (const override_alr& overrides) const
{
   return ( memcmp(&( _OverrideMsg.getData() ), &overrides, sizeof( override_alr ) ) != 0 );
}

/* FORMAT HASH 04925c31fe285fde0b72e479deeded21 */
