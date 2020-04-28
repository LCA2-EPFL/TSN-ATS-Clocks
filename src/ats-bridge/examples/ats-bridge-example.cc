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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("AtsBridgeExample");

int 
main (int argc, char *argv[])
{
   //Set LocalTime Simulator Impl
  GlobalValue::Bind ("SimulatorImplementationType", 
                     StringValue ("ns3::LocalTimeSimulatorImpl"));
  
  bool verbose = true;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.Parse (argc,argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer terminals;
  terminals.Create (2);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (1);

  //Aggregate clock 

  Ptr<PerfectClockModelImpl> clockImpl0 = CreateObject <PerfectClockModelImpl> ();
  Ptr<PerfectClockModelImpl> clockImpl1 = CreateObject <PerfectClockModelImpl> ();
  Ptr<PerfectClockModelImpl> clockImpl2 = CreateObject <PerfectClockModelImpl> ();

  clockImpl0 -> SetAttribute ("Frequency", DoubleValue (1));
  clockImpl1 -> SetAttribute ("Frequency", DoubleValue (1));
  clockImpl2 -> SetAttribute ("Frequency", DoubleValue (1));

  
  Ptr<LocalClock> clock0 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock1 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock2 = CreateObject<LocalClock> ();

  clock0 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl0));
  clock1 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl1));
  clock2 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl2));

  Ptr<Node> source = terminals.Get (0);
  Ptr<Node> consumer = terminals.Get (1);
  Ptr<Node> switchATS = csmaSwitch.Get (0);

  source -> AggregateObject (clock0);
  consumer -> AggregateObject (clock1);
  switchATS ->AggregateObject (clock2);



  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  // Create the csma links, from each terminal to the switch

  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchDevices;

  for (int i = 0; i < 2; i++)
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
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::Burst", UintegerValue (558));

 
  //Create filter
  
  TrafficControlHelper tcATS;
  uint16_t handle = tcATS.SetRootQueueDisc ("ns3::ATSTransmissionQueueDisc");
  tcATS.AddPacketFilter (handle, "ns3::ATSQueueDiscFilter");
  TrafficControlHelper::ClassIdList cid = tcATS.AddQueueDiscClasses (handle, 3, "ns3::QueueDiscClass");
  tcATS.AddChildQueueDisc (handle, cid[0], "ns3::ATSSchedulerQueueDisc", "Rate", DataRateValue (DataRate ("84.375KB/s")));
  tcATS.AddChildQueueDisc (handle, cid[1], "ns3::ATSSchedulerQueueDisc", "Rate", DataRateValue (DataRate ("84.375KB/s")));
  tcATS.AddChildQueueDisc (handle, cid[2], "ns3::ATSSchedulerQueueDisc");
  tcATS.Install (switchDevices.Get (1));

  // Add internet stack to the terminals
  InternetStackHelper internet;
  internet.Install (terminals);

  // We've got the "hardware" in place.  Now we need to add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4.Assign (terminalDevices);

  //
  // Create an OnOff application to send UDP datagrams from node zero to node 4.
  //
  NS_LOG_INFO ("Create Applications.");
  uint16_t port1 = 9;   // Discard port (RFC 863)
  uint16_t port2 = 10;
  uint16_t port3 = 11;

  
  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.2"), port1)));
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("80KB/s")));
  ApplicationContainer app = onoff.Install (terminals.Get (0));
  app.Start (Seconds (1));
  app.Stop (Seconds (1.3));

  OnOffHelper onoff1 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.2"), port2)));
  onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff1.SetAttribute ("DataRate", DataRateValue (DataRate ("80KB/s")));
  app = onoff1.Install (terminals.Get (0));
  app.Start (Seconds (1));
  app.Stop (Seconds (1.3));

  

  //Create this flow to avoid having ARP in the computation of end to end delay
  OnOffHelper onoff2 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.2"), port3)));
  onoff2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff2.SetAttribute ("DataRate", DataRateValue (DataRate ("20KB/s")));
  app = onoff2.Install (terminals.Get (0));
  app.Start (Seconds (0.9));
  app.Stop (Seconds (0.95));
  
  // Create two packet sink to measure end to end delay
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address ("10.1.1.2"), port1)));
  app = sink.Install (terminals.Get (1));
  app.Start (Seconds (0.0));
  PacketSinkHelper sink1 ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address ("10.1.1.2"), port2)));
  app = sink1.Install (terminals.Get (1));
  app.Start (Seconds (0.0));

  //
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Trace output will be sent to the file "csma-bridge.tr"
  //
  AsciiTraceHelper ascii;
  //csma.EnableAsciiAll (ascii.CreateFileStream ("csma-ats.tr"));
  csma.EnableAscii (ascii.CreateFileStream ("csma-ats-port.tr"), switchDevices.Get (1));
  //
  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     csma-bridge-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  //
  csma.EnablePcapAll ("csma-bridge", false);
  
  //Enable pcap for node 0 Arrival packet to tx queue of the device.
  PcapHelper pcaphelper;
  Ptr<PcapFileWrapper> file = pcaphelper.CreateFile ("Source-packet-transmitted-Enqueue", std::ios::out, PcapHelper::DLT_EN10MB);
  Ptr<CsmaNetDevice> csmaDevice0 = terminalDevices.Get (0)->GetObject<CsmaNetDevice> ();
  Ptr<Queue<Packet> > queue = csmaDevice0->GetQueue ();
  
  pcaphelper.HookDefaultSink <Queue<Packet>> (queue, "Enqueue", file);
  Ptr<PcapFileWrapper> file1 = pcaphelper.CreateFile ("Source-packet-transmitted-Dequeue", std::ios::out, PcapHelper::DLT_EN10MB);
  
  pcaphelper.HookDefaultSink <Queue<Packet>> (queue, "Dequeue", file1);
  
  std::string probeType = "ns3::PacketProbe";
  std::string tracePath = "/NodeList/0/DeviceList/0/TxQueue/Enqueue";
  GnuplotHelper gnuHelper;
  gnuHelper.ConfigurePlot ("Source-2-flows-Enqueue",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Packet Byte");
  gnuHelper.PlotProbe (probeType,
                        tracePath,
                        "OutputBytes",
                        "Packet Byte Count",
                        GnuplotAggregator::KEY_BELOW);      
  tracePath = "/NodeList/0/DeviceList/0/TxQueue/Dequeue";
  gnuHelper.ConfigurePlot ("Source-2-flows-Dequeue",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Packet Byte");                          
  gnuHelper.PlotProbe (probeType,
                        tracePath,
                        "OutputBytes",
                        "Packet Byte Count",
                        GnuplotAggregator::KEY_BELOW);
  tracePath = "/NodeList/2/DeviceList/1/TxQueue/Dequeue";
  gnuHelper.ConfigurePlot ("ATSTxDequeue",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Packet Byte");                          
  gnuHelper.PlotProbe (probeType,
                        tracePath,
                        "OutputBytes",
                        "Packet Byte Count",
                        GnuplotAggregator::KEY_BELOW);
  tracePath = "/NodeList/1/DeviceList/0/MacRx";
  gnuHelper.ConfigurePlot ("ConsumerReceive",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Packet Byte");                          
  gnuHelper.PlotProbe (probeType,
                        tracePath,
                        "OutputBytes",
                        "Packet Byte Count",
                        GnuplotAggregator::KEY_BELOW);                      
  //Time plots
  tracePath = "/NodeList/2/$ns3::TrafficControlLayer/RootQueueDiscList/*/DequeueTxTime";
  probeType = "ns3::TimeProbe";
  gnuHelper.ConfigurePlot ("Dequeue-from-transmission-queue",
                            "Dequeue time",
                            "Time (seconds)",
                            "Time"); 
                                              
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "Output",
                      "Packet Time",
                      GnuplotAggregator::NO_KEY);

  tracePath = "/NodeList/2/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/0/QueueDisc/EligibilityTimeScheduler";
  gnuHelper.ConfigurePlot ("ElibilityTimeDequeue-0",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "Output",
                      "Time",
                      GnuplotAggregator::KEY_BELOW);
  tracePath = "/NodeList/2/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/1/QueueDisc/EligibilityTimeScheduler";
  gnuHelper.ConfigurePlot ("ElibilityTimeDequeue-1",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "Output",
                      "Time",
                      GnuplotAggregator::KEY_BELOW);
  tracePath = "/NodeList/2/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/0/QueueDisc/ArrivalTimeScheduler";
  gnuHelper.ConfigurePlot ("ArrivalTimeToScheduler-0",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "Output",
                      "Time",
                      GnuplotAggregator::KEY_BELOW);
  tracePath = "/NodeList/2/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/1/QueueDisc/ArrivalTimeScheduler";
  gnuHelper.ConfigurePlot ("ArrivalTimeToScheduler-1",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "Output",
                      "Time",
                      GnuplotAggregator::KEY_BELOW); 


  //End to end delay trace source. 
  //Flow 1 TX
  tracePath = "/NodeList/0/ApplicationList/0/Tx";
  probeType = "ns3::PacketProbe";
  gnuHelper.ConfigurePlot ("FLOW1TX",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "OutputBytes",
                      "Time",
                      GnuplotAggregator::KEY_BELOW);   

 //Flow 2 TX
  tracePath = "/NodeList/0/ApplicationList/1/Tx";
  probeType = "ns3::PacketProbe";
  gnuHelper.ConfigurePlot ("FLOW2TX",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "OutputBytes",
                      "Time",
                      GnuplotAggregator::KEY_BELOW); 

  //FLOW 1 RX
  tracePath = "/NodeList/1/ApplicationList/0/Rx";
  probeType = "ns3::PacketProbe";
  gnuHelper.ConfigurePlot ("FLOW1RX",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "OutputBytes",
                      "Time",
                      GnuplotAggregator::KEY_BELOW); 
                    
  //FLOW 2 RX
  tracePath = "/NodeList/1/ApplicationList/1/Rx";
  probeType = "ns3::PacketProbe";
  gnuHelper.ConfigurePlot ("FLOW2RX",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "OutputBytes",
                      "Time",
                      GnuplotAggregator::KEY_BELOW); 

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


