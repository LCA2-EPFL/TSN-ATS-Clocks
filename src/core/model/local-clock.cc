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
#include "log.h"
#include "time.h"


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

LocalClock::LocalClock (Ptr<ClockModelImpl> clock)
{
  NS_LOG_FUNCTION (this);
  m_clock = clock;
}

LocalClock::~LocalClock()
{
  NS_LOG_FUNCTION (this);
}

void LocalClock::GetLocalTime (Time &time)
{
  m_clock->GetLocalTime(time);
}

void LocalClock::SetClock (Ptr<ClockModelImpl> new_clock_model)
{
    //TODO
}

void LocalClock::GlobalToLocalTime (Time &globalTime, Time &localTime)
{
  m_clock->GlobalToLocalTime (globalTime, localTime);
}

void LocalClock::LocalToGlobalTime (Time &localTime, Time &globalTime)
{
  m_clock->LocalToGlobalTime (localTime, globalTime);
}

void LocalClock::GlobalToLocalAbs (Time &globalDelay, Time &localDelay)
{
  m_clock->GlobalToLocalAbs (globalDelay, localDelay);
}

void LocalClock::LocalToGlobalAbs (Time &localDelay, Time &globalDelay)
{
  m_clock->LocalToGlobalAbs (localDelay, globalDelay);
}

void LocalClock::InsertEvent()
{
  //TODO
}
void Reschedule()
{
  //TODO
}
void UpdateGlobalAbs()
{
  //TODO
}
}//namespace