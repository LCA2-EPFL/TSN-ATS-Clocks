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
 * Author: Guillermo Aguirre,
 * Obtained from Matthieu.coudron <matthieu.coudron@lip6.fr> implementation. 
 * Perfect Clock class is just for testing purpose. 
 */
// TODO Logging 

#include "perfect-clock-model-impl.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PerfectClockModelImpl");

NS_OBJECT_ENSURE_REGISTERED (PerfectClockModelImpl);

PerfectClockModelImpl::PerfectClockModelImpl (double frequency)
{
  NS_LOG_FUNCTION (this << frequency);
  m_frequency = frequency;
  m_timeUpdates.first = Simulator::Now();
  m_timeUpdates.second = Simulator::Now();
}
PerfectClockModelImpl::~PerfectClockModelImpl ()
{
  NS_LOG_FUNCTION (this);
}

Time PerfectClockModelImpl::GetLocalTime ()
{
  NS_LOG_FUNCTION (this);
  Time time;
  Time localDuration;
  Time globalDuration = Simulator::Now () - std::get<1>(m_timeUpdates);
  
  NS_ASSERT_MSG (globalDuration < 0, "Global Duration" << globalDuration << "negative. Cannot calculate LocalTime");

  localDuration = GlobalToLocalAbs (globalDuration);
  time = localDuration + std::get<0>(m_timeUpdates);
  NS_LOG_DEBUG("Local Time: " << time);
  return time;
}

Time PerfectClockModelImpl::GlobalToLocalTime (Time globalTime)
{
  NS_LOG_FUNCTION(this << globalTime);
  Time localDuration;
  Time localTime;
  Time globalDuration = globalTime - std::get<1>(m_timeUpdates);

  NS_ASSERT_MSG (globalDuration < 0, "Global Duration" << globalDuration << "negative. Cannot calculate GlobalToLocalTime");
  
  localDuration = GlobalToLocalAbs (globalDuration);
  localTime = std::get<0>(m_timeUpdates) + localDuration;
  NS_LOG_DEBUG("Local Time: " << localTime <<" From Global Time: "<< globalTime);
  return localTime;
}
Time PerfectClockModelImpl::LocalToGlobalTime (Time localTime)
{
  NS_LOG_FUNCTION (this << localTime);
  Time globalTime;
  Time globalDuration;
  Time localDuration = localTime - std::get<0>(m_timeUpdates);
  
  NS_ASSERT_MSG (localDuration < 0, "Local Duration" << localDuration << "negative. Cannot calculate LocalToGlobalTime");

  globalDuration = LocalToGlobalAbs (localDuration);
  globalTime = globalDuration + std::get<1>(m_timeUpdates);
  NS_LOG_DEBUG ( "Global Time:" << globalTime << "From Local Time: " << localTime);
  return globalTime;
}
Time PerfectClockModelImpl::GlobalToLocalAbs (Time globaldDelay)
{
  NS_LOG_FUNCTION (this << globaldDelay);
  Time localDelay;
  localDelay = globaldDelay / m_frequency;
  NS_LOG_DEBUG ("Local Delay: " << localDelay << "From Global Delay: " << globaldDelay);
  return localDelay;
}
Time PerfectClockModelImpl::LocalToGlobalAbs (Time localDelay)
{
  NS_LOG_FUNCTION (this << localDelay);
  Time globalDelay;
  globalDelay = localDelay * m_frequency;
  NS_LOG_DEBUG ("Global Delay: " << globalDelay << "From Local Delay: " << localDelay);
  return globalDelay;
}
}