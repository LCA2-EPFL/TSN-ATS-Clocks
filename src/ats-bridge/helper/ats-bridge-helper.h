/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef ATS_BRIDGE_HELPER_H
#define ATS_BRIDGE_HELPER_H

#include "ns3/ats-bridge-net-device.h"
#include "ns3/net-device-container.h"
#include "ns3/object-factory.h"
#include <string>

namespace ns3 {
class Node;
class AttributeValue;
/* ... */
class ATSBridgeHelper
{


  public:
  /*
   * Construct a ATSBridgeHelper
   */
  ATSBridgeHelper ();
  /**
   * Set an attribute on each ns3::ATSBridgeHelper created by
   * BridgeHelper::Install
   *
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   */
  void SetDeviceAttribute (std::string n1, const AttributeValue &v1);
  /**
   * This method creates an ns3::ATSBridgeHelper with the attributes
   * configured by ATSBridgeHelper::SetDeviceAttribute, adds the device
   * to the node, and attaches the given NetDevices as ports of the
   * bridge.
   *
   * \param node The node to install the device in
   * \param c Container of NetDevices to add as bridge ports
   * \returns A container holding the added net device.
   */
  NetDeviceContainer Install (Ptr<Node> node, NetDeviceContainer c);
  /**
   * This method creates an ns3::ATSBridgeHelper with the attributes
   * configured by BridgeHelper::SetDeviceAttribute, adds the device
   * to the node, and attaches the given NetDevices as ports of the
   * bridge.
   *
   * \param nodeName The name of the node to install the device in
   * \param c Container of NetDevices to add as bridge ports
   * \returns A container holding the added net device.
   */
  NetDeviceContainer Install (std::string nodeName, NetDeviceContainer c);
private:
  ObjectFactory m_deviceFactory; //!< Object factory
  ObjectFactory m_tcFactory;
};

}

#endif /* ATS_BRIDGE_HELPER_H */

