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


#include "localtime-simulator-impl.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/ptr.h"
#include "ns3/assert.h"
#include "ns3/node-list.h"
#include "ns3/node.h"
#include "extended-event-id.h"




/**
 * \file
 * \ingroup simulator
 * ns3::LocalTimeSimulatorImpl implementation
 */

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("LocalTimeSimulatorImpl");

NS_OBJECT_ENSURE_REGISTERED (LocalTimeSimulatorImpl);

TypeId
LocalTimeSimulatorImpl::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LocalTimeSimulatorImpl")
    .SetParent<DefaultSimulatorImpl> ()
    .SetGroupName ("Core")
    .AddConstructor<LocalTimeSimulatorImpl> ()
  ;
  return tid;
}

LocalTimeSimulatorImpl::LocalTimeSimulatorImpl()
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("Local Time Simulator implementation created");
}

LocalTimeSimulatorImpl::~LocalTimeSimulatorImpl ()
{
  NS_LOG_FUNCTION (this);
}

EventId
LocalTimeSimulatorImpl::Schedule (Time const &delay, EventImpl *event)
{
  NS_LOG_INFO (this << delay << event);

  // Some app schedule events to distroy simulatin with a context of 4294967295. We skip that contet because 
  // it doesn't correspond to any node.

  uint32_t context = DefaultSimulatorImpl::GetContext ();
  NS_LOG_DEBUG ("context ... " << context);


  if ( context == uint32_t(4294967295) )
  {
    NS_LOG_DEBUG ("The context doens't correspond to a node");
  }
  else
  {
    m_currentContext = context;
    NS_LOG_DEBUG ("Setting up the context ... " << m_currentContext);
  }
  
  NS_LOG_DEBUG ("Schedule event with Context: " << m_currentContext);
  Ptr <Node>  n = NodeList::GetNode (m_currentContext);
  Ptr <LocalClock> clock = n -> GetObject <LocalClock> ();
  Time globalTimeDelay = clock -> LocalToGlobalAbs (delay);
  EventId eventId = DefaultSimulatorImpl::Schedule (globalTimeDelay, event);

  Ptr <ExtendedEventId> extendedEventId = CreateObject <ExtendedEventId> ();

  extendedEventId -> SetEventId (eventId);
  Time localTimeStamp = clock -> LocalToGlobalTime (TimeStep (eventId.GetTs ()));
  extendedEventId -> SetLocalTimeStamp (localTimeStamp.GetTimeStep ());

  clock -> InsertEvent (extendedEventId);
  
  return eventId;
}

void
LocalTimeSimulatorImpl::ScheduleWithContext (uint32_t context, Time const &delay, EventImpl *event)
{
  NS_LOG_INFO (this << context << delay << event);
  //TODO
  NS_LOG_DEBUG ("Sheduling events with context: " << context);
  DefaultSimulatorImpl::ScheduleWithContext (context,delay,event);

}
}// namespace ns