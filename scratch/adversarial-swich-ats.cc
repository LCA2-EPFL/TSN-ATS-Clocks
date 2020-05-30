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
#include "ns3/bridge-helper.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("AdversarialSwitchATS");

int 
main (int argc, char *argv[])
{
   //Set LocalTime Simulator Impl
  GlobalValue::Bind ("SimulatorImplementationType", 
                     StringValue ("ns3::LocalTimeSimulatorImpl"));
  Time::SetResolution (Time::NS);
  bool verbose = true;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.Parse (argc,argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer terminals;
  terminals.Create (4);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (1);

  NodeContainer atsSwitch;
  atsSwitch.Create (1);

  //Aggregate clock 

  Ptr<AdversarialClock> clockImpl0 = CreateObject <AdversarialClock> ();
  Ptr<AdversarialClock> clockImpl1 = CreateObject <AdversarialClock> ();
  Ptr<AdversarialClock> clockImpl2 = CreateObject <AdversarialClock> ();

  Ptr<PerfectClockModelImpl> clockImpl3 = CreateObject <PerfectClockModelImpl> ();
  Ptr<PerfectClockModelImpl> clockImpl4 = CreateObject <PerfectClockModelImpl> ();
  Ptr<PerfectClockModelImpl> clockImpl5 = CreateObject <PerfectClockModelImpl> ();
  //Clock paramenters
  Time epsilon = NanoSeconds (500); //Time (MilliSeconds (10).GetDouble () * (1 - (1/1.001))) - MicroSeconds (1);
  Time period = 3 * (Time (MilliSeconds (10).GetDouble ()/1.001)) + 3 * epsilon;
  std::cout << "Epsilon: " << epsilon << std::endl;
  std::cout << "Period: " << period << std::endl;

  clockImpl0 -> SetAttribute ("Delta", TimeValue (MicroSeconds (1)));
  clockImpl0 -> SetAttribute ("Period", TimeValue (period));
  clockImpl0 -> SetAttribute ("Interval", TimeValue (MilliSeconds (10))); 
  clockImpl0 -> SetAttribute ("xvalueGlobal", TimeValue (MilliSeconds (5))); 
  clockImpl0 -> SetAttribute ("Slope", DoubleValue (1.001));

  clockImpl1 -> SetAttribute ("Delta", TimeValue (MicroSeconds (1)));
  clockImpl1 -> SetAttribute ("Period", TimeValue (period));
  clockImpl1 -> SetAttribute ("Interval", TimeValue (MilliSeconds (10)));
  clockImpl1 -> SetAttribute ("xvalueGlobal", TimeValue (MilliSeconds (5) + epsilon
  +  Time (MilliSeconds (10).GetDouble ()/1.001)));  
  clockImpl1 -> SetAttribute ("Slope", DoubleValue (1.001));

  clockImpl2 -> SetAttribute ("Delta", TimeValue (MicroSeconds (1)));
  clockImpl2 -> SetAttribute ("Period", TimeValue (period));
  clockImpl2 -> SetAttribute ("Interval", TimeValue (MilliSeconds (10)));
  clockImpl2 -> SetAttribute ("xvalueGlobal", TimeValue (MilliSeconds (5) + 2*epsilon + 2*
  Time (MilliSeconds (10).GetDouble ()/1.001)));
  clockImpl2 -> SetAttribute ("Slope", DoubleValue (1.001));

  clockImpl3 -> SetAttribute ("Frequency", DoubleValue (1));
  clockImpl4 -> SetAttribute ("Frequency", DoubleValue (1));
  clockImpl5 -> SetAttribute ("Frequency", DoubleValue (1));
  
  Ptr<LocalClock> clock0 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock1 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock2 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock3 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock4 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock5 = CreateObject<LocalClock> ();

  clock0 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl0));
  clock1 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl1));
  clock2 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl2));
  clock3 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl3));
  clock4 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl4));
  clock5 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl5));

  Ptr<Node> source1 = terminals.Get (0);
  Ptr<Node> source2 = terminals.Get (1);
  Ptr<Node> source3 = terminals.Get (2);

  Ptr<Node> consumer = terminals.Get (3);
  Ptr<Node> switchATS = atsSwitch.Get (0);
  Ptr<Node> nodeSwitch = csmaSwitch.Get (0);
  source1 -> AggregateObject (clock0);
  source2 -> AggregateObject (clock1);
  source3 -> AggregateObject (clock2);
  consumer -> AggregateObject (clock3);
  switchATS -> AggregateObject (clock4);
  nodeSwitch -> AggregateObject (clock5);


  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (500000000000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (0)));
  csma.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("10000p"));
  

  // Create the csma links, from each terminal to the switch

  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchDevices;
  NetDeviceContainer switchATSDevices;

  for (int i = 0; i < 3; i++)
    {
      NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch));
      terminalDevices.Add (link.Get (0));
      switchDevices.Add (link.Get (1));
    }

  NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (3), atsSwitch));
  terminalDevices.Add (link.Get (0));
  switchATSDevices.Add (link.Get (1));

  // Set datarate bettween Switch -- ATSSwitch

  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  link = csma.Install (NodeContainer (csmaSwitch, atsSwitch));
  switchDevices.Add (link.Get (0));
  Ptr<CsmaNetDevice> switchDevice = DynamicCast<CsmaNetDevice> (link.Get (0));
  switchDevice->SetInterframeGap (Seconds (0));
  switchATSDevices.Add (link.Get (1));

  // Create the bridge netdevice, which will do the packet switching
  Ptr<Node> atsSwitchNode = atsSwitch.Get (0);
  ATSBridgeHelper bridge;
  bridge.Install (atsSwitchNode, switchATSDevices);

  Ptr<Node> switchNode = csmaSwitch.Get (0);
  BridgeHelper bridge1;
  bridge1.Install (switchNode, switchDevices);



  //Set ATSSchedulerQueueDisc parameters
  
  Ptr<ATSSchedulerGroup> group;
  group = CreateObject<ATSSchedulerGroup> ();
  group->InsertNewGroup (Seconds (50), Seconds (0));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::MaxSize", QueueSizeValue (QueueSize ("7000p")));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::Group", PointerValue (group));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::GroupID", UintegerValue (0));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::Burst", UintegerValue (1498));
 
  //Create filter
  
  TrafficControlHelper tcATS;
  uint16_t handle = tcATS.SetRootQueueDisc ("ns3::ATSTransmissionQueueDisc");
  tcATS.AddPacketFilter (handle, "ns3::ATSQueueDiscFilter");
  TrafficControlHelper::ClassIdList cid = tcATS.AddQueueDiscClasses (handle, 3, "ns3::QueueDiscClass");
  tcATS.AddChildQueueDisc (handle, cid[0], "ns3::ATSSchedulerQueueDisc", "Rate", DataRateValue (DataRate ("149.8KB/s")));
  tcATS.AddChildQueueDisc (handle, cid[1], "ns3::ATSSchedulerQueueDisc", "Rate", DataRateValue (DataRate ("149.8KB/s")));
  tcATS.AddChildQueueDisc (handle, cid[2], "ns3::ATSSchedulerQueueDisc", "Rate", DataRateValue (DataRate ("149.8KB/s")));
  tcATS.Install (switchATSDevices.Get (0));

  // Add internet stack to the terminals
  InternetStackHelper internet;
  internet.Install (terminals);

  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4.Assign (terminalDevices);
/**
  //SET ARP chaches Node 1
  Ptr<Node> node1 = terminals.Get (0);
  Ptr<Ipv4> node1IPV4 = node1->GetObject<Ipv4> ();
  int32_t intIndex = node1IPV4->GetInterfaceForDevice (terminalDevices.Get (0));
  Ptr<Ipv4L3Protocol> node1prot = node1->GetObject<Ipv4L3Protocol> ();
  Ptr<Ipv4Interface> node1int = node1prot->GetInterface (intIndex);

  Ptr<ArpCache> node1Cache = node1int->GetArpCache (); 
  ArpCache::Entry* arpCacheEntry = node1Cache->Add (Ipv4Address ("10.1.1.4"));
  arpCacheEntry->SetMacAddress (Mac48Address ("00:00:00:00:00:07"));
  arpCacheEntry->MarkPermanent ();
*/
  //SET ARP chaches Node 2
  Ptr<Node> node2 = terminals.Get (1);
  Ptr<Ipv4> node2IPV4 = node2->GetObject<Ipv4> ();
  int32_t intIndex2 = node2IPV4->GetInterfaceForDevice (terminalDevices.Get (1));
  Ptr<Ipv4L3Protocol> node2prot = node2->GetObject<Ipv4L3Protocol> ();
  Ptr<Ipv4Interface> node2int = node2prot->GetInterface (intIndex2);

  Ptr<ArpCache> node2Cache = node2int->GetArpCache (); 
  ArpCache::Entry* arpCacheEntry2 = node2Cache->Add (Ipv4Address ("10.1.1.4"));
  arpCacheEntry2->SetMacAddress (Mac48Address ("00:00:00:00:00:07"));
  arpCacheEntry2->MarkPermanent ();

  //SET ARP chaches Node 3
  Ptr<Node> node3 = terminals.Get (2);
  Ptr<Ipv4> node3IPV4 = node3->GetObject<Ipv4> ();
  int32_t intIndex3 = node3IPV4->GetInterfaceForDevice (terminalDevices.Get (2));
  Ptr<Ipv4L3Protocol> node3prot = node3->GetObject<Ipv4L3Protocol> ();
  Ptr<Ipv4Interface> node3int = node3prot->GetInterface (intIndex3);

  Ptr<ArpCache> node3Cache = node3int->GetArpCache (); 
  ArpCache::Entry* arpCacheEntry3 = node3Cache->Add (Ipv4Address ("10.1.1.4"));
  arpCacheEntry3->SetMacAddress (Mac48Address ("00:00:00:00:00:07"));
  arpCacheEntry3->MarkPermanent ();

  NS_LOG_INFO ("Create Applications.");
  uint16_t port1 = 9;   // Discard port (RFC 863)
  uint16_t port2 = 10;
  uint16_t port3 = 11;
  uint16_t port4 = 12;

  //Source 1

  Time initialDeviation = MilliSeconds(5); //X1 value- x1 value in local + Delta/2
  AdversarialGenerationHelper onoff ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port1)));
  onoff.SetAttribute ("Period", TimeValue (period));
  onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("147KB/s")));
  onoff.SetAttribute ("PacketSize", UintegerValue (1470) );
  ApplicationContainer app = onoff.Install (terminals.Get (0));
  Time initApp = 2*period + initialDeviation;
  app.Start (initApp);
  app.Stop (Seconds (1));

  Ptr<Node> node1 = terminals.Get (0);
  Ptr<LocalClock> node1Clock = node1->GetObject <LocalClock> ();
  Time initApp2 = node1Clock->LocalToGlobalTime (initApp + MilliSeconds (10) - MicroSeconds (1)/2);
  initApp2 += epsilon;
  std::cout << "Init app 2 at: " << initApp2 << std::endl;

  //Source 2
  AdversarialGenerationHelper onoff1 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port2)));
  onoff1.SetAttribute ("Period", TimeValue (period));
  onoff1.SetAttribute ("DataRate", DataRateValue (DataRate ("147KB/s")));
  onoff1.SetAttribute ("PacketSize", UintegerValue (1470) );
  app = onoff1.Install (terminals.Get (1));
  app.Start (initApp2);
  app.Stop (Seconds (1));

  Ptr<Node> node4 = terminals.Get (1);
  Ptr<LocalClock> node2Clock = node4->GetObject <LocalClock> ();
  Time initApp3 = node2Clock->LocalToGlobalTime (initApp2 + MilliSeconds (10) - MicroSeconds (1)/2);
  initApp3 += epsilon;
  std::cout << "Init app 3 at: " << initApp3 << std::endl;

  //Source 3
  AdversarialGenerationHelper onoff5 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port3)));
  onoff5.SetAttribute ("Period", TimeValue (period));
  onoff5.SetAttribute ("DataRate", DataRateValue (DataRate ("147KB/s")));
  onoff5.SetAttribute ("PacketSize", UintegerValue (1470) );
  app = onoff5.Install (terminals.Get (2));
  app.Start (initApp3);
  app.Stop (Seconds (1));

 
  //Create this flow to avoid having ARP in the computation of end to end delay
  OnOffHelper onoff2 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port4)));
  onoff2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff2.SetAttribute ("DataRate", DataRateValue (DataRate ("1000KB/s")));
  onoff2.SetAttribute ("PacketSize", UintegerValue (10) );
  app = onoff2.Install (terminals.Get (0));
  app.Start (Seconds (0));
  app.Stop (Seconds (0.001));

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
  
  Time delay0 = 1*3*( Time (MilliSeconds (10).GetDouble ()*(1-1/1.001)) - epsilon);
  Time delay = 2*3*( Time (MilliSeconds (10).GetDouble ()*(1-1/1.001)) - epsilon);
  Time delay1 = 3*3*( Time (MilliSeconds (10).GetDouble ()*(1-1/1.001)) - epsilon);
  Time delay2 = 4*3*( Time (MilliSeconds (10).GetDouble ()*(1-1/1.001)) - epsilon);
  Time delay3 = 5*3*( Time (MilliSeconds (10).GetDouble ()*(1-1/1.001)) - epsilon);
  Time delay4 = 6*3*( Time (MilliSeconds (10).GetDouble ()*(1-1/1.001)) - epsilon);
  Time delay5 = 7*3*( Time (MilliSeconds (10).GetDouble ()*(1-1/1.001)) - epsilon);
  Time delay6 = 8*3*( Time (MilliSeconds (10).GetDouble ()*(1-1/1.001)) - epsilon);

  Time increase = 3 * ( Time (MilliSeconds (10).GetDouble ()*(1-1/1.001)) -epsilon);

  std::cout << "Delay " << delay0 << std::endl;
  std::cout << "Delay " << delay << std::endl;
  std::cout << "Delay " << delay1 << std::endl;
  std::cout << "Delay " << delay2 << std::endl;
  std::cout << "Delay " << delay3 << std::endl;
  std::cout << "Delay " << delay4 << std::endl;
  std::cout << "Delay " << delay5 << std::endl;
  std::cout << "Delay " << delay6 << std::endl;

  std::cout << "Increase " << increase << std::endl;

  std::string probeType;
  std::string tracePath;
  GnuplotHelper gnuHelper;
  FileHelper fileHelper;                  
  //Time plots
  //EligibilityTime Flow 1
  //End to end delay trace source. 
  //FIRST FLOW for arp

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

  //Eligibility time flow 1
  FileHelper fileHelper5;
  fileHelper5.ConfigureFile ("ElibilityTimeFlow1",
                            FileAggregator::FORMATTED);
  fileHelper5.Set2dFormat ("%e %e");
  fileHelper5.WriteProbe ("ns3::TimeProbe",
                        "/NodeList/5/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/0/QueueDisc/EligibilityTimeScheduler",
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
  tracePath = "/NodeList/5/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/1/QueueDisc/EligibilityTimeScheduler";
  fileHelper6.ConfigureFile ("ElibilityTimeFlow2",
                            FileAggregator::FORMATTED);
  fileHelper6.Set2dFormat ("%e %e");
  fileHelper6.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");
                    
  //EligibilityTime Flow 3
  FileHelper fileHelper9;
  tracePath = "/NodeList/5/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/2/QueueDisc/EligibilityTimeScheduler";
  fileHelper9.ConfigureFile ("ElibilityTimeFlow3",
                            FileAggregator::FORMATTED);
  fileHelper9.Set2dFormat ("%e %e");
  fileHelper9.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");

  //Arrival Time to Scheduler Flow 1
  FileHelper fileHelper7;
  tracePath = "/NodeList/5/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/0/QueueDisc/ArrivalTimeScheduler";
  fileHelper7.ConfigureFile ("ArrivalTimeFlow1",
                            FileAggregator::FORMATTED);
  fileHelper7.Set2dFormat ("%e %e");
  fileHelper7.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");

  //Arrival Time to Scheduler Flow 1
  FileHelper fileHelper8;                  
  tracePath = "/NodeList/5/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/1/QueueDisc/ArrivalTimeScheduler";
  fileHelper8.ConfigureFile ("ArrivalTimeFlow2",
                            FileAggregator::FORMATTED);
  fileHelper8.Set2dFormat ("%e %e");
  fileHelper8.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");

  //Arrival Time to Scheduler Flow 3                      
  FileHelper fileHelper10;                  
  tracePath = "/NodeList/5/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/2/QueueDisc/ArrivalTimeScheduler";
  fileHelper10.ConfigureFile ("ArrivalTimeFlow3",
                            FileAggregator::FORMATTED);
  fileHelper10.Set2dFormat ("%e %e");
  fileHelper10.WriteProbe ("ns3::TimeProbe",
                        tracePath,
                        "Output");                   

  GnuplotHelper plotHelper;
  plotHelper.ConfigurePlot ("Sink",
                            "ArrivalCurve Sink",
                            "Simulation time (Seconds)",
                            "Interarrival time (Seconds)",
                            "png");
  plotHelper.PlotProbe ("ns3::PacketProbe",
                        "/NodeList/3/ApplicationList/0/Rx",
                        "OutputBytes",
                        "Emitter Interarrival Time",
                        GnuplotAggregator::KEY_INSIDE);
  
                                      
  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


