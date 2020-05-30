 /* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

 #include "mac-queue-disc-item.h"
 #include "ns3/log.h"

namespace ns3
{
  NS_LOG_COMPONENT_DEFINE ("MacQueueDiscItem");

  MacQueueDiscItem::MacQueueDiscItem (Ptr<Packet> p, const Address& addr, uint16_t protocol)
    :QueueDiscItem (p, addr, protocol)
    {
      Ipv4Header ipHeader;
      p->PeekHeader (ipHeader);
      m_header = ipHeader;
    }
    MacQueueDiscItem::~MacQueueDiscItem ()
    {
    }
    void 
    MacQueueDiscItem::AddHeader (void)
    {
     /* Ptr<Packet> p = GetPacket ();
      *NS_ASSERT (p != 0);
      *p->AddHeader (m_header);
      */
    }
    void
    MacQueueDiscItem::SetSource (Mac48Address src)
    {
      m_source = src;
    }

    Mac48Address
    MacQueueDiscItem::GetSource ()
    {
      return m_source;
    }
    bool 
    MacQueueDiscItem::Mark(void)
    {
      return false;
    }
}
  