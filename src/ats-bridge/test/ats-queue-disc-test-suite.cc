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
  void CheckDequeue (Ptr<ATSSchedulerQueueDisc> leaf);
  virtual void DoRun (void);
};

AtsQueueDiscTestCase::AtsQueueDiscTestCase ()
  : TestCase ("AtsBridge test case (does nothing)")
{
}
void
AtsQueueDiscTestCase::CheckDequeue (Ptr<ATSSchedulerQueueDisc> leaf)
{
  NS_TEST_ASSERT_MSG_EQ (leaf->GetCurrentSize ().GetValue (), 0, "There should be no packets");
}
void
AtsQueueDiscTestCase::DoRun (void)
{
  /**Test 1: Simple enqueue ATSTransmissionQueueDisc 
   *          (1:1)
   * ATSTransmissionQueueDisc (Root)
   *            |
   *            |
   *            |
   *    ATSSchedulerQueueDisc (Leaf)
   *  */

  //Scheduler Parameters (For tocket bucket shaping in ATSScheduler)

  uint32_t burst = 200;
  DataRate rate = DataRate ("100KB/s");
  Time clockOffsetVariation = Seconds (0);
  Time clockRateDeviationMax = Seconds (0);
  Time arrivalRecognitionDelayMax = Seconds (0);
  Time processDelayMin = Seconds (0);
  Time processDelayMax = Seconds (0);
  uint32_t pktSize = 0;
  Address dst;
  pktSize = 500;
  uint32_t maxSize = 5;


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
  
  

  Ptr<Packet> p1= Create<Packet> (pktSize);
  Ptr<ATSQueueDiscTestItem> item = Create<ATSQueueDiscTestItem> (p1,dst,Simulator::Now ());

  // Create Root queue disc and add class-filter

  Ptr<ATSTransmissionQueueDisc> ATSroot = CreateObject<ATSTransmissionQueueDisc> ();  

  

  Ptr<QueueDiscClass> c = CreateObject<QueueDiscClass> ();
  Ptr<ATSQueueDiscTestFilter> filter = CreateObject<ATSQueueDiscTestFilter> (true);

  c->SetQueueDisc (leaf);
  ATSroot->AddQueueDiscClass (c);
  ATSroot->AddPacketFilter (filter);
  filter->SetReturnValue (0);

  
  NS_TEST_ASSERT_MSG_EQ (ATSroot->GetNQueueDiscClasses (),1, "Root queue disc does not have 1 class");
  NS_TEST_ASSERT_MSG_EQ (ATSroot->GetNPacketFilters (),1, "There should be one paket filter");
  
  //Create the group object
  Ptr<ATSSchedulerGroup> group = CreateObject<ATSSchedulerGroup> ();

  group->InsertNewGroup (Seconds (10), Seconds (0));
  leaf->SetATSGroup (group);

  leaf->Initialize ();  
  ATSroot->Initialize ();

  NS_TEST_ASSERT_MSG_EQ (leaf->GetCurrentSize ().GetValue (), 0, "There should be no packets");
  ATSroot->Enqueue (Create<ATSQueueDiscTestItem> (p1,dst,Simulator::Now ()));
  NS_TEST_ASSERT_MSG_EQ (leaf->GetCurrentSize ().GetValue (), 1, "There should be one packet");
  Simulator::Schedule (Seconds(1),&AtsQueueDiscTestCase::CheckDequeue, this, leaf);
  Simulator::Stop (Seconds (1.3));
  Simulator::Run ();


  std::cout << "holaaa" << std::endl;
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

