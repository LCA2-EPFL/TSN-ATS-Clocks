/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
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
 *
 * Author: Guillermo Aguirre 
 */
#ifndef LOCALTIME_SIMULATOR_IMPL_H
#define LOCALTIME_SIMULATOR_IMPL_H

#include "ns3/default-simulator-impl.h"
#include "ns3/local-clock.h"



/**
 * \file
 * \ingroup simulator
 * ns3::LocalTimetSimulatorImpl declaration.
 */

namespace ns3{

/**
 *  \ingroup simulator
 * 
 * @brief Implementation of single process simulator based on node local time. 
 * This class translate the local delay (node time) to global delay (simulation time)
 * when function Schedule and SchedulewithContext are called.
 */

class LocalTimeSimulatorImpl : public SimulatorImpl
{
public:
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /** Contructor. */
  LocalTimeSimulatorImpl ();
  /** Destructor. */
  ~LocalTimeSimulatorImpl();

  // Inherited
    virtual void Destroy ();
    virtual bool IsFinished (void) const;
    virtual void Stop (void);
    virtual void Stop (const Time &delay);
    virtual void ScheduleWithContext (uint32_t context, const Time &delay, EventImpl *event);
    virtual EventId ScheduleNow (EventImpl *event);
    virtual EventId ScheduleDestroy (EventImpl *event);
    virtual void Remove (const EventId &id);
    virtual bool IsExpired (const EventId &id) const;
    virtual void Run (void);
    virtual Time Now (void) const;
    virtual Time GetDelayLeft (const EventId &id) const;
    virtual Time GetMaximumSimulationTime (void) const;
    virtual void SetScheduler (ObjectFactory schedulerFactory);
    virtual uint32_t GetSystemId (void) const;
    virtual uint32_t GetContext (void) const;
    virtual uint64_t GetEventCount (void) const;

    virtual void DoDispose (void);

    /**
     */
    void CancelRescheduling (const EventId &id);


  /**
   *  \brief This function provides a mechanism to translate the delay which is based on the node local time. 
   * The node implementation is obtained through the node list and the current context.
   * DefaultSimulatorImpl::Schedule(const Time &delay, EventImpl *event) is called to simulate 
   * the events in the main simulator.
   */
  virtual EventId Schedule (const Time &delay, EventImpl *event);
  /**
  * \brief Every time a there is a cancel call ensure if the cancel comes from a rescheduling event 
  * due to a clock change. If the cancel call is due to a rescehduling, we insert those events 
  * in m_eventCancelation but we dont set the flag m_cancel of eventImpl so we can reschedule afterards.
  */
  virtual void Cancel (const EventId &id);

private:

  /** \brief Process the next event. Check if the event to invoke is one of the events that is been 
   * canceled by the clock update function. We  don't invoke those events. This is done in order to maintain the event implementation. 
   */
  void ProcessOneEvent (void);
  /** Move events from a different context into the main event queue. */
  void ProcessEventsWithContext (void);
  /** Function that insert and event in the scheduler */
  Scheduler::Event InsertScheduler (EventImpl *impl, Time tAbsolute);
  /** Calculate absoulte time*/
  Time CalculateAbsoluteTime (Time delay);
 
  /** Wrap an event with its execution context. */
  struct EventWithContext {
    /** The event context. */
    uint32_t context;
    /** Event timestamp. */
    uint64_t timestamp;
    /** The event implementation. */
    EventImpl *event;
  };
  /** Container type for the events from a different context. */
  typedef std::list<struct EventWithContext> EventsWithContext;
  /** The container of events from a different context. */
  EventsWithContext m_eventsWithContext;
  /**
   * Flag \c true if all events with context have been moved to the
   * primary event queue.
   */
  bool m_eventsWithContextEmpty;
  /** Mutex to control access to the list of events with context. */
  SystemMutex m_eventsWithContextMutex;

  /** Container type for the events to run at Simulator::Destroy() */
  typedef std::list<EventId> DestroyEvents;
  /** The container of events to run at Destroy. */
  DestroyEvents m_destroyEvents;
  /** Flag calling for the end of the simulation. */
  bool m_stop;
  /** The event priority queue. */
  Ptr<Scheduler> m_events;

  /** Next event unique id. */
  uint32_t m_uid;
  /** Unique id of the current event. */
  uint32_t m_currentUid;
  /** Timestamp of the current event. */
  uint64_t m_currentTs;
  /** Execution context of the current event. */
  uint32_t m_currentContext;
  /** The event count. */
  uint64_t m_eventCount;
  /** List of events cancelled due to rescheduling */
  std::list<EventId> m_eventCancelation;

  /**
   * Number of events that have been inserted but not yet scheduled,
   *  not counting the Destroy events; this is used for validation
   */
  int m_unscheduledEvents;

  /** Main execution thread. */
  SystemThread::ThreadId m_main;
};

}// namespace ns3

#endif /* LOCALTIME_SIMULATOR_IMPL_H */

