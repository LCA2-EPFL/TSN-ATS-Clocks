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
#ifndef LOCAL_CLOCK_H
#define LOCAL_CLOCK_H

#include "ns3/object.h"
#include "ns3/object-factory.h"
#include "ns3/clock-model-impl.h"
#include "ns3/scheduler.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/extended-event-id.h"
namespace ns3 {
/**
 * \file
 * \ingroup Clock
 * ns3::LocalClock declaration
 */
/**
 * \ingroup Clock
 * 
 * @brief This class represent the clock aggregate to each node. Each node will have a local clock different
 * from other nodes and form the global simulator clock. This class would allow to schedule events
 * in local time and make the convertion between both domains (Local-Global). Also, each time there
 * is a clock update this class we reschedule the events in the main simulator taking into account the 
 * update.
 */



class LocalClock : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  
  LocalClock ();

  /**
   * \param ClockModelImpl a implementation of a ClockModel Class.
   */
  LocalClock(Ptr<ClockModelImpl> clock);
  ~LocalClock ();

  /**
   * \brief Return a local time that corresponds to node own clock, different
   * from Simulator::Now().
   * \return Node time
   */

  Time GetLocalTime ();
  /**
   * \brief associate a clock model implementation to the clock of the node.
   * This function is going to be called every time a change on the node clock happens 
   * (ie: message update from NTP protocol). This function is going to be in charge of 
   * rescheduling the events when there is a change in the clock. 
   * \param ClockModelImpl associate to this node.
   */
  void SetClock (Ptr<ClockModelImpl> new_clock_model);

  /**
   * \brief Transform Time from Global (simulator time) to Local(Local Node Time).
   * \param globalTime time  
   * \return Local Time  
   */
  Time GlobalToLocalTime (Time globalTime);
  
  /**
   * \brief Transform Time from Local (Local Node Time) to Global (simulator time).
   * \param localTime time
   * \return Global Time  
   */
  Time LocalToGlobalTime (Time localTime);

   /**
   * \brief Transform absolute Time from Global (simulator time) to Local (Local Node Time).
   * \param globalDelay time
   * \return  Local Absolute Time  
   */
  Time GlobalToLocalAbs (Time globalDelay);
  /**
   * \brief Transform absolute Time from Local (Local Node Time) to Global (simulator time).
   * \param localDelay time
   * \return Global Absolute Time  
   */
  Time LocalToGlobalAbs (Time localDelay);
  
  /**
   * \brief Insert a event in m_events to keep track of the events scheduled by this node.  
   * \param event Pointer to the extended eventId to be inserted
   */
  void InsertEvent (Ptr <ExtendedEventId> event);
  /**
   * Return true if SetClock function has been called.
   */
  
private:
  
  /**
   * \brief ReSchedule an Event in the main simulator. This function calculate the remaining 
   * time for the event to be executed and reschedule the event with the new delay updated to 
   * the new clock.
   * \param stamp globaltime of the vent to be rechedule
   * \param impl 
   */
  void ReSchedule (Time globalTimeStamp, EventImpl *impl);

  //Clock implementation for the local clock
  Ptr<ClockModelImpl> m_clock;  
  //List of events schedulled by this node.           
  std::list<Ptr<ExtendedEventId>> m_events;      
  
};

}// namespace ns3

#endif /* LOCAL_CLOCK_H */
