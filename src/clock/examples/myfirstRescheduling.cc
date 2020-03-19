/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/local-clock.h"
#include "ns3/perfect-clock-model-impl.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

void setClock (Ptr<LocalClock> clock, double freq)
{
  NS_LOG_DEBUG ("Calling function set clock");
  Ptr<PerfectClockModelImpl> clockImpl = CreateObject <PerfectClockModelImpl> ();
  clockImpl -> SetAttribute ("Frequency", DoubleValue (freq));
  clock -> SetClock (clockImpl);
}
int
main (int argc, char *argv[])
{

  CommandLine cmd;
  cmd.Parse (argc, argv);

  //Set LocalTime Simulator Impl
  GlobalValue::Bind ("SimulatorImplementationType", 
                     StringValue ("ns3::LocalTimeSimulatorImpl"));
  
  Time::SetResolution (Time::NS);
  
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  //LogComponentEnable ("PerfectClockModelImpl",LOG_LEVEL_INFO);


  NodeContainer nodes;
  nodes.Create (2);

  //Aggregate clock 

  Ptr<PerfectClockModelImpl> clockImpl0 = CreateObject <PerfectClockModelImpl> ();
  Ptr<PerfectClockModelImpl> clockImpl1 = CreateObject <PerfectClockModelImpl> ();
  clockImpl0 -> SetAttribute ("Frequency", DoubleValue (2));

  clockImpl1 -> SetAttribute ("Frequency", DoubleValue (1));

  //Config::SetDefault ("ns3::LocalClock::ClockModelImpl", PointerValue (clockImpl0));
  
  Ptr<LocalClock> clock0 = CreateObject<LocalClock> ();
  Ptr<LocalClock> clock1 = CreateObject<LocalClock> ();

  clock0 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl0));
  clock1 -> SetAttribute ("ClockModelImpl", PointerValue (clockImpl1));

  Ptr<Node> n1 = nodes.Get (0);
  Ptr<Node> n2 = nodes.Get (1); 

  n1 -> AggregateObject (clock0);
  n2 -> AggregateObject (clock1);


  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (2.0));
  serverApps.Stop (Seconds (500.0));

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (5.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (500.0));

  UintegerValue idNode;
  n1 ->GetAttribute ("Id", idNode);
  uint32_t num = idNode.Get ();
  std::cout << "ID\n" << num;  
  Simulator::ScheduleWithContext (num, Seconds (9.0), &setClock, clock0, 2.0);
  
  Simulator::Run ();
  Simulator::Destroy ();
}

