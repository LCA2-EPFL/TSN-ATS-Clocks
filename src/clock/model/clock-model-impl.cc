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

#include "clock-model-impl.h"
#include "ns3/log.h"

/**
 * \file clock
 * \ingroup clock
 * ns3::ClockModelImpl implementation
 */

namespace ns3{
NS_LOG_COMPONENT_DEFINE ("ClockModelImpl");

NS_OBJECT_ENSURE_REGISTERED (ClockModelImpl);

TypeId
ClockModelImpl::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ClockModelImpl")
    .SetParent<Object> ()
    .SetGroupName ("Clock")
  ;
  return tid;
}

ClockModelImpl::~ClockModelImpl ()
{
  NS_LOG_FUNCTION (this);
}
}