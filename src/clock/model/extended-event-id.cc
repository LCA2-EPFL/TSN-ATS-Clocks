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
#include "extended-event-id.h"
#include "ns3/log.h"


/**
 * \file
 * \ingroup events
 * ns3::ExtendedEventId implementation.
 */

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ExtendedEventId");

ExtendedEventId::ExtendedEventId (EventId eventid, uint64_t localTimeStamp)
{  
  NS_LOG_FUNCTION (this);
  //m_eventId = EventId(eventid.PeekEventImpl, eventid.GetTs, eventid.GetContext, eventid.GetUid);
  //m_localTimeStamp = localTimeStamp; 
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
} //namespace

  