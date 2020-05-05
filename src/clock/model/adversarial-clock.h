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
#ifndef ADVERSARIAL_CLOCK_H
#define ADVERSARIAL_CLOCK_H

#include "clock-model-impl.h"
#include "ns3/object.h"

namespace ns3 {

/**
 * \file Clock
 * ns3::AdversarialClock declaration
 * 
 * @brief This class represents an adversarial clock modelling. 
 */

class AdversarialClock : public ClockModelImpl
{
public:
  static TypeId GetTypeId (void);

  AdversarialClock ();
  ~AdversarialClock ();

  Time GetLocalTime ();
  Time GlobalToLocalTime (Time globalTime);
  Time LocalToGlobalTime (Time localtime);
  Time GlobalToLocalAbs (Time globaldDelay);
  Time LocalToGlobalAbs (Time localdelay);
private:

  Time m_delta;
  Time m_period;
  Time m_interval;
  Time m_rho;
  double m_slope;
  Time m_epsiddon;
  Time m_xj;
  Time m_xjglobal;
  Time CalculateRelativeTimeGlobalToLocal (Time time);
  Time CalculateRelativeTimeLocalToGlobal (Time time);

};

}//namespace ns3
#endif /*ADVERSARIAL_CLOCK_H*/