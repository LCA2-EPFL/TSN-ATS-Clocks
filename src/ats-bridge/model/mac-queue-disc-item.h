 /* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef MAC_QUEUE_DISC_ITEM_H
#define MAC_QUEUE_DISC_ITEM_H

#include "ns3/packet.h"
#include "ns3/queue-item.h"



namespace ns3 {

  class MacQueueDiscItem : public QueueDiscItem
  {
    public:
    MacQueueDiscItem (Ptr<Packet> p, const Address & addr, uint16_t protocol);
    ~MacQueueDiscItem ();
    virtual void AddHeader (void);
    virtual bool Mark(void);
 
  };        
}
#endif /**MAC_QUEUE_DISC_ITEM_H*/