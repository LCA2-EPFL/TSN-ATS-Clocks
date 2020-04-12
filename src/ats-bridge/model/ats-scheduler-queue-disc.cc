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
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/pointer.h"

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
                   UintegerValue (1000),
                   MakeUintegerAccessor (&ATSSchedulerQueueDisc::SetBurstSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Rate",
                   "Rate at which tokens enter the first bucket in bps or Bps.",
                   DataRateValue (DataRate ("100KB/s")),
                   MakeDataRateAccessor (&ATSSchedulerQueueDisc::m_informationRate),
                   MakeDataRateChecker ())
    .AddAttribute ("ClockOffsetVariationMax",
                   "Max clock offset variation. Difference between ATS scheduler clock and ATS transmission clock max variation." 
                   "If null, it is initialized to 0",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&ATSSchedulerQueueDisc::m_clockOffsetVariationMax),
                   MakeTimeChecker ())
    .AddAttribute ("ClockRateDeviationMax",
                   "Max absolute deviation from specific nominal rate during operation."
                   "If null, it is initialized to 0",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&ATSSchedulerQueueDisc::m_clockRateDeviationMax),
                   MakeTimeChecker ())
    .AddAttribute ("ArrivalRecognitionDelayMax",
                   "Maximun delay between the time a frame passes the boundary the network physical medium and its subsequence"
                   "recognition by the associated ATS scheduler clock."
                   "If null, it is initialized to 0",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&ATSSchedulerQueueDisc::m_arrivalRecognitionDelayMax),
                   MakeTimeChecker ())
    .AddAttribute ("ProcessDelayMin",
                   "If null, it is initialized to 0",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&ATSSchedulerQueueDisc::m_processingDelayMin),
                   MakeTimeChecker ())
    .AddAttribute ("ProcessDelayMax",
                   "If null, it is initialized to 0",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&ATSSchedulerQueueDisc::m_processingDelayMax),
                   MakeTimeChecker ())
    .AddAttribute ("MaxSize",
                   "The max queue size",
                   QueueSizeValue (QueueSize ("1000p")),
                   MakeQueueSizeAccessor (&QueueDisc::SetMaxSize,
                                          &QueueDisc::GetMaxSize),
                   MakeQueueSizeChecker ())
    .AddAttribute ("Group",
                  "Pointer to the group table",
                  PointerValue (0),
                  MakePointerAccessor (&ATSSchedulerQueueDisc::m_group),
                  MakePointerChecker<ATSSchedulerGroup> ())
    .AddAttribute ("GroupID",
                  "ID of the group it belongs",
                  UintegerValue (0),
                  MakeUintegerAccessor (&ATSSchedulerQueueDisc::m_SchedulerGroupId),
                  MakeUintegerChecker<uint32_t> ())              
  ;

  return tid;
}

ATSSchedulerQueueDisc::ATSSchedulerQueueDisc ()
: QueueDisc (QueueDiscSizePolicy::SINGLE_CHILD_QUEUE_DISC)
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

void
ATSSchedulerQueueDisc::SetATSGroup (Ptr<ATSSchedulerGroup> group, uint32_t id)
{
  NS_LOG_FUNCTION (this << group << id);
  m_group = group;
  m_SchedulerGroupId = id;
}

bool
ATSSchedulerQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item)
{
  NS_LOG_FUNCTION (this << item);

  Time maxResidenceTime;
  Time groupElibilityTime;
  Time arrivalTime = Simulator::Now ();

  maxResidenceTime = m_group->GetMaxResidenceTime (m_SchedulerGroupId);
  groupElibilityTime = m_group->GetGroupElibilityTime (m_SchedulerGroupId);

  Time lenthRecoveryDuration = m_informationRate.CalculateBytesTxTime (item->GetPacket()->GetSize ());
  Time emptyToFullDuration = m_informationRate.CalculateBytesTxTime (m_burstSize);
  Time schedulerEligibilityTime = m_bucketEmptyTime + lenthRecoveryDuration;
  Time bucketFullTime = m_bucketEmptyTime + emptyToFullDuration;
  Time eligibilityTime = Max(Max (arrivalTime,schedulerEligibilityTime),groupElibilityTime);
  
  NS_LOG_DEBUG ("Frame length: " << item->GetPacket()->GetSize ()*8 << "Information Rate: " << m_informationRate.GetBitRate ());
  NS_LOG_DEBUG ("LenthRecoveryDuration " << lenthRecoveryDuration);
  NS_LOG_DEBUG ("Empty to full Duration " << emptyToFullDuration);
  NS_LOG_DEBUG ("Scheduler Eligibility Time " << schedulerEligibilityTime);
  NS_LOG_DEBUG ("Bucket Full Time " << bucketFullTime);
  NS_LOG_DEBUG ("Group Eligibility Time:" << groupElibilityTime);
  NS_LOG_DEBUG ("Eligibility Time " << eligibilityTime);
  NS_LOG_DEBUG ("MaxResidenceTime " << maxResidenceTime);
  NS_LOG_DEBUG ("Arrival Time + MaxResidenceTime " <<  (arrivalTime.GetNanoSeconds () +  (maxResidenceTime.GetNanoSeconds ())));
  
  
  if (eligibilityTime <= arrivalTime.GetNanoSeconds () +  (maxResidenceTime.GetNanoSeconds ()))
  {
    //The frame is valid
    m_group->SetGroupElibilityTime (m_SchedulerGroupId,eligibilityTime);
    if (eligibilityTime < bucketFullTime)
    {
      m_bucketEmptyTime = schedulerEligibilityTime;
      NS_LOG_DEBUG ("EligibilityTime < bucketFullTime -> BucketEmptyTime = " << m_bucketEmptyTime);
    }
    else
    {
      m_bucketEmptyTime = schedulerEligibilityTime + eligibilityTime - bucketFullTime;
      NS_LOG_DEBUG ("EligibilityTime > bucketFullTime -> BucketEmptyTime = " << m_bucketEmptyTime);
    } 
    return AssingAndProceed (eligibilityTime, item);  
    
  }
  else 
  {
    //The frame is invalid: Do not enqueue. 
    NS_LOG_DEBUG ("FRAME INVALID");
    return false;
  }
}

bool
ATSSchedulerQueueDisc::AssingAndProceed (Time eligibilityTime, Ptr<QueueDiscItem> item)
{
  NS_LOG_FUNCTION (this << eligibilityTime << item);
  Time assignedEligibilityTime = eligibilityTime + m_clockOffsetVariationMax + m_processingDelayMax;
  NS_LOG_DEBUG ("Assing Elibility Time " << assignedEligibilityTime); 


  //Schedule the event (at elegibility time) that will insert the packet in the FIFO queue 
  //of ATSTransmissionQueueDisc. Them call to QueueDisc::Run to notify that there is a packet 
  //ready to transmit.
  //TODO change the method to ATS::Transmission Queue

  bool retval;
  retval = GetQueueDiscClass (0)->GetQueueDisc ()->Enqueue (item);


  //Schedule an event which will enqueue the item in the the transmission queue 
 
  Simulator::Schedule (assignedEligibilityTime - Simulator::Now (), &ATSSchedulerQueueDisc::ReadyForTransmission, this);
  return retval;
}

Ptr<QueueDiscItem>
ATSSchedulerQueueDisc::DoDequeue ()
{
  NS_LOG_FUNCTION (this);
  //Dequeue a packet from the queue to place it in the transmission queue. 
  return GetQueueDiscClass (0)->GetQueueDisc ()->Dequeue ();
}

bool 
ATSSchedulerQueueDisc::CheckConfig ()
{
  NS_LOG_FUNCTION (this);
  if (GetNPacketFilters () > 0)
    {
      NS_LOG_ERROR ("ATSSchedulerQueueDisc cannot have packet filters");
      return false;
    }

  if (GetNInternalQueues () > 0)
    {
      NS_LOG_ERROR ("ATSSchedulerQueueDisc cannot have internal queues");
      return false;
    }
  if (!m_group)
  {
    NS_LOG_ERROR ("ATSSchedulerQueueDisc need a group to enqueue in a qdisc");
    return false;
  }
  if (GetNQueueDiscClasses () == 0)
    {
      // create a FIFO queue disc
      NS_LOG_DEBUG ("Creating queue fifo disc class");
      ObjectFactory factory;
      factory.SetTypeId ("ns3::FifoQueueDisc");
      Ptr<QueueDisc> qd = factory.Create<QueueDisc> ();

      if (!qd->SetMaxSize (GetMaxSize ()))
        {
          NS_LOG_ERROR ("Cannot set the max size of the child queue disc to that of ATSSchedulerQueueDisc");
          return false;
        }
      qd->Initialize ();
      Ptr<QueueDiscClass> c = CreateObject<QueueDiscClass> ();
      c->SetQueueDisc (qd);
      AddQueueDiscClass (c);
    }
  if (m_group == nullptr)
  {
    NS_LOG_ERROR ("ATSScheduler needs a group");
    return false;
  }
  if (m_transmissionQueue == nullptr)
  {
    //ATSTransmissionQueue must be initialized first
    NS_LOG_ERROR ("ATSScheduler needs a transmission queue");
    return false;
  }
  
  return true;
}

void 
ATSSchedulerQueueDisc::InitializeParams ()
{
  NS_LOG_FUNCTION (this);
  //TODO: maybe initialize params here before first packet enqueue (ie, bucketEmptytime...)
  m_bucketEmptyTime = Simulator::Now ();
}

void
ATSSchedulerQueueDisc::ReadyForTransmission ()
{
  NS_LOG_FUNCTION (this);

  //Set Transmission Attribute of ATSTransmission queue and enqueue the packet(READY for transmission)
  //m_send callback of this QueueDisc has to be set up to point to the transmission 
  //queue before calling this method. This transmission queue is an internal queue of
  //ATSTranmissionQueue.
  
  Ptr<QueueDiscItem> item = DoDequeue ();
  m_transmissionQueue->SetATSToTransmission ();
  m_transmissionQueue->Enqueue (item);
}

void 
ATSSchedulerQueueDisc::SetTransmissionQueue (Ptr<ATSTransmissionQueueDisc> transmissionQueue)
{
  m_transmissionQueue = transmissionQueue;
}

}