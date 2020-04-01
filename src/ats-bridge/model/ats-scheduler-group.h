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

#ifndef ATSSCHEDULERGROUP_H
#define ATSSCHEDULERGROUP_H

#include "ns3/object.h"
#include "ns3/nstime.h"

namespace ns3 {

   /**
   * \ingroup ats-bridge 
   * \brief This class representance a instance of ATS Scheduler Group Table. It contains
   * the parameter used by all the ATS schedulers that share the same group. It's used by 
   * ATSScheudlerQueueDisc to get MaxResidence Time and get/set GroupEligibilityTime. This object
   * has to be aggregated to all ATSSchedulerQueueDisc instances before initializing the queue.  
   */

  class ATSSchedulerGroup : public Object
  {
    public:
    /**
       * \brief Get the type ID.
       * \return the object TypeId
       */
      static TypeId GetTypeId (void);

    ATSSchedulerGroup ();
    ~ATSSchedulerGroup ();

    void InsertNewGroup (Time maxResidenceTime, Time groupEligibilityTime);
    Time GetMaxResidenceTime (uint32_t id);
    Time GetGroupElibilityTime (uint32_t id);
    void SetGroupElibilityTime (uint32_t id, Time groupEligibilityTime);
    void SetMaxResidenceTime (uint32_t id, Time residenceTime);
 
    private:
    
    // Parameters of a specific group 
    struct Parameter_T
    {
      Time maxResidenceTime;
      Time groupEligibilityTime;
    }m_params_t;
    typedef std::pair<uint32_t,Parameter_T> m_GroupInfoPair;
    std::map <uint32_t,Parameter_T> m_groupTable;
    uint32_t m_NGroup;

  };


}//namespace ns3
#endif /*ATSSCHEDULERGROUP_H*/
