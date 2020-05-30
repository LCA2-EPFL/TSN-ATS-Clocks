/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/ats-bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/gnuplot-helper.h"
#include "ns3/local-clock.h"
#include "ns3/perfect-clock-model-impl.h"
#include "ns3/adversarial-clock.h"
#include "ns3/adversarial-generation-helper.h"
#include "ns3/file-helper.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("AdversarialATS");

int 
main (int argc, char *argv[])
{
   //Set LocalTime Simulator Impl
  GlobalValue::Bind ("SimulatorImplementationType", 
                     StringValue ("ns3::LocalTimeSimulatorImpl"));
  Time::SetResolution (Time::PS);
  bool verbose = true;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.Parse (argc,argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer terminals;
  terminals.Create (4);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (1);

  //Aggregate clock 

  Ptr<AdversarialClock> clockImpl0 = CreateObject <AdversarialClock> ();
  Ptr<AdversarialClock> clockImpl1 = CreateObject <AdversarialClock> ();
  Ptr<AdversarialClock> clockImpl2 = CreateObject <AdversarialClock> ();

  Ptr<PerfectClockModelImpl> clockImpl3 = CreateObject <PerfectClockModelImpl> ();
  Ptr<PerfectClockModelImpl> clockImpl4 = CreateObject <PerfectClockModelImpl> ();
   
  //Clock paramenters
  clockImpl0 -> SetAttribute ("Delta", TimeValue (MicroSeconds (1)));
  clockImpl0 -> SetAttribute ("Period", TimeValue (MilliSeconds (30)));
  clockImpl0 -> SetAttribute ("Interval", TimeValue (MilliSeconds (10))); 
  clockImpl0 -> SetAttribute ("xvalueGlobal", TimeValue (MilliSeconds (5))); 
  clockImpl0 -> SetAttribute ("Slope", DoubleValue (1.001));

  clockImpl1 -> SetAttribute ("Delta", TimeValue (MicroSeconds (1)));
  clockImpl1 -> SetAttribute ("Period", TimeValue (MilliSeconds (30)));
  clockImpl1 -> SetAttribute ("Interval", TimeValue (MilliSeconds (10)));
  clockImpl1 -> SetAttribute ("xvalueGlobal", TimeValue (MilliSeconds (5) + MicroSeconds(0.5)
  +  Time (MilliSeconds (10).GetDouble ()/1.001)));  
  clockImpl1 -> SetAttribute ("Slope", DoubleValue (1.001));

  clockImpl2 -> SetAttribute ("Delta", TimeValue (MicroSeconds (1)));
  clockImpl2 -> SetAttribute ("Period", TimeValue (MilliSeconds (30)));
  clockImpl2 -> SetAttribute ("Interval", TimeValue (MilliSeconds (10)));
  clockImpl2 -> SetAttribute ("xvalueGlobal", TimeValue (MilliSeconds (5) + 2*MicroSeconds (0.5) + 2*
  Time (MilliSeconds (10).GetDouble ()/1.001)));
  clockImpl2 -> SetAttribute ("Slope", DoubleValue (1.001));

  clockImpl3 -> SetAttribute ("Frequency", DoubleValue (1));
  clockImpl4 -> SetAttribute ("Frequency", DoubleValue (1));

  
  Ptr<LocalClock> clock0 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock1 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock2 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock3 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock4 = CreateObject<LocalClock> ();


  clock0 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl0));
  clock1 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl1));
  clock2 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl2));
  clock3 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl3));
  clock4 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl4));


  Ptr<Node> source1 = terminals.Get (0);
  Ptr<Node> source2 = terminals.Get (1);
  Ptr<Node> source3 = terminals.Get (2);

  Ptr<Node> consumer = terminals.Get (3);
  Ptr<Node> switchATS = csmaSwitch.Get (0);

  source1 -> AggregateObject (clock0);
  source2 -> AggregateObject (clock1);
  source3 -> AggregateObject (clock2);
  consumer -> AggregateObject (clock3);
  switchATS -> AggregateObject (clock4);



  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000000000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (0)));

  // Create the csma links, from each terminal to the switch

  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchDevices;

  for (int i = 0; i < 4; i++)
    {
      NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch));
      terminalDevices.Add (link.Get (0));
      switchDevices.Add (link.Get (1));
    }

  // Create the bridge netdevice, which will do the packet switching
  Ptr<Node> switchNode = csmaSwitch.Get (0);
  ATSBridgeHelper bridge;
  bridge.Install (switchNode, switchDevices);

  //Set ATSSchedulerQueueDisc parameters
  
  Ptr<ATSSchedulerGroup> group;
  group = CreateObject<ATSSchedulerGroup> ();
  group->InsertNewGroup (Seconds (50), Seconds (0));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::MaxSize", QueueSizeValue (QueueSize ("7000p")));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::Group", PointerValue (group));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::GroupID", UintegerValue (0));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::Burst", UintegerValue (540));

 
  //Create filter
  
  TrafficControlHelper tcATS;
  uint16_t handle = tcATS.SetRootQueueDisc ("ns3::ATSTransmissionQueueDisc");
  tcATS.AddPacketFilter (handle, "ns3::ATSQueueDiscFilter");
  TrafficControlHelper::ClassIdList cid = tcATS.AddQueueDiscClasses (handle, 3, "ns3::QueueDiscClass");
  tcATS.AddChildQueueDisc (handle, cid[0], "ns3::ATSSchedulerQueueDisc", "Rate", DataRateValue (DataRate ("54KB/s")));
  tcATS.AddChildQueueDisc (handle, cid[1], "ns3::ATSSchedulerQueueDisc", "Rate", DataRateValue (DataRate ("54KB/s")));
  tcATS.AddChildQueueDisc (handle, cid[2], "ns3::ATSSchedulerQueueDisc");
  tcATS.Install (switchDevices.Get (3));

  // Add internet stack to the terminals
  InternetStackHelper internet;
  internet.Install (terminals);

  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4.Assign (terminalDevices);

  NS_LOG_INFO ("Create Applications.");
  uint16_t port1 = 9;   // Discard port (RFC 863)
  uint16_t port2 = 10;
  uint16_t port3 = 11;

  //Source 1
  AdversarialGenerationHelper onoff ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port1)));
  onoff.SetAttribute ("Period", TimeValue (MilliSeconds (30)));
  onoff.SetAttribute ("Xjvalue", TimeValue (MilliSeconds(5)));
  onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("51.200KB/s")));
  ApplicationContainer app = onoff.Install (terminals.Get (0));
  app.Start (MilliSeconds (0));
  app.Stop (Seconds (0.5));

  //Source 2
  AdversarialGenerationHelper onoff1 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port2)));
  onoff1.SetAttribute ("Period", TimeValue (MilliSeconds (30)));
  onoff1.SetAttribute ("Xjvalue", TimeValue (MilliSeconds (5) + MicroSeconds (0.5) +
  Time (MilliSeconds (10).GetDouble ()/1.001)));
  onoff1.SetAttribute ("DataRate", DataRateValue (DataRate ("51.200KB/s")));
  app = onoff1.Install (terminals.Get (1));
  app.Start (MilliSeconds (0));
  app.Stop (Seconds (0.5));

  //Source 3
  AdversarialGenerationHelper onoff5 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port3)));
  onoff5.SetAttribute ("Period", TimeValue (MilliSeconds (30)));
  onoff5.SetAttribute ("Xjvalue", TimeValue (MilliSeconds (5) + 2*MicroSeconds (0.5) + 2*
  Time (MilliSeconds (10).GetDouble ()/1.001)));
  onoff5.SetAttribute ("DataRate", DataRateValue (DataRate ("51.200KB/s")));
  app = onoff5.Install (terminals.Get (2));
  app.Start (MilliSeconds (0));
  app.Stop (Seconds (0.5));

  
  //Create this flow to avoid having ARP in the computation of end to end delay
  OnOffHelper onoff2 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port3)));
  onoff2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff2.SetAttribute ("DataRate", DataRateValue (DataRate ("1000KB/s")));
  app = onoff2.Install (terminals.Get (0));
  app.Start (Seconds (0));
  app.Stop (Seconds (0.01));

  //Create this flow to avoid having ARP in the computation of end to end delay
  OnOffHelper onoff3 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port3)));
  onoff3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff3.SetAttribute ("DataRate", DataRateValue (DataRate ("1000KB/s")));
  app = onoff3.Install (terminals.Get (1));
  app.Start (Seconds (0));
  app.Stop (Seconds (0.01));

  //Create this flow to avoid having ARP in the computation of end to end delay
  OnOffHelper onoff4 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port3)));
  onoff4.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff4.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff4.SetAttribute ("DataRate", DataRateValue (DataRate ("1000KB/s")));
  app = onoff4.Install (terminals.Get (2));
  app.Start (Seconds (0));
  app.Stop (Seconds (0.01));
  
  // Create two packet sink to measure end to end delay
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port1)));
  app = sink.Install (terminals.Get (3));
  app.Start (Seconds (0.0));
  PacketSinkHelper sink1 ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port2)));
  app = sink1.Install (terminals.Get (3));
  app.Start (Seconds (0.0));
  PacketSinkHelper sink2 ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port3)));
  app = sink2.Install (terminals.Get (3));
  app.Start (Seconds (0.0));

  csma.EnablePcapAll ("csma-bridge", false);
  
  //Enable pcap for node 0 Arrival packet to tx queue of the device.
  PcapHelper pcaphelper;
  Ptr<PcapFileWrapper> file = pcaphelper.CreateFile ("Source-packet-transmitted-Enqueue", std::ios::out, PcapHelper::DLT_EN10MB);
  Ptr<CsmaNetDevice> csmaDevice0 = terminalDevices.Get (0)->GetObject<CsmaNetDevice> ();
  Ptr<Queue<Packet> > queue = csmaDevice0->GetQueue ();
  
  pcaphelper.HookDefaultSink <Queue<Packet>> (queue, "Enqueue", file);
  Ptr<PcapFileWrapper> file1 = pcaphelper.CreateFile ("Source-packet-transmitted-Dequeue", std::ios::out, PcapHelper::DLT_EN10MB);
  
  pcaphelper.HookDefaultSink <Queue<Packet>> (queue, "Dequeue", file1);
  
  std::string probeType;
  std::string tracePath;
  GnuplotHelper gnuHelper;
  FileHelper fileHelper;                  
  //Time plots
  //EligibilityTime Flow 1
  //End to end delay trace source. 
  //Flow 1 TX GlobalTime
  FileHelper fileHelper1;
  tracePath = "/NodeList/0/ApplicationList/0/TXSendGlobal";
  fileHelper1.ConfigureFile ("TxGlobalFlow1",
                            FileAggregator::FORMATTED);
  fileHelper1.Set2dFormat ("%e %e");
  fileHelper1.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");   

  //FLOW 1 TX LocalTime
  FileHelper fileHelper2;
  tracePath = "/NodeList/0/ApplicationList/0/TXSendLocal";
  fileHelper2.ConfigureFile ("TxLocalFlow1",
                            FileAggregator::FORMATTED);
  fileHelper2.Set2dFormat ("%e %e");
  fileHelper2.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");  
                    
   //Flow 2 TX GlobalTime
  FileHelper fileHelper3;
  tracePath = "/NodeList/1/ApplicationList/0/TXSendGlobal";
  fileHelper3.ConfigureFile ("TxGlobalFlow2",
                            FileAggregator::FORMATTED);
  fileHelper3.Set2dFormat ("%e %e");
  fileHelper3.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");    


  //FLOW 2 TX LocalTime
  FileHelper fileHelper4;
  tracePath = "/NodeList/1/ApplicationList/0/TXSendLocal";
  fileHelper4.ConfigureFile ("TxLocalFlow2",
                            FileAggregator::FORMATTED);
  fileHelper4.Set2dFormat ("%e %e");
  fileHelper4.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");  
  FileHelper fileHelper5;
  fileHelper.ConfigureFile ("ElibilityTimeFlow1",
                            FileAggregator::FORMATTED);
  fileHelper5.Set2dFormat ("%e %e");
  fileHelper5.WriteProbe ("ns3::TimeProbe",
                        "/NodeList/4/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/0/QueueDisc/EligibilityTimeScheduler",
                        "Output");
                    
  //Flow 3 TX GlobalTime
  FileHelper fileHelper11;
  tracePath = "/NodeList/2/ApplicationList/0/TXSendGlobal";
  fileHelper11.ConfigureFile ("TxGlobalFlow3",
                            FileAggregator::FORMATTED);
  fileHelper11.Set2dFormat ("%e %e");
  fileHelper11.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");   

  //FLOW 3 TX LocalTime
  FileHelper fileHelper12;
  tracePath = "/NodeList/2/ApplicationList/0/TXSendLocal";
  fileHelper12.ConfigureFile ("TxLocalFlow3",
                            FileAggregator::FORMATTED);
  fileHelper12.Set2dFormat ("%e %e");
  fileHelper12.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");  

  //EligibilityTime Flow 2
  FileHelper fileHelper6;
  tracePath = "/NodeList/4/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/1/QueueDisc/EligibilityTimeScheduler";
  fileHelper6.ConfigureFile ("ElibilityTimeFlow2",
                            FileAggregator::FORMATTED);
  fileHelper6.Set2dFormat ("%e %e");
  fileHelper6.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");
                    
  //EligibilityTime Flow 3
  FileHelper fileHelper9;
  tracePath = "/NodeList/4/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/2/QueueDisc/EligibilityTimeScheduler";
  fileHelper6.ConfigureFile ("ElibilityTimeFlow3",
                            FileAggregator::FORMATTED);
  fileHelper6.Set2dFormat ("%e %e");
  fileHelper6.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");
  //Arrival Time to Scheduler Flow 1
  FileHelper fileHelper7;
  tracePath = "/NodeList/4/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/0/QueueDisc/ArrivalTimeScheduler";
  fileHelper7.ConfigureFile ("ArrivalTimeFlow1",
                            FileAggregator::FORMATTED);
  fileHelper7.Set2dFormat ("%e %e");
  fileHelper7.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");
  //Arrival Time to Scheduler Flow 1
  FileHelper fileHelper8;                  
  tracePath = "/NodeList/4/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/1/QueueDisc/ArrivalTimeScheduler";
  fileHelper8.ConfigureFile ("ArrivalTimeFlow2",
                            FileAggregator::FORMATTED);
  fileHelper8.Set2dFormat ("%e %e");
  fileHelper8.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");
  //Arrival Time to Scheduler Flow 3                      
  FileHelper fileHelper10;                  
  tracePath = "/NodeList/4/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/2/QueueDisc/ArrivalTimeScheduler";
  fileHelper8.ConfigureFile ("ArrivalTimeFlow3",
                            FileAggregator::FORMATTED);
  fileHelper8.Set2dFormat ("%e %e");
  fileHelper8.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");                   


  
                                      
  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


