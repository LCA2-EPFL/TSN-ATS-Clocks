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

#ifndef EXTENDED_EVENT_ID_H
#define EXTENDED_EVENT_ID_H

#include "ns3/event-id.h"
#include "ns3/object.h"
/**
 * \file
 * \ingroup events
 * ns3::ExtendedEventId declarations.
 */

namespace ns3 {
class EventId;
class EventImpl; 

/**
 * \ingroup events
 * \brief An extended identifier for simulation events.
 *
 * This an extended class of EventId. It relates each EventId with a local time timestamp. This class
 * is used in LocalClock class in order to maintain a list of scheduled events by the node asociated with 
 * a local time stamp.   
 */
class ExtendedEventId : public Object
{
public:
   /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Default contructor
   */
  ExtendedEventId ();

  /**
   * \brief Contructor
   * \param Event EventId of the extendedEventId
   */
  ExtendedEventId (EventId event);

  ~ExtendedEventId ();

  EventId GetEventId ();
  uint64_t GetLocalTimeStamp ();

  void SetEventId (EventId event);
  void SetLocalTimeStamp (uint64_t timeStamp);

private:
  //Event ID
  EventId m_eventId;
  //Local Time Stamp asociated to the event
  uint64_t m_localTimeStamp;

};


}//namespace ns3

#endif /* EXTENDED_EVENT_ID_H */