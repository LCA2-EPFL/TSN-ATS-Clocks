/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ats-bridge-net-device.h"
#include "ns3/log.h"
#include "ns3/traffic-control-layer.h"
#include "mac-queue-disc-item.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ATSBridgeNetDevice");

NS_OBJECT_ENSURE_REGISTERED (ATSBridgeNetDevice);

TypeId
ATSBridgeNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ATSBridgeNetDevice")
    .SetParent<BridgeNetDevice> ()
    .SetGroupName("ATSBridge")
    .AddConstructor<ATSBridgeNetDevice> ()
    ;
    return tid;
}

ATSBridgeNetDevice::ATSBridgeNetDevice ()
:BridgeNetDevice ()
{
  NS_LOG_FUNCTION (this);
}

ATSBridgeNetDevice::~ATSBridgeNetDevice ()
{
  NS_LOG_FUNCTION (this);
}

void 
ATSBridgeNetDevice::ForwardUnicast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
uint16_t protocol, Mac48Address src, Mac48Address dst)
{
  NS_LOG_FUNCTION (this);

  Ptr<Node> node = GetNode ();
  Ptr<TrafficControlLayer> tc =  node->GetObject <TrafficControlLayer> ();
  Ptr<MacQueueDiscItem> item = Create<MacQueueDiscItem> (packet->Copy (), Address (dst), protocol);

  NS_LOG_DEBUG ("LearningBridgeForward (incomingPort=" << incomingPort->GetInstanceTypeId ().GetName ()
                                                       << ", packet=" << packet << ", protocol="<<protocol
                                                       << ", src=" << src << ", dst=" << dst << ")");
  BridgeNetDevice::Learn (src, incomingPort);
  Ptr<NetDevice> outPort = BridgeNetDevice::GetLearnedState (dst);
  if (outPort != NULL && outPort != incomingPort)
    {
      NS_LOG_LOGIC ("Learning bridge state says to use port `" << outPort->GetInstanceTypeId ().GetName () << "'");
      tc->Send (outPort, item);
    }
  else
  {
    NS_LOG_DEBUG ("Port : " << this->GetNBridgePorts ());
    NS_LOG_LOGIC ("No learned state: send through all ports");
    for (uint32_t i = 0; i < this->GetNBridgePorts (); i++)
    {
      Ptr<NetDevice> port = this->GetBridgePort (i);
      if (port != incomingPort)
          {
            NS_LOG_LOGIC ("LearningBridgeForward (" << src << " => " << dst << "): " 
                                                    << incomingPort->GetInstanceTypeId ().GetName ()
                                                    << " --> " << port->GetInstanceTypeId ().GetName ()
                                                    << " (UID " << packet->GetUid () << ").");
            tc->Send (port, item);
          }
    }
  }
}

void 
ATSBridgeNetDevice::ForwardBroadcast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                         uint16_t protocol, Mac48Address src, Mac48Address dst)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_DEBUG ("LearningBridgeForward (incomingPort=" << incomingPort->GetInstanceTypeId ().GetName ()
                                                       << ", packet=" << packet << ", protocol="<<protocol
                                                       << ", src=" << src << ", dst=" << dst << ")");
  Ptr<Node> node = this->GetNode ();
  Ptr<TrafficControlLayer> tc = node->GetObject <TrafficControlLayer> ();
  Ptr<MacQueueDiscItem> item = Create<MacQueueDiscItem> (packet->Copy (), Address (dst), protocol);

  BridgeNetDevice::Learn (src, incomingPort);

  for (uint32_t i = 0; i < this->GetNBridgePorts (); i++)
  {
    Ptr<NetDevice> port = this->GetBridgePort (i);
    if (port != incomingPort)
      {
        NS_LOG_LOGIC ("LearningBridgeForward (" << src << " => " << dst << "): " 
                                                << incomingPort->GetInstanceTypeId ().GetName ()
                                                << " --> " << port->GetInstanceTypeId ().GetName ()
                                                << " (UID " << packet->GetUid () << ").");
        tc->Send (port, item);
      }
  }
}

}

