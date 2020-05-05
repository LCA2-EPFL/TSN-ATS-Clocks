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
                  DoubleValue (1.5),
                  MakeDoubleAccessor (&AdversarialClock::m_slope),
                  MakeDoubleChecker <double> ())
    .AddAttribute ("Interval", "",
                  TimeValue(MicroSeconds(1)),
                  MakeTimeAccessor (&AdversarialClock::m_interval),
                  MakeTimeChecker ())
    .AddAttribute ("xvalue", "",
                  TimeValue(MicroSeconds(1)),
                  MakeTimeAccessor (&AdversarialClock::m_xj),
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
  
  m_xjglobal = m_xj + m_delta/2;
  uint64_t cycles =  globalTime.GetInteger () / m_period.GetInteger ();

  NS_LOG_DEBUG ("Number of cycles global time: " << cycles);
  
  if (globalTime >  m_xjglobal + cycles)
  {
    globalTime = globalTime -  cycles * m_period;
    NS_LOG_DEBUG ("Adjust globaltime: " << globalTime);
  }
  Time ret;
  ret = CalculateRelativeTimeGlobalToLocal (globalTime);
  ret += cycles * m_period; 
  NS_LOG_DEBUG ("Time + cycles = Now LocalTime " << ret);
  return  ret;
}

Time 
AdversarialClock::LocalToGlobalTime (Time localtime)
{
  NS_LOG_FUNCTION (this << localtime);

  uint64_t cycles =  localtime.GetInteger () / m_period.GetInteger ();

  NS_LOG_DEBUG ("Number of cycles: " << cycles);
  
  if (localtime >  m_xj + cycles)
  {
    localtime = localtime -  cycles * m_period;
    NS_LOG_DEBUG ("Adjust localtime: " << localtime);
  }
  Time ret;
  ret = CalculateRelativeTimeLocalToGlobal (localtime);
  ret += cycles*m_period; 
  NS_LOG_DEBUG ("Time + cycles " << ret);
  return  ret;
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
  Time localTime = GlobalToLocalTime (Simulator::Now ());
  Time ret = LocalToGlobalTime (localTime + localdelay);
  ret -=Simulator::Now (); 
  NS_LOG_DEBUG ("Returned timed to simulator " << ret); 
  return ret;
}

Time
AdversarialClock::CalculateRelativeTimeGlobalToLocal (Time time)
{
  NS_LOG_FUNCTION (this << time);

  Time ret;
  if ((time <  m_xjglobal) || (time == m_xjglobal))
  {
      ret = time - (m_delta/2);
      NS_LOG_DEBUG ("Time 1: " << ret);
  }

  if (((m_xjglobal < time) && (time < m_xjglobal + Time ( m_interval.GetDouble ()/m_slope))) || (time == m_xjglobal + Time ( m_interval.GetDouble ()/m_slope)))
  {
    NS_LOG_DEBUG ("mxjglobal " << m_xjglobal);
    NS_LOG_DEBUG ("Div: " << Time ( m_interval.GetDouble ()/m_slope));
    ret = Time (m_slope * (time - m_xjglobal).GetDouble ()) + m_xjglobal - m_delta / 2; 
    NS_LOG_DEBUG ("Time 2: " << ret);
  }

  if (((m_xjglobal + Time ( m_interval.GetDouble ()/m_slope) < time) && (time < m_xjglobal + Time ( m_interval.GetDouble ()/m_slope) + m_interval)) || 
  (time == m_xjglobal + Time ( m_interval.GetDouble ()/m_slope) + m_interval))
  {
    //TODO doens't work
    NS_LOG_DEBUG ("result no time: " <<  (m_interval.GetDouble ())/m_slope);
    ret = Time ((1 / m_slope) * (time - m_xjglobal - m_interval.GetDouble () / m_slope) + m_interval + m_xjglobal - m_delta/2);
    NS_LOG_DEBUG ("Time 3: " << ret);
  }

  if (((m_xjglobal + Time ( m_interval.GetDouble ()/m_slope) + m_interval < time) && (time < m_xjglobal + m_period)) || 
  (time == m_xjglobal + m_period))
  {
    ret = time - m_delta/2;
    NS_LOG_DEBUG ("Time 4: " << ret);
  }

  return ret;
}

Time
AdversarialClock::CalculateRelativeTimeLocalToGlobal (Time time)
{
  NS_LOG_FUNCTION (this << time);

  Time ret;
  if ((time <  m_xj) || (time == m_xj))
  {
    //To do
    ret = time + (m_delta/2);
    NS_LOG_DEBUG ("Time 1: " << ret);
  }

  if (((m_xj < time) && (time < m_xj + Time ( m_interval.GetDouble ()/m_slope))) || (time == m_xj + m_interval))
  {
    NS_LOG_DEBUG ("Rest " << (time - m_xj).GetDouble ());
    NS_LOG_DEBUG ("Rest time " << time - m_xj);
    NS_LOG_DEBUG ("Multi: " << Time ((1 / m_slope) * (time - m_xj).GetDouble ()));
    NS_LOG_DEBUG ("m_j value " << m_xj);
    NS_LOG_DEBUG ("Sum " << Time ((1 / m_slope) * (time - m_xj).GetDouble ()) + m_xj);
    ret = Time ((1 / m_slope) * (time - m_xj).GetDouble ()) + m_xj + m_delta / 2; 
    NS_LOG_DEBUG ("Time 2: " << ret);
  }

  if (((m_xj + m_interval < time) && (time < m_xj + Time ( m_interval.GetDouble ()/m_slope) + m_interval)) || 
  (time == m_xj + Time ( m_interval.GetDouble ()/m_slope) + m_interval))
  {
    NS_LOG_DEBUG ("div no time: " <<  (m_interval.GetDouble ())/m_slope);
    NS_LOG_DEBUG ("div time " << Time ((m_interval.GetDouble ())/m_slope));
    NS_LOG_DEBUG ("res no time" << time.GetDouble () - m_xj.GetDouble () - m_interval.GetDouble () / m_slope);
    NS_LOG_DEBUG ("res  time" <<  Time (time.GetDouble () - m_xj.GetDouble () - m_interval.GetDouble () / m_slope));

    ret = Time ( m_slope * (time.GetDouble () - m_xj.GetDouble () - m_interval.GetDouble () / m_slope) + m_interval + m_xj + m_delta/2);
    NS_LOG_DEBUG ("Time 3: " << ret);
  }

  if (((m_xj + Time ( m_interval.GetDouble ()/m_slope) + m_interval < time) && (time < m_xj + m_period)) || 
  (time == m_xj + m_period))
  {
    ret = time + m_delta/2;
    NS_LOG_DEBUG ("Time 4: " << ret);
  }

  return ret;  
}

}