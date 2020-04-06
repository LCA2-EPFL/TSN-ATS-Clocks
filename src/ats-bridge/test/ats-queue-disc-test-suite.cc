/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

// Include a header file from your module to test.

// An essential include is test.h
#include "ns3/test.h"
#include "ns3/ats-scheduler-group.h"
#include "ns3/ats-scheduler-queue-disc.h"
#include "ns3/ats-transmission-queue-disc.h"
#include "ns3/packet.h"
#include "ns3/queue-disc.h"
#include "ns3/simulator.h"
#include "ns3/config.h"
#include "ns3/traffic-control-layer.h"
#include "ns3/simple-net-device.h"
#include "ns3/node-container.h"
#include "ns3/simple-channel.h"


using namespace ns3;
/**
 * \ingroup ats-bridge
 * \ingroup tests
 *
 * \brief ATS Queue Disc Test Queue Item
 */
class ATSQueueDiscTestItem : public QueueDiscItem
{
  public:
    /**
     * Contructor
     * 
     * \param p packet
     * \param addr address
     * \param time time
     */
    ATSQueueDiscTestItem (Ptr<Packet> p, const Address & addr, Time time);
  virtual ~ATSQueueDiscTestItem ();
  virtual void AddHeader (void);
  virtual bool Mark(void);

private:
  ATSQueueDiscTestItem ();
  /**
   * \brief Copy constructor
   * Disable default implementation to avoid misuse
   */
  ATSQueueDiscTestItem (const ATSQueueDiscTestItem &);
  /**
   * \brief Assignment operator
   * \return this object
   * Disable default implementation to avoid misuse
   */
  ATSQueueDiscTestItem &operator = (const ATSQueueDiscTestItem &);
  Time m_EnqueueTime; // Enqueu time
};

ATSQueueDiscTestItem::ATSQueueDiscTestItem (Ptr<Packet> p, const Address & addr, Time time)
  : QueueDiscItem (p, addr, 0),
    m_EnqueueTime (time)
{
}

ATSQueueDiscTestItem::~ATSQueueDiscTestItem ()
{
}

void
ATSQueueDiscTestItem::AddHeader (void)
{
}

bool
ATSQueueDiscTestItem::Mark (void)
{
  return false;
}

/**
 * \ingroup ats-bridge
 * \ingroup tests
 *
 * \brief ATS Queue Disc Test Packet Filter
 */
class ATSQueueDiscTestFilter : public PacketFilter
{
public:
  /**
   * Constructor
   *
   * \param cls whether this filter is able to classify a PrioQueueDiscTestItem
   */
  ATSQueueDiscTestFilter (bool cls);
  virtual ~ATSQueueDiscTestFilter ();
  /**
   * \brief Set the value returned by DoClassify
   *
   * \param ret the value that DoClassify returns
   */
  void SetReturnValue (int32_t ret);

private:
  virtual bool CheckProtocol (Ptr<QueueDiscItem> item) const;
  virtual int32_t DoClassify (Ptr<QueueDiscItem> item) const;

  bool m_cls;     //!< whether this filter is able to classify a PrioQueueDiscTestItem
  int32_t m_ret;  //!< the value that DoClassify returns if m_cls is true
};

ATSQueueDiscTestFilter::ATSQueueDiscTestFilter (bool cls)
  : m_cls (cls),
    m_ret (0)
{
}

ATSQueueDiscTestFilter::~ATSQueueDiscTestFilter ()
{
}

void
ATSQueueDiscTestFilter::SetReturnValue (int32_t ret)
{
  m_ret = ret;
}

bool
ATSQueueDiscTestFilter::CheckProtocol (Ptr<QueueDiscItem> item) const
{
  return m_cls;
}

int32_t
ATSQueueDiscTestFilter::DoClassify (Ptr<QueueDiscItem> item) const
{
  return m_ret;
}


class AtsQueueDiscTestCase : public TestCase
{
public:
  AtsQueueDiscTestCase ();
  void CheckDequeue (Ptr<QueueDisc> leaf, uint32_t numP, std::string printStatement);
  void Enqueue (Address dst, uint32_t pktSize, Ptr<QueueDisc> queue);
  virtual void DoRun (void);
};

AtsQueueDiscTestCase::AtsQueueDiscTestCase ()
  : TestCase ("AtsBridge test case (does nothing)")
{
}
void
AtsQueueDiscTestCase::Enqueue (Address dst, uint32_t pktSize, Ptr<QueueDisc> queue)
{
  queue->Enqueue (Create<ATSQueueDiscTestItem> (Create<Packet> (pktSize),dst,Simulator::Now ()));
}
void
AtsQueueDiscTestCase::CheckDequeue (Ptr<QueueDisc> ats, uint32_t numP, std::string printStatement )
{
  NS_TEST_ASSERT_MSG_EQ (ats->GetCurrentSize ().GetValue (), numP, printStatement);
}
void
AtsQueueDiscTestCase::DoRun (void)
{
  /**Test 1: Create objects and Set attributes of ATSSchedulerQueueq*/     

  //Scheduler Parameters (For tocket bucket shaping in ATSScheduler)
  Ptr<ATSTransmissionQueueDisc> ATSroot;
  Ptr<ATSQueueDiscTestFilter> filter;
  Ptr<ATSSchedulerGroup> group;

  uint32_t burst = 1000; //bytes
  DataRate rate = DataRate ("100KB/s");
  Time clockOffsetVariation = Seconds (0);
  Time clockRateDeviationMax = Seconds (0);
  Time arrivalRecognitionDelayMax = Seconds (0);
  Time processDelayMin = Seconds (0);
  Time processDelayMax = Seconds (0);
  uint32_t pktSize = 1000; //bytes
  Address dst;
  uint32_t maxSize = 20;


  Ptr<ATSSchedulerQueueDisc> leaf = CreateObject<ATSSchedulerQueueDisc> ();

  NS_TEST_ASSERT_MSG_EQ (leaf->SetAttributeFailSafe ("Burst",UintegerValue (burst)),true,
                        "Verify that we can set the attribute Burst");
  NS_TEST_ASSERT_MSG_EQ (leaf->SetAttributeFailSafe ("Rate",DataRateValue (rate)),true,
                        "Verify that we can set the attribute Rate");
  NS_TEST_ASSERT_MSG_EQ (leaf->SetAttributeFailSafe ("ClockOffsetVariationMax",TimeValue (clockOffsetVariation)),true,
                        "Verify that we can set the attribute ClockOffsetVariationMax");
  NS_TEST_ASSERT_MSG_EQ (leaf->SetAttributeFailSafe ("ClockRateDeviationMax",TimeValue (clockRateDeviationMax)),true,
                        "Verify that we can set the attribute ClockRateDeviationMax");  
  NS_TEST_ASSERT_MSG_EQ (leaf->SetAttributeFailSafe ("ArrivalRecognitionDelayMax",TimeValue (arrivalRecognitionDelayMax)),true,
                        "Verify that we can set the attribute ArrivalRecognitionDelayMax");
  NS_TEST_ASSERT_MSG_EQ (leaf->SetAttributeFailSafe ("ProcessDelayMin",TimeValue (processDelayMin)),true,
                        "Verify that we can set the attribute ProcessDelayMin");
  NS_TEST_ASSERT_MSG_EQ (leaf->SetAttributeFailSafe ("ProcessDelayMax",TimeValue (processDelayMax)),true,
                        "Verify that we can set the attribute ProcessDelayMax"); 
  NS_TEST_ASSERT_MSG_EQ (leaf->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (QueueSizeUnit::PACKETS, maxSize))), true,
                         "Verify that we can set the attribute maxSize");
  
  
  /**Test 2: Create the following structure and tets simple enqueue/dequeue*/
  /**
  (1:1)
   * ATSTransmissionQueueDisc (Root) (Classifier + FIFO queue with packets ready for transmission)
   *            |
   *            |
   *            |
   *  ATSSchedulerQueueDisc (Leaf)(with FIFO)
   *  
   * This test creates a basic structure with one ATSTransmissionQueueDisc and one ATSSchedulerQueue. 
   * In this case ATStransmissionQueueDisc implements a packet filter which i not a StreamFilter as in
   * 801.1Q standard. 
   * 1 packet is enqueued and dequeued at eligibilityTime. 
   * 
   * 1. Just one ATSSchedulerQueueDisc per ATAtransmissionQueueDisc
   * 2. Enqueue of five packets each of 1000B each followed by the dequeue at eligibilityTime from 
   * ATSSchedulerQueueDisc to ATSTransmission internal queue. 
   * 3- Verify when packets are enqueue in the transmission queue once eligibility time has elapsed.
   * 
   */

  ATSroot = CreateObject<ATSTransmissionQueueDisc> ();  
  Ptr<QueueDiscClass> c = CreateObject<QueueDiscClass> ();
  filter = CreateObject<ATSQueueDiscTestFilter> (true);
  c->SetQueueDisc (leaf);
  ATSroot->AddQueueDiscClass (c);
  ATSroot->AddPacketFilter (filter);
  filter->SetReturnValue (0);

  NS_TEST_ASSERT_MSG_EQ (ATSroot->GetNQueueDiscClasses (),1, "Root queue disc does not have 1 class");
  NS_TEST_ASSERT_MSG_EQ (ATSroot->GetNPacketFilters (),1, "There should be one paket filter");
  
  //Create the group object which is shared by all the ATSScheduler that belong to the same group.
  group = CreateObject<ATSSchedulerGroup> ();
  group->InsertNewGroup (Seconds (1000000000), Seconds (0));
  leaf->SetATSGroup (group, 0);

  leaf->Initialize ();  
  ATSroot->Initialize ();

  //Set leaf callback to enqueue packets in transmission queue
  leaf->SetSendCallback ([ATSroot] (Ptr<QueueDiscItem> item)
                          {ATSroot->Enqueue (item);});
  leaf->SetTransmissionQueue (ATSroot);

  
  
 
  Config::SetDefault ("ns3::QueueDisc::Quota", UintegerValue (5));
  NodeContainer nodesA;
  nodesA.Create (2);
  Ptr<SimpleNetDevice> txDevA = CreateObject<SimpleNetDevice> ();
  nodesA.Get (0)->AddDevice (txDevA);
  Ptr<SimpleNetDevice> rxDevA = CreateObject<SimpleNetDevice> ();
  nodesA.Get (1)->AddDevice (rxDevA);
  Ptr<SimpleChannel> channelA = CreateObject<SimpleChannel> ();
  txDevA->SetChannel (channelA);
  rxDevA->SetChannel (channelA);
  txDevA->SetNode (nodesA.Get (0));
  rxDevA->SetNode (nodesA.Get (1));

  dst = txDevA->GetAddress ();

  Ptr<TrafficControlLayer> tcA = CreateObject<TrafficControlLayer> ();
  nodesA.Get (0)->AggregateObject (tcA);
  tcA->SetRootQueueDiscOnDevice (txDevA, ATSroot);
  tcA->Initialize ();

  Ptr<Packet> p1= Create<Packet> (pktSize);
  Ptr<Packet> p2= Create<Packet> (pktSize);
  Ptr<Packet> p3= Create<Packet> (pktSize);
  Ptr<Packet> p4= Create<Packet> (pktSize);
  Ptr<Packet> p5= Create<Packet> (pktSize);

  ATSroot->Enqueue (Create<ATSQueueDiscTestItem> (p1,dst,Simulator::Now ()));
  ATSroot->Enqueue (Create<ATSQueueDiscTestItem> (p2,dst,Simulator::Now ()));
  ATSroot->Enqueue (Create<ATSQueueDiscTestItem> (p3,dst,Simulator::Now ()));
  ATSroot->Enqueue (Create<ATSQueueDiscTestItem> (p4,dst,Simulator::Now ()));
  ATSroot->Enqueue (Create<ATSQueueDiscTestItem> (p5,dst,Simulator::Now ()));
  
  //The eligibilityTime of the p1 is 0.01s 

  Time eligibilityTimeP1 = Seconds (0.011); // SchedulerEligbilityTime = 0s (BucketEmptyTime)+ 1000B/100KB = 0.01s
  Time eligibilityTimeP2 = Seconds (0.021); // SchedulerEligbilityTime = 0.01 (BucketEmptyTime)+ 1000B/100KB = 0.02s
  Time eligibilityTimeP3 = Seconds (0.031); // SchedulerEligbilityTime = 0.02s (BucketEmptyTime)+ 1000B/100KB = 0.03s
  Time eligibilityTimeP4 = Seconds (0.041); // SchedulerEligbilityTime = 0.03s (BucketEmptyTime)+ 1000B/100KB = 0.04s
  Time eligibilityTimeP5 = Seconds (0.051); // SchedulerEligbilityTime = 0.04s (BucketEmptyTime)+ 1000B/100KB = 0.05s

  Simulator::Schedule (Seconds (0.0015),&AtsQueueDiscTestCase::CheckDequeue, this, leaf, 5,"There should be 5 packets ready for transmission");
  Simulator::Schedule (eligibilityTimeP1,&AtsQueueDiscTestCase::CheckDequeue, this, leaf, 4,"There should be 4 packets");
  Simulator::Schedule (eligibilityTimeP2,&AtsQueueDiscTestCase::CheckDequeue, this, leaf, 3,"There should be 3 packets");
  Simulator::Schedule (eligibilityTimeP3,&AtsQueueDiscTestCase::CheckDequeue, this, leaf, 2,"There should be 2 packets");
  Simulator::Schedule (eligibilityTimeP4,&AtsQueueDiscTestCase::CheckDequeue, this, leaf, 1,"There should be 1 packets");
  Simulator::Schedule (eligibilityTimeP5,&AtsQueueDiscTestCase::CheckDequeue, this, leaf, 0,"There should be 0 packets");

  //Dequeue 5 packets from trnasmission queue Quota=5
  Simulator::Schedule (eligibilityTimeP5,&AtsQueueDiscTestCase::CheckDequeue, this, ATSroot, 5,"There should be 5 packets ready for trnasmission");
  Simulator::Schedule (Seconds (0.07),&AtsQueueDiscTestCase::CheckDequeue, this, ATSroot, 0,"There should be 0 packets ready for trnasmission"); 
  Simulator::Schedule (Seconds (0.06),&QueueDisc::Run, ATSroot);
  Simulator::Schedule (Seconds (0.07),&AtsQueueDiscTestCase::CheckDequeue, this, ATSroot, 0,"There should be 0 packets ready for trnasmission");

  Simulator::Stop (Seconds (0.1));
  Simulator::Run ();

  //Test3: In this test we have strucuture where one ATSTransmissionQueue have two ATSSchedulerQueue attach. Both same group
  /**(1:2)
   *              ATSTransmissionQueueDisc (Root) 
                  *            |
                  *            |
                  *            |
                  * -------------------------
                  * |                       |
                  * |                       |
   *  ATSSchedulerQueueDisc (Leaf1)   ATSSchedulerQueueDisc (Leaf2) (Same group)
   * 
   * 1. Enqueue 2 packets, 1 in each leaf and check that the eligibilityTime is calculated properly depending on the groupTime
   * 2. Commited Information rate of leaf1 is the half of leaf2. This means that when calculating eligibility time of 
   * Packet 2 (leaf2), the groupEligibilityTime should be bigger that schedulerEligibilityTime. 
   * 3. check that organuization into groups results in non-decreasing ordering of elibilityTimes.
   */
  Ptr<ATSSchedulerQueueDisc> leaf1 = CreateObject<ATSSchedulerQueueDisc> ();
  Ptr<ATSSchedulerQueueDisc> leaf2 = CreateObject<ATSSchedulerQueueDisc> ();
  DataRate rate1 = DataRate ("50KB/s");
  NS_TEST_ASSERT_MSG_EQ (leaf1->SetAttributeFailSafe ("Burst",UintegerValue (burst)),true,
                        "Verify that we can set the attribute Burst");
  NS_TEST_ASSERT_MSG_EQ (leaf2->SetAttributeFailSafe ("Burst",UintegerValue (burst)),true,
                        "Verify that we can set the attribute Burst");                      
  NS_TEST_ASSERT_MSG_EQ (leaf1->SetAttributeFailSafe ("Rate",DataRateValue (rate1)),true,
                        "Verify that we can set the attribute Rate");
  NS_TEST_ASSERT_MSG_EQ (leaf2->SetAttributeFailSafe ("Rate",DataRateValue (rate)),true,
                        "Verify that we can set the attribute Rate");
  NS_TEST_ASSERT_MSG_EQ (leaf1->SetAttributeFailSafe ("ClockOffsetVariationMax",TimeValue (clockOffsetVariation)),true,
                        "Verify that we can set the attribute ClockOffsetVariationMax");
  NS_TEST_ASSERT_MSG_EQ (leaf2->SetAttributeFailSafe ("ClockOffsetVariationMax",TimeValue (clockOffsetVariation)),true,
                        "Verify that we can set the attribute ClockOffsetVariationMax");                      
  NS_TEST_ASSERT_MSG_EQ (leaf1->SetAttributeFailSafe ("ClockRateDeviationMax",TimeValue (clockRateDeviationMax)),true,
                        "Verify that we can set the attribute ClockRateDeviationMax");
  NS_TEST_ASSERT_MSG_EQ (leaf2->SetAttributeFailSafe ("ClockRateDeviationMax",TimeValue (clockRateDeviationMax)),true,
                        "Verify that we can set the attribute ClockRateDeviationMax");                                               
  NS_TEST_ASSERT_MSG_EQ (leaf1->SetAttributeFailSafe ("ArrivalRecognitionDelayMax",TimeValue (arrivalRecognitionDelayMax)),true,
                        "Verify that we can set the attribute ArrivalRecognitionDelayMax");
  NS_TEST_ASSERT_MSG_EQ (leaf2->SetAttributeFailSafe ("ArrivalRecognitionDelayMax",TimeValue (arrivalRecognitionDelayMax)),true,
                        "Verify that we can set the attribute ArrivalRecognitionDelayMax");                      
  NS_TEST_ASSERT_MSG_EQ (leaf1->SetAttributeFailSafe ("ProcessDelayMin",TimeValue (processDelayMin)),true,
                        "Verify that we can set the attribute ProcessDelayMin");
  NS_TEST_ASSERT_MSG_EQ (leaf2->SetAttributeFailSafe ("ProcessDelayMin",TimeValue (processDelayMin)),true,
                        "Verify that we can set the attribute ProcessDelayMin");                      
  NS_TEST_ASSERT_MSG_EQ (leaf1->SetAttributeFailSafe ("ProcessDelayMax",TimeValue (processDelayMax)),true,
                        "Verify that we can set the attribute ProcessDelayMax");
  NS_TEST_ASSERT_MSG_EQ (leaf2->SetAttributeFailSafe ("ProcessDelayMax",TimeValue (processDelayMax)),true,
                        "Verify that we can set the attribute ProcessDelayMax");                        
  NS_TEST_ASSERT_MSG_EQ (leaf1->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (QueueSizeUnit::PACKETS, maxSize))), true,
                         "Verify that we can set the attribute maxSize");
  NS_TEST_ASSERT_MSG_EQ (leaf2->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (QueueSizeUnit::PACKETS, maxSize))), true,
                         "Verify that we can set the attribute maxSize");      
      

  ATSroot = CreateObject<ATSTransmissionQueueDisc> ();

  
  NS_TEST_ASSERT_MSG_EQ (ATSroot->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (QueueSizeUnit::PACKETS, 20))), true,
                         "Verify that we can set the attribute maxSize");
  Ptr<QueueDiscClass> c1 = CreateObject<QueueDiscClass> ();
  Ptr<QueueDiscClass> c2 = CreateObject<QueueDiscClass> ();

  filter = CreateObject<ATSQueueDiscTestFilter> (true);
  c1->SetQueueDisc (leaf1);
  c2->SetQueueDisc (leaf2);
  ATSroot->AddQueueDiscClass (c1);
  ATSroot->AddQueueDiscClass (c2);
  ATSroot->AddPacketFilter (filter);

  NS_TEST_ASSERT_MSG_EQ (ATSroot->GetNQueueDiscClasses (),2, "Root queue disc does not have 2 class");
  NS_TEST_ASSERT_MSG_EQ (ATSroot->GetNPacketFilters (),1, "There should be one paket filter");
  
  group = CreateObject<ATSSchedulerGroup> ();
  group->InsertNewGroup (Seconds (1000000000), Seconds (0));
  leaf1->SetATSGroup (group, 0);
  leaf2->SetATSGroup (group, 0);

  leaf1->Initialize ();
  leaf2->Initialize (); 
  Config::SetDefault ("ns3::QueueDisc::Quota", UintegerValue (5));
  NodeContainer nodesB;
  nodesB.Create (2);
  Ptr<SimpleNetDevice> txDevB = CreateObject<SimpleNetDevice> ();
  nodesB.Get (0)->AddDevice (txDevB);
  Ptr<SimpleNetDevice> rxDevB = CreateObject<SimpleNetDevice> ();
  nodesB.Get (1)->AddDevice (rxDevB);
  Ptr<SimpleChannel> channelB = CreateObject<SimpleChannel> ();
  txDevB->SetChannel (channelB);
  rxDevB->SetChannel (channelB);
  txDevB->SetNode (nodesB.Get (0));
  rxDevB->SetNode (nodesB.Get (1));

  dst = txDevA->GetAddress ();

  Ptr<TrafficControlLayer> tcB = CreateObject<TrafficControlLayer> ();
  nodesB.Get (0)->AggregateObject (tcB);
  tcB->SetRootQueueDiscOnDevice (txDevB, ATSroot);
  tcB->Initialize ();
   
  ATSroot->Initialize ();

  leaf1->SetSendCallback ([ATSroot] (Ptr<QueueDiscItem> item)
                          {ATSroot->Enqueue (item);});
  leaf1->SetTransmissionQueue (ATSroot); 
  leaf2->SetSendCallback ([ATSroot] (Ptr<QueueDiscItem> item)
                          {ATSroot->Enqueue (item);});
  leaf2->SetTransmissionQueue (ATSroot);                      


  filter->SetReturnValue (0);
  AtsQueueDiscTestCase::Enqueue (dst, pktSize, ATSroot);
  Simulator::ScheduleNow (&AtsQueueDiscTestCase::CheckDequeue, this, leaf1, 1,"There should be 1 packets");

  filter->SetReturnValue (1);
  AtsQueueDiscTestCase::Enqueue (dst, pktSize, ATSroot);
  Simulator::ScheduleNow (&AtsQueueDiscTestCase::CheckDequeue, this, leaf2, 1,"There should be 1 packets");

  eligibilityTimeP1 =  Seconds (0.02); // SchedulerEligbilityTime = 0.1s (BucketEmptyTime) (initialize with Simulator::Now ())+ 500B/100KB = 0.12s
  eligibilityTimeP2 = eligibilityTimeP1; // SchedulerEligbilityTime = 0.11s (GroupEligbilityTime is the max) EligibilityTime = GroupEligbilityTime  
                                      // (Should not be SchedulerEligbilityTime = 0.1s (BucketEmptyTime)+ 1000B/100KB = 0.11s)
  
  Simulator::Schedule (Seconds (0.01),&AtsQueueDiscTestCase::CheckDequeue, this, leaf2, 1,"There should be 1 packets in leaf2");
  Simulator::Schedule (eligibilityTimeP1,&AtsQueueDiscTestCase::CheckDequeue, this, leaf1, 0,"There should be 1 packets in leaf1");
  Simulator::Schedule (eligibilityTimeP2,&AtsQueueDiscTestCase::CheckDequeue, this, leaf2, 0,"There should be 1 packets in leaf2");

  Simulator::Schedule (Seconds (0.03),&AtsQueueDiscTestCase::CheckDequeue, this, ATSroot, 2,"There should be 5 packets ready for trnasmission");
  
  Simulator::Schedule (Seconds (0.03),&QueueDisc::Run, ATSroot);
  Simulator::Schedule (Seconds (0.04),&AtsQueueDiscTestCase::CheckDequeue, this, ATSroot, 0,"There should be 0 packets ready for trnasmission");


  Simulator::Stop (Seconds (1.15));
  Simulator::Run ();





  Simulator::Destroy ();
}

// The TestSuite class names the TestSuite, identifies what type of TestSuite,
// and enables the TestCases to be run.  Typically, only the constructor for
// this class must be defined
//
class AtsBridgeTestSuite : public TestSuite
{
public:
  AtsBridgeTestSuite ()
  : TestSuite ("ats-queue", UNIT)
  {
    AddTestCase (new AtsQueueDiscTestCase (), TestCase::QUICK);
  }

}g_AtsBridgeTestSuite;

