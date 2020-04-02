/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Natale Patriciello <natale.patriciello@gmail.com>
 *               2016 Stefano Avallone <stavallo@unina.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef ATSTRANSMISSIONQUEUEDISC_H
#define ATSTRANSMISSIONQUEUEDISC_H

#include "ns3/queue-disc.h"

namespace ns3 {

  /**
   * \ingroup ats-bridge 
   * \brief 
   * 
   */
  class ATSTransmissionQueueDisc : public QueueDisc
  {
    public:
    
      /**
       * \brief Get the type ID.
       * \return the object TypeId
       */
      static TypeId GetTypeId (void);

      /**
       * \brief Constructor
       */
      ATSTransmissionQueueDisc ();

      /**
       * \brief Destructor
       */
      virtual ~ATSTransmissionQueueDisc ();

       /**
       * \brief Function to enqueue in the internal FIFO queue, call by the ATSScheduler when 
       * eligibility time has elapsed. 
       */
      bool ATSEnqueue (Ptr<QueueDiscItem> item);
    

    private:

      virtual bool DoEnqueue (Ptr<QueueDiscItem> item);
      virtual Ptr<QueueDiscItem> DoDequeue (void);
      virtual bool CheckConfig (void);
      virtual void InitializeParams (void); 
      

      
      
  }; 
} //namespace ns3

#endif /* ATSTRANSMISSIONQUEUEDISC_H */