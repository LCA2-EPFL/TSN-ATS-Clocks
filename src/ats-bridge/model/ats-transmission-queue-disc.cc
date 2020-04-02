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
                   "The maximum number of packets accepted by FIFO internal queue disc",
                   QueueSizeValue (QueueSize ("100p")),
                   MakeQueueSizeAccessor (&QueueDisc::SetMaxSize,
                                          &QueueDisc::GetMaxSize),
                   MakeQueueSizeChecker ())
    ;

  return tid;
}
  
  ATSTransmissionQueueDisc::ATSTransmissionQueueDisc ()
  : QueueDisc (QueueDiscSizePolicy::NO_LIMITS)
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

    // Classify with an stream filter classifier
    int32_t ret = Classify (item);

    if (ret==PacketFilter::PF_NO_MATCH)
    {
      //Unable to classify the packet.
      //TODO: drop packet
      return false;
    }
    else
    {
      NS_LOG_DEBUG ("Packet filters returned " << ret);

      NS_ASSERT_MSG (ret < GetNQueueDiscClasses (), "Selected band out of range");

      bool retval = GetQueueDiscClass (ret)->GetQueueDisc ()->Enqueue (item);

      return retval;
    }
  }

  Ptr<QueueDiscItem>
  ATSTransmissionQueueDisc::DoDequeue ()
  {
    NS_LOG_FUNCTION (this);

    Ptr<QueueDiscItem> item = GetInternalQueue (0)->Dequeue ();

    NS_LOG_LOGIC ("Popped " << item);

    NS_LOG_LOGIC ("Number packets " << GetInternalQueue (0)->GetNPackets ());

    return item;
  }
  
  bool
  ATSTransmissionQueueDisc::CheckConfig ()
  {
    if (GetNInternalQueues () == 0)
    {
      // add a DropTail queue
      AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                          ("MaxSize", QueueSizeValue (GetMaxSize ())));
    }
    if (GetNInternalQueues () != 1)
      {
        NS_LOG_ERROR ("ATSTransmissionQueueDisc needs 1 internal queue");
        return false;
      }
    return true;
  }
  void 
  ATSTransmissionQueueDisc::InitializeParams ()
  {
    NS_LOG_FUNCTION (this);
    //TODO: maybe initialize params here before first packet enqueue
  }
  
  bool 
  ATSTransmissionQueueDisc::ATSEnqueue (Ptr<QueueDiscItem> item)
  {
    NS_LOG_FUNCTION (this << item);
    return GetInternalQueue (0)->Enqueue (item);
  }
}