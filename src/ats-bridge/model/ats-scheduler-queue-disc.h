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

#ifndef ATSSCHEDULERQUEUEDISC_H
#define ATSSCHEDULERQUEUEDISC_H

#include "ns3/queue-disc.h"
#include "ns3/data-rate.h"
#include "ats-scheduler-group.h"
#include "ns3/callback.h"

namespace ns3 {

  /**
   * \ingroup ats-bridge 
   * \brief 
   * 
   */
  class ATSSchedulerQueueDisc : public QueueDisc
  {
    public:
    
      /**
       * Callback to the enqueue method. In a first implementation, the this callback is set to
       * the a method of ATSTrnasmissionQueue disc. However, the enqueue method of ATSScheduler
       * is not specified in the standar 802.1Q, so this callback can be change. 
       */
      typedef Callback<bool,Ptr<QueueDiscItem>> EnqueueCallBack;
      /**
       * \brief Get the type ID.
       * \return the object TypeId
       */
      static TypeId GetTypeId (void);

      /**
       * \brief Constructor
       */
      ATSSchedulerQueueDisc ();

      /**
       * \brief Destructor
       */
      virtual ~ATSSchedulerQueueDisc ();

       /**
       * \brief Set committed information rate for this ATSScheduler
       * 
       * \param committedRate 
       */ 
      void SetInformationRate (DataRate committedRate);

      /**
       * \brief Get committed information rate for this ATSScheduler
       * 
       * \return Data rate of ATSScheduler
       */
      DataRate GetInformationRate ();

      /**
       * \brief Set committed burst size for this ATSScheduler
       * 
       * \param burstSize 
       */
      void SetBurstSize (uint32_t burstSize);

      /**
       * \brief Get committed burst size for this ATSScheduler
       * 
       * \param committedSize 
       */
      uint32_t GetBurstSize ();

      /**
       * \brief Get ATSScheduler clockOffset max
       * 
       * \return offset
       */
      Time GetClockOffsetVaritionMax ();

      /**
       * \brief Get ATSScheduler clock rate deviation max
       * 
       * \return rate deviation
       */
      Time GetClockRateDeviationMax ();

      /**
       * \brief Get ATSScheduler arrival recognition delay max
       * 
       * \return Arrival Recognition Delay
       */
      Time GetArrivalRecognitionDelayMax ();

      /**
       * \brief Get ATSScheduler processing delay max
       * 
       * \return processing delay Max
       */
      Time GetProcessingDelayMax ();

      /**
       * \brief Get ATSScheduler processing delay min
       * 
       * \return processing delay Min
       */
      Time GetProcessingDelayMin ();


      /**
       * \brief Set ATSScheduler clockOffset max
       * 
       * \param offset 
       */
      void SetClockOffsetVaritionMax (Time offset);

      /**
       * \brief Set ATSScheduler clock rate deviation max
       * 
       * \param rateDeviation
       */
      void SetClockRateDeviationMax (Time rateDeviation);

      /**
       * \brief Set ATSScheduler arrival recognition delay max
       * 
       * \param arrivalDeay
       */
      void SetArrivalRecognitionDelayMax ( Time arrivalDeay);

      /**
       * \brief Set ATSScheduler processing delay max
       * 
       * \param processDelaymax
       */
      void SetProcessingDelayMax (Time processDelaymax);

      /**
       * \brief Set ATSScheduler processing delay min
       * 
       * \param processDelaymin
       */
      void SetProcessingDelayMin (Time processDelaymin);

       /**
       * \brief Set callback to enqueue the packet ready for transmission
       * 
       * \param item
       */
      void SetEnqueueCallBack (EnqueueCallBack ec);
      
      /**
       * \brief Set the pointer to ATSSchedulerGroup
       * 
       * \param group
       */
      void SetATSGroup (Ptr<ATSSchedulerGroup> group);
      

    private:

      virtual bool DoEnqueue (Ptr<QueueDiscItem> item);
      virtual Ptr<QueueDiscItem> DoDequeue (void);
      virtual bool CheckConfig (void);
      virtual void InitializeParams (void); 
      void AssingAndProceed(Time eligibilityTime, Ptr<QueueDiscItem> item);
      void EnqueueInTransmission(Ptr<QueueDiscItem> item);
      
      // parameters of ATS Scheduler queue disc leaf

      DataRate m_informationRate;
      uint32_t m_burstSize;

      //Scheduler timing characteristic for time calculations
      Time m_clockOffsetVariationMax;
      Time m_clockRateDeviationMax;
      Time m_arrivalRecognitionDelayMax;
      Time m_processingDelayMax;
      Time m_processingDelayMin;

      //Parameters for Tocken Bucket State machine 
      Time m_bucketEmptyTime;
      //Group to which this scheduler belongs 
      uint32_t m_SchedulerGroupId;
 
      EnqueueCallBack m_enqueueCallBack;
      Ptr<ATSSchedulerGroup> m_group;
  }; 

} //namespace ns3

#endif /* ATSSCHEDULERQUEUEDISC_H */