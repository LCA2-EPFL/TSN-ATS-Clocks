 /* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

 #include "mac-queue-disc-item.h"
 #include "ns3/log.h"


namespace ns3
{
  NS_LOG_COMPONENT_DEFINE ("MacQueueDiscItem");

  MacQueueDiscItem::MacQueueDiscItem (Ptr<Packet> p, const Address& addr, uint16_t protocol)
    :QueueDiscItem (p, addr, protocol)
    {
    }
    MacQueueDiscItem::~MacQueueDiscItem ()
    {
    }
    void 
    MacQueueDiscItem::AddHeader (void)
    {
    }
    bool 
    MacQueueDiscItem::Mark(void)
    {
      return false;
    }
}
  