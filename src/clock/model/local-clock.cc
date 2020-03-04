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
#include "ns3/simulator.h"



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
  NS_LOG_FUNCTION (this << clock);
  m_clock = clock;
}

LocalClock::~LocalClock()
{
  NS_LOG_FUNCTION (this);
}

Time LocalClock::GetLocalTime ()
{
  NS_LOG_FUNCTION (this);
  return m_clock->GetLocalTime();
}

void LocalClock::SetClock (Ptr<ClockModelImpl> newClock)
{
    NS_LOG_FUNCTION (this << newClock);
    NS_LOG_DEBUG ("New Clock");

    for (std::list<Ptr<ExtendedEventId>>::const_iterator iter = m_events.begin(), end = m_events.end(); iter != end; ++iter)
    {
      if (Simulator::IsExpired ((*iter) -> GetEventId()))
      {
        //Todo: Remove from the list
        m_events.remove ((*iter));
      }
      else
      {
        NS_LOG_DEBUG ("Rescheduling event");
        Ptr<ClockModelImpl> oldClock;
        oldClock = m_clock;
        m_clock = newClock;
        ReSchedule ((*iter) -> GetEventId (), oldClock);
      }
    }
}

Time LocalClock::GlobalToLocalTime (Time globalTime)
{
  NS_LOG_FUNCTION (this << globalTime);
  return m_clock->GlobalToLocalTime (globalTime);
}

Time LocalClock::LocalToGlobalTime (Time localTime)
{
  NS_LOG_FUNCTION (this << localTime);
  return m_clock->LocalToGlobalTime (localTime);
}

Time LocalClock::GlobalToLocalAbs (Time globalDelay)
{
  NS_LOG_FUNCTION (this << globalDelay);
  return m_clock->GlobalToLocalAbs (globalDelay);
}

Time LocalClock::LocalToGlobalAbs (Time localDelay)
{
  NS_LOG_FUNCTION (this << localDelay);
  return m_clock->LocalToGlobalAbs (localDelay);
}

void LocalClock::InsertEvent( Ptr <ExtendedEventId> event)
{
  NS_LOG_FUNCTION (this << event);
  m_events.push_back (event);
}
void Reschedule(EventId event, Ptr<ClockModelImpl> oldClock)
{
  Time globalOldDurationRemain;
  Time eventTimeStamp;
  Time localOldDurationRemain;

  eventTimeStamp = Time(event.GetTs ());
  Simulator::Remove (event);
  globalOldDurationRemain = eventTimeStamp - Simulator::Now ();
  
  NS_ASSERT_MSG (globalOldDurationRemain.GetTimeStep () < 0, "Remaining GlobalTime is negative" << globalOldDurationRemain.GetTimeStep);
  
  localOldDurationRemain = oldClock -> GlobalToLocalAbs (globalOldDurationRemain);

  NS_LOG_DEBUG ("Old Global Time" << globalOldDurationRemain << "to Old Local Time" << localOldDurationRemain);
  
  Simulator::Schedule (localOldDurationRemain, event.PeekEventImpl());
}
}//namespace ns3