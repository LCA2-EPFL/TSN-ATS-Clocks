/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ats-bridge-helper.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/names.h"
#include "ns3/ats-bridge-net-device.h"
#include "ns3/traffic-control-layer.h"
#include "ns3/config.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ATSBridgeHelper");

ATSBridgeHelper::ATSBridgeHelper ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_deviceFactory.SetTypeId ("ns3::BridgeNetDevice");
}

void 
ATSBridgeHelper::SetDeviceAttribute (std::string n1, const AttributeValue &v1)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_deviceFactory.Set (n1, v1);
}

NetDeviceContainer
ATSBridgeHelper::Install (Ptr<Node> node, NetDeviceContainer c)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_LOGIC ("**** Install bridge device on node " << node->GetId ());

  NetDeviceContainer devs;
  Ptr<ATSBridgeNetDevice> dev = CreateObject<ATSBridgeNetDevice> ();
  devs.Add (dev);
  node->AddDevice (dev);
  

  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      NS_LOG_LOGIC ("**** Add BridgePort "<< *i);
      dev->AddBridgePort (*i);
    }
  
  Ptr<TrafficControlLayer> tc = CreateObject <TrafficControlLayer> ();
  node->AggregateObject (tc);

  return devs;
}

NetDeviceContainer
ATSBridgeHelper::Install (std::string nodeName, NetDeviceContainer c)
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return Install (node, c);
}



}

