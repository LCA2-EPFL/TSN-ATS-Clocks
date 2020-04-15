/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/ats-bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/gnuplot-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("AtsBridgeExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.Parse (argc,argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer terminals;
  terminals.Create (4);

  NodeContainer csmaSwitch;
  csmaSwitch.Create (1);

  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

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
  group->InsertNewGroup (Seconds (10), Seconds (0));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::MaxSize", QueueSizeValue (QueueSize ("7000p")));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::Group", PointerValue (group));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::GroupID", UintegerValue (0));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::Burst", UintegerValue (540));
  Config::SetDefault ("ns3::ATSSchedulerQueueDisc::Rate", DataRateValue (DataRate ("10KB/s")));

  //Create filter
  
  TrafficControlHelper tcATS;
  uint16_t handle = tcATS.SetRootQueueDisc ("ns3::ATSTransmissionQueueDisc");
  tcATS.AddPacketFilter (handle, "ns3::ATSQueueDiscFilter");
  TrafficControlHelper::ClassIdList cid = tcATS.AddQueueDiscClasses (handle, 3, "ns3::QueueDiscClass");
  tcATS.AddChildQueueDisc (handle, cid[0], "ns3::ATSSchedulerQueueDisc");
  tcATS.AddChildQueueDisc (handle, cid[1], "ns3::ATSSchedulerQueueDisc");
  tcATS.AddChildQueueDisc (handle, cid[2], "ns3::ATSSchedulerQueueDisc");
  tcATS.Install (switchDevices.Get (3));

  // Add internet stack to the terminals
  InternetStackHelper internet;
  internet.Install (terminals);

  // We've got the "hardware" in place.  Now we need to add IP addresses.
  //
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4.Assign (terminalDevices);

  //
  // Create an OnOff application to send UDP datagrams from node zero to node 4.
  //
  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;   // Discard port (RFC 863)

  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port)));
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("10KB/s")));
  ApplicationContainer app = onoff.Install (terminals.Get (0));
  app.Start (Seconds (1.0));
  app.Stop (Seconds (1.9));

  OnOffHelper onoff1 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.2"), port)));
  onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff1.SetAttribute ("DataRate", DataRateValue (DataRate ("200KB/s")));
  app = onoff1.Install (terminals.Get (0));
  app.Start (Seconds (1.0));
  app.Stop (Seconds (1.4));

  OnOffHelper onoff2 ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (Ipv4Address ("10.1.1.2"), port)));
  onoff2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff2.SetAttribute ("DataRate", DataRateValue (DataRate ("200KB/s")));
  app = onoff2.Install (terminals.Get (0));
  app.Start (Seconds (1.0));
  app.Stop (Seconds (1.4));
  
  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address ("10.1.1.4"), port)));
  app = sink.Install (terminals.Get (3));
  app.Start (Seconds (0.0));

  //
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Trace output will be sent to the file "csma-bridge.tr"
  //
  AsciiTraceHelper ascii;
  //csma.EnableAsciiAll (ascii.CreateFileStream ("csma-ats.tr"));
  csma.EnableAscii (ascii.CreateFileStream ("csma-ats-port.tr"), switchDevices.Get (3));
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
  gnuHelper.ConfigurePlot ("Source-1-flows-Enqueue",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Packet Byte");
  gnuHelper.PlotProbe (probeType,
                        tracePath,
                        "OutputBytes",
                        "Packet Byte Count",
                        GnuplotAggregator::KEY_BELOW);      
  tracePath = "/NodeList/0/DeviceList/0/TxQueue/Dequeue";
  gnuHelper.ConfigurePlot ("Source-1-flows-Dequeue",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Packet Byte");                          
  gnuHelper.PlotProbe (probeType,
                        tracePath,
                        "OutputBytes",
                        "Packet Byte Count",
                        GnuplotAggregator::KEY_BELOW);
  tracePath = "/NodeList/3/DeviceList/0/MacRx";
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
  tracePath = "/NodeList/4/$ns3::TrafficControlLayer/RootQueueDiscList/*/DequeueTxTime";
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

  tracePath = "/NodeList/4/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/*/QueueDisc/EligibilityTimeScheduler";
  gnuHelper.ConfigurePlot ("ElibilityTimeDequeue",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "Output",
                      "Time",
                      GnuplotAggregator::KEY_BELOW);

  tracePath = "/NodeList/4/$ns3::TrafficControlLayer/RootQueueDiscList/*/QueueDiscClassList/*/QueueDisc/ArrivalTimeScheduler";
  gnuHelper.ConfigurePlot ("ArrivalTimeToScheduler",
                            "Packet arrival time",
                            "Time (seconds)",
                            "Time");  
                                                  
  gnuHelper.PlotProbe (probeType,
                      tracePath,
                      "Output",
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


