#include "ats-queue-disc-filter.h"
#include "ns3/log.h"
#include "ns3/udp-header.h"
#include "mac-queue-disc-item.h"

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
  m_streamFilter[0].dst = Ipv4Address ("10.1.1.4");
  m_streamFilter[0].macSrc = Mac48Address ("00:00:00:00:00:01");
  m_streamFilter[0].macDst = Mac48Address ("00:00:00:00:00:07");
  m_streamFilter[0].dstport = 9;
  m_streamFilter[1].src = Ipv4Address ("10.1.1.2");
  m_streamFilter[1].dst = Ipv4Address ("10.1.1.4");
  m_streamFilter[1].macSrc = Mac48Address ("00:00:00:00:00:03");
  m_streamFilter[1].macDst = Mac48Address ("00:00:00:00:00:07");
  m_streamFilter[1].dstport = 10;
  m_streamFilter[2].src = Ipv4Address ("10.1.1.3");
  m_streamFilter[2].dst = Ipv4Address ("10.1.1.4");
  m_streamFilter[2].macSrc = Mac48Address ("00:00:00:00:00:05");
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
  
  Ptr<MacQueueDiscItem> itemmac = DynamicCast <MacQueueDiscItem> (item);
  Ipv4Header ipHeader;
  Ptr<Packet> p = item->GetPacket ();
  p->PeekHeader (ipHeader);
  int32_t ret = -1;
  Mac48Address srcmac = itemmac-> GetSource ();
  NS_LOG_DEBUG ( "src :" << srcmac );
  
  
  for (auto const& pair : m_streamFilter)
  {
    if (pair.second.macSrc == srcmac )
    {
      ret = pair.first;
      
      NS_LOG_DEBUG ("Filter Coincidence " << pair.second.dst << " and " 
      << pair.second.src << " and port: " << pair.second.dstport << 
      " Return value " << ret);
    }
    
  }
  
  return ret;
}
bool 
ATSQueueDiscFilter::CheckProtocol (Ptr<QueueDiscItem> item) const
{
  return true;
}

}