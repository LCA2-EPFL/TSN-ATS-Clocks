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
#ifndef LOCALTIME_SIMULATOR_IMPL_H
#define LOCALTIME_SIMULATOR_IMPL_H

#include "ns3/default-simulator-impl.h"
#include "local-clock.h"



/**
 * \file
 * \ingroup simulator
 * ns3::LocalTimetSimulatorImpl declaration.
 */

namespace ns3{

/**
 *  \ingroup simulator
 * 
 * @brief Implementation of single process simulator based on node local time. 
 * This class translate the local delay (node time) to global delay (simulation time)
 * when function Schedule and SchedulewithContext are called.
 */

class LocalTimeSimulatorImpl : public DefaultSimulatorImpl
{
public:
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /** Contructor. */
  LocalTimeSimulatorImpl ();
  /** Destructor. */
  ~LocalTimeSimulatorImpl();

  /**
   * This function provides a mechanism to translate the delay which is based on the node local time. 
   * The node implementation is obtained through the node list and the current context.
   * DefaultSimulatorImpl::Schedule(const Time &delay, EventImpl *event) is called to simulate 
   * the events in the main simulator.
   */
  virtual EventId Schedule (const Time &delay, EventImpl *event);

  /**
   * TODO decide how to treat packets where the context is the next receving node and context chage.
   */
  virtual void ScheduleWithContext (uint32_t context, const Time &delay, EventImpl *event);
  
private:
  /** Execution context of the current event. */
  uint32_t m_currentContext;
};

}// namespace ns3

#endif /* LOCALTIME_SIMULATOR_IMPL_H */

