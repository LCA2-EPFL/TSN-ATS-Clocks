/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005 INRIA
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
 * 
 */
#include "ns3/extended-event-id.h"
#include "ns3/log.h"


/**
 * \file
 * \ingroup events
 * ns3::ExtendedEventId implementation.
 */

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ExtendedEventId");

TypeId
ExtendedEventId::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ExtendedEventId")
    .SetParent<Object> ()
    .SetGroupName ("Clock")
    .AddConstructor<ExtendedEventId> ()
  ;
  return tid;
}

ExtendedEventId::ExtendedEventId ()
{  
  NS_LOG_FUNCTION (this);
}
ExtendedEventId::ExtendedEventId (EventId event)
{
  m_eventId = EventId (event.PeekEventImpl (), event.GetTs (),event.GetContext (), event.GetUid ()); 
}

ExtendedEventId::~ExtendedEventId ()
{
  NS_LOG_FUNCTION (this);
}

EventId
ExtendedEventId::GetEventId ()
{
  return m_eventId;
}

uint64_t 
ExtendedEventId::GetLocalTimeStamp ()
{
  return m_localTimeStamp;
}

void
ExtendedEventId::SetEventId (EventId event)
{
   //EventId (event.PeekEventImpl (), event.GetTs (), event.GetContext (), event.GetUid ());
}
void
ExtendedEventId::SetLocalTimeStamp (uint64_t timeStamp)
{
  m_localTimeStamp = timeStamp;
}
} //namespace

  