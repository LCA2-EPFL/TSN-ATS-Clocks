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



#include "ns3/local-clock.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/pointer.h"




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
    .SetGroupName ("Clock")
    .AddConstructor<LocalClock> ()
    .AddAttribute ("ClockModelImpl",
                  "The clock model implementation used to simulate local clock",
                  PointerValue (),
                  MakePointerAccessor (&LocalClock::m_clock),
                  MakePointerChecker<ClockModelImpl> ())
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
  m_clock = clock;
  NS_LOG_DEBUG ("Create Local Clock with contructor");
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
    Ptr<ClockModelImpl> oldClock;
    oldClock = m_clock;
    m_clock = newClock;

    
    for (std::list<Ptr<ExtendedEventId>>::const_iterator iter = m_events.begin (); iter != m_events.end ();)
    {
      if (Simulator::IsExpired ((*iter) -> GetEventId()))
      {
        NS_LOG_DEBUG ("Event expired, remove...eventId : " << (*iter) -> GetEventId ().GetUid ());
        NS_LOG_DEBUG ("EventTimeStamp: " << TimeStep ((*iter) ->  GetEventId ().GetTs ()) << " Simulator::Now : " << Simulator::Now ());
        NS_LOG_DEBUG ("Remove");
        iter = m_events.erase (iter);    
      }
      else
      {
        iter++;
      }
      
    }
    std::list<Ptr<ExtendedEventId>> eventListAux (m_events);
    m_events.clear ();

    for (std::list<Ptr<ExtendedEventId>>::const_iterator iter = eventListAux.begin (); iter != eventListAux.end ();++iter)
  {
      NS_LOG_DEBUG ("Rescheduling event eventId : " <<  (*iter) -> GetEventId ().GetUid ());
      (*iter) -> GetEventId().PeekEventImpl () -> Ref ();

      EventImpl *eventImplPtr = (*iter) -> GetEventId().PeekEventImpl ();
      Simulator::Cancel ((*iter) -> GetEventId());
      LocalClock::ReSchedule ((*iter) -> GetEventId (), oldClock, eventImplPtr);
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

void LocalClock::InsertEvent( Ptr<ExtendedEventId> event)
{
  NS_LOG_FUNCTION (this << event);
  m_events.push_back (event);
  NS_LOG_DEBUG ("Inserting event in the list: " << event -> GetEventId ().GetUid ());
}

void LocalClock::ReSchedule(EventId event, Ptr<ClockModelImpl> oldClock, EventImpl *impl)
{
  Time globalOldDurationRemain;
  Time eventTimeStamp;
  Time localOldDurationRemain;

  eventTimeStamp = TimeStep (event.GetTs ());
  globalOldDurationRemain = eventTimeStamp - Simulator::Now ();
  
  NS_LOG_DEBUG ("eventTimeStamp: " << eventTimeStamp << " Simulator::Now : " << Simulator::Now () << " GlobalOldDurationRemain: " << globalOldDurationRemain);
  
  localOldDurationRemain = oldClock -> GlobalToLocalAbs (globalOldDurationRemain);

  NS_LOG_DEBUG ("Old Global Time " << globalOldDurationRemain.GetTimeStep () << " to Old Local Time " << localOldDurationRemain.GetTimeStep ());
  NS_LOG_DEBUG ("Event timeStamp : " << event.GetTs ());
  
  NS_LOG_DEBUG ("Is cancell ? " << impl ->IsCancelled ());
  Simulator::Schedule (localOldDurationRemain, impl);
}

}//namespace ns3