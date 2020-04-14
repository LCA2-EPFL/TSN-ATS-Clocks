/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Universita' degli Studi di Napoli Federico II
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
 * Authors:  Stefano Avallone <stavallo@unina.it>
 */
#include "ats-transmission-queue-disc.h"
#include "ns3/log.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/simulator.h"
#include "ats-scheduler-queue-disc.h"


namespace ns3{

  NS_LOG_COMPONENT_DEFINE ("ATSTransmissionQueueDisc");
  NS_OBJECT_ENSURE_REGISTERED (ATSTransmissionQueueDisc); 

  TypeId ATSTransmissionQueueDisc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ATSTransmissionQueueDisc")
    .SetParent<QueueDisc> ()
    .SetGroupName("ATSBridge")
    .AddConstructor<ATSTransmissionQueueDisc> ()
    .AddAttribute ("MaxSize",
                   "The max queue size",
                   QueueSizeValue (QueueSize ("100p")),
                   MakeQueueSizeAccessor (&QueueDisc::SetMaxSize,
                                          &QueueDisc::GetMaxSize),
                   MakeQueueSizeChecker ())
    .AddTraceSource ("EnqueueTxTime",
                     "Time of the last enqueue packet in the Tx queue",
                     MakeTraceSourceAccessor (&ATSTransmissionQueueDisc::m_transmissionEnqueueTime),
                     "ns3::Time::TracedCallback")
    ;
    
  return tid;
}
  
  ATSTransmissionQueueDisc::ATSTransmissionQueueDisc ()
  : QueueDisc (QueueDiscSizePolicy::SINGLE_INTERNAL_QUEUE)
  {
    NS_LOG_FUNCTION (this);
  }

  ATSTransmissionQueueDisc::~ATSTransmissionQueueDisc ()
  {
    NS_LOG_FUNCTION (this);
  }

  bool
  ATSTransmissionQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item)
  {
    NS_LOG_FUNCTION (this << item);
    bool retval = false;
   
    if(m_state == TransmissionQueueState::STREAM_FILTERING)
    {
      if (GetNPacketFilters () == 0)
      {
        NS_LOG_LOGIC ("No packet filter");
        retval = GetQueueDiscClass (0)->GetQueueDisc ()->Enqueue (item);
      }
      else
      {
        NS_LOG_DEBUG ("Strem filtering state");
        // Classify with an stream filter classifier
        int32_t ret = Classify (item);
        NS_LOG_DEBUG ("Classifier return value: " << ret);

        if (ret==PacketFilter::PF_NO_MATCH)
        {
          //Unable to classify the packet.
          DropBeforeEnqueue (item, "Not able to classify");
          return false;
        }
        else
        {
          retval = GetQueueDiscClass (ret)->GetQueueDisc ()->Enqueue (item);
        }
      }
    }
      
    if (m_state ==TransmissionQueueState::READY_FOR_TRANSMISSION)  
    {
      NS_LOG_DEBUG ("Ready for transmission, enqueuing in internal queue");
      NS_LOG_DEBUG ("QUEUE tam: " << GetInternalQueue (0)->GetMaxSize ());
      retval = GetInternalQueue (0)->Enqueue (item);
      m_transmissionEnqueueTime (Simulator::Now ());
      if (!retval)
      {
        NS_LOG_WARN ("Packet enqueue failed. Check the size of the internal queues");
      }
      m_state = TransmissionQueueState::STREAM_FILTERING;
    }

    return retval;
    
  }

  Ptr<QueueDiscItem>
  ATSTransmissionQueueDisc::DoDequeue ()
  {
    NS_LOG_FUNCTION (this);

    Ptr<QueueDiscItem> item;
    item = GetInternalQueue (0)->Dequeue ();
    return item;
  }
  
  bool
  ATSTransmissionQueueDisc::CheckConfig ()
  {
    
    if (GetNQueueDiscClasses () < 1)
    {
      NS_LOG_ERROR ("ATSTransmissionQueue needs at least 1 class");
      return false;
    }
    else
    {
      // We set the transmission queue of ATSScheduler
      for (uint32_t i = 0; i < GetNQueueDiscClasses (); i++)
      {
        Ptr<QueueDisc> disc = GetQueueDiscClass (i)->GetQueueDisc ();
        Ptr <ATSSchedulerQueueDisc> scheduler = DynamicCast<ATSSchedulerQueueDisc> (disc) ;

        if (scheduler == nullptr)
        {
          NS_ASSERT ("ATStransmission needs a ATSScheduler");
          return false;
        }
        scheduler->SetTransmissionQueue (this);
        scheduler->SetSendCallback ([this] (Ptr<QueueDiscItem> item)
                          {this->Enqueue (item);});
      }
      
    }
  
    if (GetNInternalQueues () == 0)
    {
      // add a DropTail queue
      AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                          ("MaxSize", QueueSizeValue (GetMaxSize ())));
    }
    return true;
  }
  void 
  ATSTransmissionQueueDisc::InitializeParams ()
  {
    NS_LOG_FUNCTION (this);
    m_state = TransmissionQueueState::STREAM_FILTERING;
  }

  void 
  ATSTransmissionQueueDisc::SetATSToTransmission ()
  {
    NS_LOG_FUNCTION (this);
    m_state = TransmissionQueueState::READY_FOR_TRANSMISSION;
  }
}