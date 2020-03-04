/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Guillermo Aguirre 
 */

// TODO Logging 



#include "local-clock.h"
#include "ns3/log.h"


/**
 * \file clock
 * ns3::LocalClock implementation 
 */
namespace ns3{ 

NS_LOG_COMPONENT_DEFINE ("LocalClock");

NS_OBJECT_ENSURE_REGISTERED (LocalClock);

TypeId
LocalClock::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LocalClock")
    .SetParent<Object> ()
    .SetGroupName ("Core")
    .AddConstructor<LocalClock> ()
  ;
  return tid;
}
LocalClock::LocalClock ()
{
  NS_LOG_FUNCTION (this);

}
LocalClock::LocalClock (Ptr<ClockModelImpl> clock)
{
  NS_LOG_FUNCTION (this);
  //m_clock = clock;
}

LocalClock::~LocalClock()
{
  NS_LOG_FUNCTION (this);
}

void LocalClock::GetLocalTime (Time &time)
{
 // m_clock->GetLocalTime(time);
}

void LocalClock::SetClock (Ptr<ClockModelImpl> new_clock_model)
{
    //TODO
}

Time LocalClock::GlobalToLocalTime (Time globalTime)
{
  // return m_clock->GlobalToLocalTime (globalTime);
  Time t;
  return t;
}

Time LocalClock::LocalToGlobalTime (Time localTime)
{
  //return m_clock->LocalToGlobalTime (localTime);
  Time t;
  return t;
}

Time LocalClock::GlobalToLocalAbs (Time globalDelay)
{
  //return m_clock->GlobalToLocalAbs (globalDelay);
  Time t;
  return t;
}

Time LocalClock::LocalToGlobalAbs (Time localDelay)
{
  //return m_clock->LocalToGlobalAbs (localDelay);
  Time t;
  return t;
}

void LocalClock::InsertEvent(EventId event)
{
  //TODO
}
void Reschedule(EventId event)
{
  //TODO
}
void UpdateGlobalAbs()
{
  //TODO
}
}//namespace