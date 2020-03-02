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

#include "perfect-clock-model-impl.h"
#include "log.h"
#include "simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PerfectClockModelImpl");

NS_OBJECT_ENSURE_REGISTERED (PerfectClockModelImpl);

  PerfectClockModelImpl::PerfectClockModelImpl (double frequency)
  {
    NS_LOG_FUNCTION (this);
    m_frequency = frequency;
    m_timeUpdates.first = Simulator::Now();
    m_timeUpdates.second = Simulator::Now();
  }
  PerfectClockModelImpl::~PerfectClockModelImpl ()
  {
    NS_LOG_FUNCTION (this);
  }

  void GetLocalTime (Time &time)
  {

  }

  void GlobalToLocalTime (Time &globalTime, Time &localTime)
  {

  }
  void LocalToGlobalTime (Time &localtime, Time &globalTime)
  {

  }
  void GlobalToLocalAbs (Time &globaldDelay, Time &localDelay)
  {

  }
  void LocalToGlobalAbs (Time &localdelay, Time &localDelay)
  {
    
  }

}