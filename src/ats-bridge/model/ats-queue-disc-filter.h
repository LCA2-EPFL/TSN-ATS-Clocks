#ifndef ATSQUEUEDISCFILTER_H
#define ATSQUEUEDISCFILTER_H

#include "ns3/object.h"
#include "ns3/nstime.h"
#include <map>
#include "ns3/ipv4-queue-disc-item.h"
#include "ns3/ipv4-packet-filter.h"

namespace ns3{

  class ATSQueueDiscFilter : public Ipv4PacketFilter
  {
  public:
    static TypeId GetTypeId (void);
    ATSQueueDiscFilter ();
    virtual ~ATSQueueDiscFilter ();
  private:
    virtual int32_t DoClassify (Ptr<QueueDiscItem> item) const;
    virtual bool CheckProtocol (Ptr<QueueDiscItem> item) const;
    struct  tuple
    {
      Ipv4Address src;
      Ipv4Address dst;
      Mac48Address macSrc;
      Mac48Address macDst;
      uint16_t dstport;
    };
    std::map<int32_t, tuple> m_streamFilter;
  };
}

#endif