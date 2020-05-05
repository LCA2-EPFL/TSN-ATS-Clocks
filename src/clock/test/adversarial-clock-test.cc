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
#include "ns3/adversarial-clock.h"

using namespace ns3;  

NS_LOG_COMPONENT_DEFINE ("TestAdversarialClock");

class AdversarialClockTest : public TestCase
{
public:
  AdversarialClockTest (std::string descr, ObjectFactory schedulerFactory);

  void EventA ();
  void EventB ();
  void EventC (Time localTime, Time globalTime, Time newTime);

  void CreateNode (Time delta, Time period, double slope, Time intercal, Time xvalue);
  void ScheduleSend (Time t);
  void ScheduleSend1 ();
  void ScheduleCheck (Time localTime, Time globalTime);
  bool m_b;
  bool m_a;
  
  virtual ~AdversarialClockTest ();
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

AdversarialClockTest::AdversarialClockTest (
  std::string descr,
  ObjectFactory schedulerFactory
)
: TestCase ("Check that basic event handling is working with " + 
              schedulerFactory.GetTypeId ().GetName ()),
    m_schedulerFactory (schedulerFactory)
{
}
AdversarialClockTest::~AdversarialClockTest ()
{
}

void 
AdversarialClockTest::EventA ()
{
  m_a = true;
  NS_LOG_DEBUG ("Event execute");
  std::cout << "Execution Time" << Simulator::Now () << std::endl;
}
void 
AdversarialClockTest::EventB ()
{
  NS_LOG_DEBUG ("Event execute");
  std::cout << "Execution Time  " << Simulator::Now () << std::endl;
}

void
AdversarialClockTest::EventC (Time localTime, Time globalTime, Time newTime)
{
  std::cout << "Event C Conte4xt" <<Simulator::GetContext () << std::endl;
  std::cout << "Expected at "  << globalTime << "/" << localTime << "(sim/node)" << std::endl;
  std::cout << "Event rescheduled executed at at simulator time" << Simulator::Now () << std::endl;
  NS_TEST_ASSERT_MSG_EQ (newTime, Simulator::Now (), "Wrong rescheduling time");
}
void 
AdversarialClockTest::ScheduleSend (Time t)
{
  std::cout << "Executed ScheduleSend at simulator time" << Simulator::Now () << std::endl;
  Simulator::Schedule (t,&AdversarialClockTest::EventB, this);
}
void 
AdversarialClockTest::ScheduleSend1 ()
{
  EventId event = Simulator::Schedule (Seconds(2),&AdversarialClockTest::EventC, this, Seconds (6), Seconds (12),
  Seconds (21));
  std::cout << "Event C scheduled at simulator time" << event.GetTs () << std::endl;
}
void
AdversarialClockTest::CreateNode (Time delta, Time period, double slope, Time interval, Time xvalue)
{
  m_node = CreateObject<Node> ();
  clock0 = CreateObject<LocalClock> ();
  m_clock = CreateObject<AdversarialClock> ();

  //Set Clock attributes
  m_clock -> SetAttribute ("Delta", TimeValue (delta));
  m_clock -> SetAttribute ("Period", TimeValue (period));
  m_clock -> SetAttribute ("Slope", DoubleValue (slope));
  m_clock -> SetAttribute ("Interval", TimeValue (interval));
  m_clock -> SetAttribute ("xvalue", TimeValue (xvalue));

  clock0 -> SetAttribute ("ClockModelImpl", PointerValue (m_clock));
  m_node -> AggregateObject (clock0);
}
void 
AdversarialClockTest::ScheduleCheck (Time localTime, Time globalTime)
{
  std::cout << " Expected at "  << globalTime << "/" << localTime << "(sim/node)" << std::endl;
  NS_TEST_ASSERT_MSG_EQ (localTime, m_clock -> GetLocalTime (), "Wrong local time");
  NS_TEST_ASSERT_MSG_EQ (globalTime, Simulator::Now (), "Wrong global time");
}
void
AdversarialClockTest::DoSetup ()
{  
  GlobalValue::Bind ("SimulatorImplementationType", 
                     StringValue ("ns3::LocalTimeSimulatorImpl"));

  Time::SetResolution (Time::FS);
  Time delta = MicroSeconds (1);
  Time period = MilliSeconds (30);
  double slope = 1.001;
  Time interval = MilliSeconds (10);  
  Time xvalue = MilliSeconds (5);

  AdversarialClockTest::CreateNode (delta, period, slope, interval, xvalue);

  uint32_t id = 0;
  m_a = false;
  m_b = false;

  Simulator::SetScheduler (m_schedulerFactory);

  Simulator::ScheduleWithContext (id, MilliSeconds (1), &AdversarialClockTest::EventA, this);

  Simulator::Schedule (period, &AdversarialClockTest::ScheduleSend, this, period);
  Simulator::Schedule (period + xvalue + delta/2, &AdversarialClockTest::ScheduleSend, this, period);
  Simulator::Schedule (period + xvalue + delta/2 + Time (interval.GetDouble () / slope), &AdversarialClockTest::ScheduleSend, this, period);
  Simulator::Schedule (period + xvalue + delta/2 + interval + Time (interval.GetDouble () / slope), &AdversarialClockTest::ScheduleSend, this, period);
}

void
AdversarialClockTest::DoRun (void)
{
  Simulator::Run ();
  NS_TEST_EXPECT_MSG_EQ (m_a, true, "Event A did not run ?");
  Simulator::Destroy ();
}

class AdversarialClockTestSuite : public TestSuite
{
public:
  AdversarialClockTestSuite ()
  :TestSuite ("adversarial-clock", UNIT)
  {
    ObjectFactory factory;
    factory.SetTypeId (ListScheduler::GetTypeId ());

    AddTestCase (new AdversarialClockTest ("Check basic event handling is working", factory), TestCase::QUICK);
  }
}g_AdversarialClockTestSuite;


