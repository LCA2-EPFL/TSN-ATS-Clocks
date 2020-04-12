#include "ats-queue-disc-filter.h"
#include "ns3/log.h"

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
  m_streamFilter[1].src = Ipv4Address ("10.1.1.2");
  m_streamFilter[1].dst = Ipv4Address ("10.1.1.4");
  m_streamFilter[1].macSrc = Mac48Address ("00:00:00:00:00:02");
  m_streamFilter[1].macDst = Mac48Address ("00:00:00:00:00:07");
  m_streamFilter[2].src = Ipv4Address ("10.1.1.3");
  m_streamFilter[2].dst = Ipv4Address ("10.1.1.4");
  m_streamFilter[2].macSrc = Mac48Address ("00:00:00:00:00:03");
  m_streamFilter[2].macDst = Mac48Address ("00:00:00:00:00:07");
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
 
  NS_LOG_DEBUG ("Packet Size " <<  ipHeader.GetPayloadSize () << "Packet Size " << p->GetSize () 
  << "Get protocol" << item->GetProtocol ());

  int32_t ret = -1;
  Ipv4Address src = ipHeader.GetSource ();
  Ipv4Address dest = ipHeader.GetDestination ();
  
  NS_LOG_DEBUG ("dst :" << dest << "src :" << src << " " << item->GetAddress ());
  

  for (auto const& pair : m_streamFilter)
  {
    if (pair.second.dst == dest && pair.second.src == src)
    {
      ret = pair.first;
      NS_LOG_DEBUG ("Filter Coincidence " << pair.second.dst << "and " << pair.second.src << 
      "Return value " << ret);
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