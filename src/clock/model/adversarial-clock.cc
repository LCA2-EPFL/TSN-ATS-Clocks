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

#include "adversarial-clock.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("AdversarialClock");

NS_OBJECT_ENSURE_REGISTERED (AdversarialClock);  

TypeId
AdversarialClock::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AdversarialClock")
    .SetParent<ClockModelImpl> ()
    .SetGroupName ("Clock")
    .AddConstructor<AdversarialClock> ()
    .AddAttribute ("Delta", "",
                  TimeValue(MicroSeconds(1)),
                  MakeTimeAccessor (&AdversarialClock::m_delta),
                  MakeTimeChecker ())
    .AddAttribute ("Period", "",
                  TimeValue(MicroSeconds(1)),
                  MakeTimeAccessor (&AdversarialClock::m_period),
                  MakeTimeChecker ())
    .AddAttribute ("Rho", "",
                  TimeValue(MicroSeconds(1)),
                  MakeTimeAccessor (&AdversarialClock::m_rho),
                  MakeTimeChecker ())        
    .AddAttribute ("Slope", "",
                  TimeValue(MicroSeconds(1)),
                  MakeTimeAccessor (&AdversarialClock::m_slope),
                  MakeTimeChecker ())
    .AddAttribute ("Interval", "",
                  TimeValue(MicroSeconds(1)),
                  MakeTimeAccessor (&AdversarialClock::m_interval),
                  MakeTimeChecker ())                
  ;
  return tid;
}

AdversarialClock::AdversarialClock ()
{
  NS_LOG_FUNCTION (this);
}
AdversarialClock::~AdversarialClock ()
{
  NS_LOG_FUNCTION (this);
}

Time
AdversarialClock::GetLocalTime ()
{
  NS_LOG_FUNCTION (this);
  return Time (0);
}

Time 
AdversarialClock::GlobalToLocalTime (Time globalTime)
{
  NS_LOG_FUNCTION (this << globalTime);
  return Time (0);
}

Time 
AdversarialClock::LocalToGlobalTime (Time localtime)
{
  NS_LOG_FUNCTION (this << localtime);

  if (localtime >   m_xj + Time (localtime.GetDouble % m_period.GetDouble))
  {
    localtime = localtime -  m_period;
  }
  return CalculateRelativeTime (localtime);
  
}

Time
AdversarialClock::GlobalToLocalAbs (Time globaldDelay)
{
  NS_LOG_FUNCTION (this << globaldDelay);
  return Time (0);
}

Time
AdversarialClock::LocalToGlobalAbs (Time localdelay)
{
  NS_LOG_FUNCTION (this << localdelay);
  
  return LocalToGlobalTime (Simulator::Now () + localdelay);
}

Time
AdversarialClock::CalculateRelativeTime (Time time)
{
  NS_LOG_FUNCTION (this << time);
  Time ret;
  if (time <=  m_xj)
  {
    ret = time - (m_delta/2);
  }
  if (m_xj < time <= m_xj + m_interval/m_slope)
  {
    ret = m_slope.GetDouble * (time - m_xj) + m_xj - m_delta/2; 
  }
  if (m_xj + m_interval/m_slope < time <= m_xj + m_interval/m_slope + m_interval)
  {
    ret = (1/m_slope).GetDouble * (time - m_xj + m_interval/m_slope + m_interval + m_xj - m_delta/2);
  }
  if (m_xj + m_interval/m_slope + m_interval < time <= m_xj + m_period)
  {
    ret = time - m_delta/2;
  }
}

}