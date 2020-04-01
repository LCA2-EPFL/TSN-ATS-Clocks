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

#include "ats-scheduler-queue-disc.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ats-scheduler-group.h"

namespace ns3{

  NS_LOG_COMPONENT_DEFINE ("ATSSchedulerQueueDisc");

  NS_OBJECT_ENSURE_REGISTERED (ATSSchedulerQueueDisc);

TypeId ATSSchedulerQueueDisc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ATSSchedulerQueueDisc")
    .SetParent<QueueDisc> ()
    .SetGroupName ("ATSBridge")
    .AddConstructor<ATSSchedulerQueueDisc> ()
    .AddAttribute ("Burst",
                   "Size of the committed burst in bytes",
                   UintegerValue (125000),
                   MakeUintegerAccessor (&ATSSchedulerQueueDisc::SetBurstSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Rate",
                   "Rate at which tokens enter the first bucket in bps or Bps.",
                   DataRateValue (DataRate ("125KB/s")),
                   MakeDataRateAccessor (&ATSSchedulerQueueDisc::SetInformationRate),
                   MakeDataRateChecker ())
    .AddAttribute ("ClockOffsetVariationMax",
                   "Max clock offset variation. Difference between ATS scheduler clock and ATS transmission clock max variation." 
                   "If null, it is initialized to 0",
                   UintegerValue (0),
                   MakeUintegerAccessor (&ATSSchedulerQueueDisc::SetClockOffsetVaritionMax),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("ClockRateDeviationMax",
                   "Max absolute deviation from specific nominal rate during operation."
                   "If null, it is initialized to 0",
                   UintegerValue (0),
                   MakeUintegerAccessor (&ATSSchedulerQueueDisc::SetClockRateDeviationMax),
                   MakeUintegerChecker <uint32_t>())
    .AddAttribute ("ArrivalRecognitionDelayMax",
                   "Maximun delay between the time a frame passes the boundary the network physical medium and its subsequence"
                   "recognition by the associated ATS scheduler clock."
                   "If null, it is initialized to 0",
                   UintegerValue (0),
                   MakeUintegerAccessor (&ATSSchedulerQueueDisc::SetArrivalRecognitionDelayMax),
                   MakeUintegerChecker <uint32_t>())
    .AddAttribute ("ProcessDelayMin",
                   "If null, it is initialized to 0",
                   UintegerValue (0),
                   MakeUintegerAccessor (&ATSSchedulerQueueDisc::SetProcessingDelayMin),
                   MakeUintegerChecker <uint32_t>())
    .AddAttribute ("ProcessDelayMax",
                   "If null, it is initialized to 0",
                   UintegerValue (0),
                   MakeUintegerAccessor (&ATSSchedulerQueueDisc::SetProcessingDelayMax),
                   MakeUintegerChecker <uint32_t>())
  ;

  return tid;
}

ATSSchedulerQueueDisc::ATSSchedulerQueueDisc ()
: QueueDisc (QueueDiscSizePolicy::NO_LIMITS)
{
  NS_LOG_FUNCTION (this);
}

ATSSchedulerQueueDisc::~ATSSchedulerQueueDisc ()
{
  NS_LOG_FUNCTION (this);
}

void
ATSSchedulerQueueDisc::SetClockOffsetVaritionMax (Time offset)
{
  NS_LOG_FUNCTION (this);
  m_clockOffsetVariationMax = offset;
}

void
ATSSchedulerQueueDisc::SetClockRateDeviationMax (Time rateDeviation)
{
  NS_LOG_FUNCTION (this);
  m_clockRateDeviationMax = rateDeviation;
}

void 
ATSSchedulerQueueDisc::SetArrivalRecognitionDelayMax (Time arrivalDelay)
{
  NS_LOG_FUNCTION (this);
  m_arrivalRecognitionDelayMax = arrivalDelay;
}

void
ATSSchedulerQueueDisc::SetProcessingDelayMax (Time processDelay)
{
  NS_LOG_FUNCTION (this);
  m_processingDelayMax = processDelay;
}

void
ATSSchedulerQueueDisc::SetProcessingDelayMin (Time processDelay)
{
  NS_LOG_FUNCTION (this);
  m_processingDelayMin = processDelay;
}
void 
ATSSchedulerQueueDisc::SetBurstSize (uint32_t burst)
{
  NS_LOG_FUNCTION (this);
  m_burstSize = burst;
}

void 
ATSSchedulerQueueDisc::SetInformationRate (DataRate rate)
{
  NS_LOG_FUNCTION (this);
  m_informationRate = rate;
}

Time 
ATSSchedulerQueueDisc::GetClockOffsetVaritionMax ()
{
  NS_LOG_FUNCTION (this);
  return m_clockOffsetVariationMax;
}

Time
ATSSchedulerQueueDisc::GetClockRateDeviationMax ()
{
  NS_LOG_FUNCTION (this);
  return m_clockRateDeviationMax;
}

Time
ATSSchedulerQueueDisc::GetArrivalRecognitionDelayMax ()
{
  NS_LOG_FUNCTION (this);
  return m_arrivalRecognitionDelayMax;
}

Time
ATSSchedulerQueueDisc::GetProcessingDelayMax ()
{
  NS_LOG_FUNCTION (this);
  return m_processingDelayMax;
}
Time 
ATSSchedulerQueueDisc::GetProcessingDelayMin ()
{
  NS_LOG_FUNCTION (this);
  return m_processingDelayMin;
}

DataRate
ATSSchedulerQueueDisc::GetInformationRate ()
{
  NS_LOG_FUNCTION (this);
  return m_informationRate;
}

uint32_t
ATSSchedulerQueueDisc::GetBurstSize ()
{
  NS_LOG_FUNCTION (this);
  return m_burstSize;
}

bool
ATSSchedulerQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item)
{
  NS_LOG_FUNCTION (this << item);
  //Set group parameters

  Time maxResidenceTime;
  Time groupElibilityTime;

  ATSSchedulerGroup group = this->GetObject<ATSSchedulerGroup>;
  maxResidenceTime = group.GetMaxResidenceTime (m_SchedulerGroupId);
  groupElibilityTime = group.GetGroupElibilityTime (m_SchedulerGroupId);
  //implementetation of the Tocken Bucket shaper state machine

  Time lenthRecoveryDuration = item->GetPacket.GetSize / m_informationRate;
  NS_LOG_LOGIC ("LenthRecoveryDuration " << lenthRecoveryDuration);
  Time emptyToFullDuration = m_burstSize / m_informationRate.GetBitRate*8;
  NS_LOG_LOGIC ("Empty to full Duration " << emptyToFullDuration);
  Time schedulerEleigibilityTime = m_bucketEmptyTime + emptyToFullDuration;
  NS_LOG_LOGIC ("Scheduler Eligibility Time " << schedulerEleigibilityTime);
  Time bucketFullTime = m_bucketEmptyTime + emptyToFullDuration;
  NS_LOG_LOGIC ("Bucket Full Time " << bucketFullTime);
  Time eligibilityTime = Max(Max (Simulator::Now(),schedulerEleigibilityTime),groupElibilityTime);
  NS_LOG_LOGIC ("Eligibility Time " << eligibilityTime);

  if (eligibilityTime <= Simulator::Now() + (maxResidenceTime / 1.0E9))
  {
    //The frame is valid
    group.SetGroupElibilityTime (m_SchedulerGroupId,eligibilityTime);
    if (eligibilityTime < bucketFullTime)
    {
      m_bucketEmptyTime = schedulerEleigibilityTime;
    }
    else
    {
      m_bucketEmptyTime = schedulerEleigibilityTime + eligibilityTime - bucketFullTime;
    } 
    AssingAndProceed (eligibilityTime, item);  
    return true;
  }
  else 
  {
    //The frame is invalid: Do not enqueue. 
    return false;
  }
}

Ptr<QueueDiscItem>
ATSSchedulerQueueDisc::DoDequeue ()
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (true,"This function shouldn't be called");
}

bool 
ATSSchedulerQueueDisc::CheckConfig ()
{
  NS_LOG_FUNCTION (this);
  if (GetNInternalQueues () > 0)
    {
      NS_LOG_ERROR ("ATSSchedulerQueueDisc cannot have internal queues");
      return false;
    }

  if (GetNPacketFilters () > 0)
    {
      NS_LOG_ERROR ("ATSSchedulerQueueDisc cannot have packet filters");
      return false;
    }

  if (GetNQueueDiscClasses () > 0)
    {
      NS_LOG_ERROR ("ATSSchedulerQueueDisc cannot have classes");
      return false;
    }
}

void
ATSSchedulerQueueDisc::AssingAndProceed (Time eligibilityTime, Ptr<QueueDiscItem> item)
{
  NS_LOG_FUNCTION (this << eligibilityTime << item);
  Time assignedEligibilityTime = eligibilityTime + m_clockOffsetVariationMax + m_processingDelayMax;
  NS_LOG_LOGIC ("Assing Elibility Time " << assignedEligibilityTime); 
  //Schedule the event (at elegibility time) that will insert the packet in the FIFO queue 
  //of ATSTransmissionQueueDisc. Them call to QueueDisc::Run to notify that there is a packet 
  //ready to transmit.
  //TODO change the method to ATS::Transmission Queue
  Simulator::Schedule (eligibilityTime, &m_enqueueCallBack,this, item);
  //TOO how can be call the rootQueue to execute Run. Maybe a flag to indicate 
  Simulator::ScheduleNow (&QueueDisc::Run, this);
}
void
ATSSchedulerQueueDisc::SetEnqueueCallBack (EnqueueCallBack ec)
{ 
  NS_LOG_FUNCTION (this << &ec);
  m_enqueueCallBack = ec;
}

void 
ATSSchedulerQueueDisc::InitializeParams ()
{
  NS_LOG_FUNCTION (this);
  //TODO: maybe initialize params here before first packet enqueue (ie, bucketEmptytime...)
  m_bucketEmptyTime = Simulator::Now ();
}

}