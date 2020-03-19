/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/test.h"
#include "ns3/localtime-simulator-impl.h"
#include "ns3/local-clock.h"
#include "ns3/perfect-clock-model-impl.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/core-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TestLocalSimulatorScheduling");




class EventSchedulTestCase : public TestCase
{
public:
  EventSchedulTestCase (std::string descr, ObjectFactory schedulerFactory);

  void EventA (Time localTime, Time globalTime);
  void EventB (Time localTime, Time globalTime);
  void EventC (Time localTime, Time globalTime, Time newTime);

  void CreateNode ();
  void ScheduleSend ();
  void ScheduleSend1 ();
  void ScheduleCheck (Time localTime, Time globalTime);
  void NewFrequency ();
  bool m_b;
  bool m_a;
  
  virtual ~EventSchedulTestCase ();
  virtual void DoRun (void);
  virtual void DoSetup ();
 
  Ptr<Node> m_node;
  Ptr<ClockModelImpl> m_clock;
  Ptr<LocalClock> clock0;
  ObjectFactory m_schedulerFactory;

};

/**
* This test aim to check that events are scheduled according to a global time shifted 
* from a local time. Also check that events are run. 
* This test work just with the perfect clock impl. 
* This test schedule 2 events, it checks that the first event is scheduled according to
* node clock freqeuncy. Second event is rescheduled due to a clock change. The test checks 
* that the event has been rescheduled in the proper time. 
*/

EventSchedulTestCase::EventSchedulTestCase (
  std::string descr,
  ObjectFactory schedulerFactory
)
: TestCase ("Check that basic event handling is working with " + 
              schedulerFactory.GetTypeId ().GetName ()),
    m_schedulerFactory (schedulerFactory)
{
}
EventSchedulTestCase::~EventSchedulTestCase ()
{
}

void 
EventSchedulTestCase::EventA (Time localTime, Time globalTime)
{
  m_a = true;
  std::cout << "Sim Time" << Simulator::Now () << std::endl;
}
void 
EventSchedulTestCase::EventB (Time localTime, Time globalTime)
{
  ScheduleCheck (localTime, globalTime);
  std::cout << "Executed B at simulator time" << Simulator::Now () << std::endl;
  m_b = true;
}
void
EventSchedulTestCase::EventC (Time localTime, Time globalTime, Time newTime)
{
  std::cout << "Event C Conte4xt" <<Simulator::GetContext () << std::endl;
  std::cout << "Expected at "  << globalTime << "/" << localTime << "(sim/node)" << std::endl;
  std::cout << "Event rescheduled executed at at simulator time" << Simulator::Now () << std::endl;
  NS_TEST_ASSERT_MSG_EQ (newTime, Simulator::Now (), "Wrong rescheduling time");
}
void 
EventSchedulTestCase::ScheduleSend ()
{
  std::cout << "Executed ScheduleSend at simulator time" << Simulator::Now () << std::endl;
  Simulator::Schedule (Seconds(2),&EventSchedulTestCase::EventB, this, Seconds (3), Seconds (6));
}
void 
EventSchedulTestCase::ScheduleSend1 ()
{
  EventId event = Simulator::Schedule (Seconds(2),&EventSchedulTestCase::EventC, this, Seconds (6), Seconds (12),
  Seconds (21));
  std::cout << "Event C scheduled at simulator time" << event.GetTs () << std::endl;
}
void
EventSchedulTestCase::CreateNode ()
{
  m_node = CreateObject<Node> ();
  clock0 = CreateObject<LocalClock> ();
  m_clock = CreateObject<PerfectClockModelImpl> ();
  m_clock -> SetAttribute ("Frequency", DoubleValue (2));
  clock0 -> SetAttribute ("ClockModelImpl", PointerValue (m_clock));
  m_node -> AggregateObject (clock0);
}
void 
EventSchedulTestCase::ScheduleCheck (Time localTime, Time globalTime)
{
  std::cout << " Expected at "  << globalTime << "/" << localTime << "(sim/node)" << std::endl;
  NS_TEST_ASSERT_MSG_EQ (localTime, m_clock -> GetLocalTime (), "Wrong local time");
  NS_TEST_ASSERT_MSG_EQ (globalTime, Simulator::Now (), "Wrong global time");
}
void
EventSchedulTestCase::NewFrequency ()
{
  std::cout << "Event New freq at " << Simulator::Now () << std::endl;
  Ptr<ClockModelImpl> newClock = CreateObject<PerfectClockModelImpl> ();
  newClock -> SetAttribute ("Frequency", DoubleValue (4));
  clock0 -> SetClock (newClock);
}
void
EventSchedulTestCase::DoSetup ()
{  
  GlobalValue::Bind ("SimulatorImplementationType", 
                     StringValue ("ns3::LocalTimeSimulatorImpl"));
  CreateNode ();
  uint32_t id = 0;
  m_a = false;
  m_b = false;

  Simulator::SetScheduler (m_schedulerFactory);
 //These events are scheduled without the clock freq

  Simulator::ScheduleWithContext (id, Seconds (1), &EventSchedulTestCase::EventA, this, Seconds (1), Seconds (2));
  Simulator::Schedule (Seconds (2), &EventSchedulTestCase::ScheduleSend, this);
  Simulator::Schedule (Seconds (8), &EventSchedulTestCase::ScheduleSend1, this);
  Simulator::Schedule (Seconds (9), &EventSchedulTestCase::NewFrequency, this);
}

void
EventSchedulTestCase::DoRun (void)
{
  Simulator::Run ();
  NS_TEST_EXPECT_MSG_EQ (m_a, true, "Event A did not run ?");
  NS_TEST_EXPECT_MSG_EQ (m_b, true, "Event B did not run ?");
  Simulator::Destroy ();
}

class LocalSimulatorTestSuite : public TestSuite
{
public:
  LocalSimulatorTestSuite ()
  :TestSuite ("node-scheduling", UNIT)
  {
    ObjectFactory factory;
    factory.SetTypeId (ListScheduler::GetTypeId ());

    AddTestCase (new EventSchedulTestCase ("Check basic event handling is working", factory), TestCase::QUICK);
  }
}g_localSimulatorTestSuite;


