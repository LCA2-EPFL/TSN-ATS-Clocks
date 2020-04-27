#include "ats-queue-disc-filter.h"
#include "ns3/log.h"
#include "ns3/udp-header.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("ATSQueueDiscFilter");
NS_OBJECT_ENSURE_REGISTERED (ATSQueueDiscFilter); 
TypeId 
ATSQueueDiscFilter::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ATSQueueDiscFilter")
    .SetParent<Ipv4PacketFilter> ()
    .SetGroupName("ATSBridge")
    .AddConstructor<ATSQueueDiscFilter> ()
    ;
  return tid;
}


ATSQueueDiscFilter::ATSQueueDiscFilter ()
{
  NS_LOG_FUNCTION (this);
  m_streamFilter[0].src = Ipv4Address ("10.1.1.1");
  m_streamFilter[0].dst = Ipv4Address ("10.1.1.2");
  m_streamFilter[0].macSrc = Mac48Address ("00:00:00:00:00:01");
  m_streamFilter[0].macDst = Mac48Address ("00:00:00:00:00:07");
  m_streamFilter[0].dstport = 9;
  m_streamFilter[1].src = Ipv4Address ("10.1.1.1");
  m_streamFilter[1].dst = Ipv4Address ("10.1.1.2");
  m_streamFilter[1].macSrc = Mac48Address ("00:00:00:00:00:01");
  m_streamFilter[1].macDst = Mac48Address ("00:00:00:00:00:07");
  m_streamFilter[1].dstport = 10;
  m_streamFilter[2].src = Ipv4Address ("10.1.1.1");
  m_streamFilter[2].dst = Ipv4Address ("10.1.1.2");
  m_streamFilter[2].macSrc = Mac48Address ("00:00:00:00:00:01");
  m_streamFilter[2].macDst = Mac48Address ("00:00:00:00:00:07");
  m_streamFilter[2].dstport = 11;
  
}
ATSQueueDiscFilter::~ATSQueueDiscFilter ()
{
  NS_LOG_FUNCTION (this);
}
int32_t
ATSQueueDiscFilter::DoClassify (Ptr<QueueDiscItem> item) const
{
  NS_LOG_DEBUG (this << item);
  
  Ptr<Packet> p = item->GetPacket ();
  
  Ipv4Header ipHeader;

  p->RemoveHeader (ipHeader);
  Ptr<Packet> pkt = p->Copy ();
  UdpHeader udpHdr;
  p->PeekHeader (udpHdr);
  
  NS_LOG_DEBUG ("PORT ----- " << udpHdr.GetDestinationPort ());

  
 
  NS_LOG_DEBUG ("Packet Size " <<  ipHeader.GetPayloadSize () << "Packet Size " << p->GetSize () 
  << "Get protocol" << item->GetProtocol ());

  int32_t ret = -1;
  Ipv4Address src = ipHeader.GetSource ();
  Ipv4Address dest = ipHeader.GetDestination ();
  uint8_t prot = ipHeader.GetProtocol ();
  
  NS_LOG_DEBUG ("dst :" << dest << "src :" << src << " " << item->GetAddress () << "protocol: " << prot);
  
  
  for (auto const& pair : m_streamFilter)
  {
    if (pair.second.dst == dest && pair.second.src == src && pair.second.dstport == udpHdr.GetDestinationPort ())
    {
      ret = pair.first;
      
      NS_LOG_DEBUG ("Filter Coincidence " << pair.second.dst << "and " 
      << pair.second.src << "and port: " << pair.second.dstport << 
      "Return value " << ret << "Protocol " << prot);
    }
  }
  
  
  p->AddHeader (ipHeader);
  return ret;
}
bool 
ATSQueueDiscFilter::CheckProtocol (Ptr<QueueDiscItem> item) const
{
  return true;
}

}