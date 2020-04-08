/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef ATS_BRIDGE_NET_DEVICE_H
#define ATS_BRIDGE_NET_DEVICE_H

#include "ns3/bridge-net-device.h"

namespace ns3 {

class Node;

class ATSBridgeNetDevice : public BridgeNetDevice
{
  public:
  static TypeId GetTypeId (void);
  ATSBridgeNetDevice ();
  virtual ~ATSBridgeNetDevice ();

  protected:
  virtual void ForwardUnicast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                       uint16_t protocol, Mac48Address src, Mac48Address dst);

  virtual void ForwardBroadcast (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet,
                         uint16_t protocol, Mac48Address src, Mac48Address dst);

};
}

#endif /* ATS_BRIDGE_NET_DEVICE_H */

