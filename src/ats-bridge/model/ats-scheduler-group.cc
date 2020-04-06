/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Natale Patriciello <natale.patriciello@gmail.com>
 *               2016 Stefano Avallone <stavallo@unina.it>
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
 */

#include "ats-scheduler-group.h"
#include "ns3/log.h"

namespace ns3{

 NS_LOG_COMPONENT_DEFINE ("ATSSchedulerGroup");
 NS_OBJECT_ENSURE_REGISTERED (ATSSchedulerGroup); 

TypeId 
ATSSchedulerGroup::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ATSSchedulerGroup")
    .SetParent<Object> ()
    .SetGroupName("ATSBridge")
    .AddConstructor<ATSSchedulerGroup> ()
    ;

  return tid;
}

ATSSchedulerGroup::ATSSchedulerGroup ()
{
  NS_LOG_FUNCTION (this);
  m_NGroup = 0;
}

ATSSchedulerGroup::~ATSSchedulerGroup ()
{
  NS_LOG_FUNCTION (this);
}

void
ATSSchedulerGroup::InsertNewGroup (Time maxResidenceTime, Time groupEligibilityTime)
{
  NS_LOG_FUNCTION (this << maxResidenceTime << groupEligibilityTime);

  m_groupTable[m_NGroup].maxResidenceTime = maxResidenceTime;
  m_groupTable[m_NGroup].groupEligibilityTime = groupEligibilityTime;
  m_NGroup++;
}

Time
ATSSchedulerGroup::GetMaxResidenceTime (uint32_t id)
{
  NS_LOG_FUNCTION (this << id);

  std::map <uint32_t,Parameter_T>::iterator it;

  it = m_groupTable.find (id);

  NS_LOG_DEBUG ("Obtained Residence time for group: " << id << "Time: " << it->second.maxResidenceTime);

  return it->second.maxResidenceTime;
}

Time
ATSSchedulerGroup::GetGroupElibilityTime (uint32_t id)
{
  NS_LOG_FUNCTION (this << id);

  std::map <uint32_t,Parameter_T>::iterator it;

  it = m_groupTable.find (id);

  return it->second.groupEligibilityTime;
}

void
ATSSchedulerGroup::SetGroupElibilityTime (uint32_t id, Time groupTime)
{
  NS_LOG_FUNCTION (this << id << groupTime);

  m_groupTable[id].groupEligibilityTime = groupTime;
}

void
ATSSchedulerGroup::SetMaxResidenceTime (uint32_t id, Time residenceTime)
{
  NS_LOG_FUNCTION (this << id << residenceTime);

  m_groupTable[id].maxResidenceTime = residenceTime;
}


}// namespace ns3