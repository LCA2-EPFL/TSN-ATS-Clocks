/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
//
// Copyright (c) 2006 Georgia Tech Research Corporation
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: George F. Riley<riley@ece.gatech.edu>
//

// ns3 - On/Off Data Source Application class
// George F. Riley, Georgia Tech, Spring 2007
// Adapted from ApplicationOnOff in GTNetS.

#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "adversarial-generation.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/clock-module.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("AdversarialGeneration");

NS_OBJECT_ENSURE_REGISTERED (AdversarialGeneration);

TypeId
AdversarialGeneration::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AdversarialGeneration")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<AdversarialGeneration> ()
    .AddAttribute ("DataRate", "The data rate in on state.",
                   DataRateValue (DataRate ("500kb/s")),
                   MakeDataRateAccessor (&AdversarialGeneration::m_cbrRate),
                   MakeDataRateChecker ())
    .AddAttribute ("PacketSize", "The size of packets sent in on state",
                   UintegerValue (512),
                   MakeUintegerAccessor (&AdversarialGeneration::m_pktSize),
                   MakeUintegerChecker<uint32_t> (1))
    .AddAttribute ("Remote", "The address of the destination",
                   AddressValue (),
                   MakeAddressAccessor (&AdversarialGeneration::m_peer),
                   MakeAddressChecker ())
   .AddAttribute  ("Xjvalue", "Time at which first packet is sent",
                    TimeValue (MicroSeconds (1)),
                    MakeTimeAccessor (&AdversarialGeneration::m_initTime),
                    MakeTimeChecker ())
    .AddAttribute  ("Period", "Period of the app",
                    TimeValue (MicroSeconds (30)),
                    MakeTimeAccessor (&AdversarialGeneration::m_period),
                    MakeTimeChecker ())                
    .AddAttribute ("MaxBytes", 
                   "The total number of bytes to send. Once these bytes are sent, "
                   "no packet is sent again, even in on state. The value zero means "
                   "that there is no limit.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&AdversarialGeneration::m_maxBytes),
                   MakeUintegerChecker<uint64_t> ())
    .AddAttribute ("Protocol", "The type of protocol to use. This should be "
                   "a subclass of ns3::SocketFactory",
                   TypeIdValue (UdpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&AdversarialGeneration::m_tid),
                   // This should check for SocketFactory as a parent
                   MakeTypeIdChecker ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&AdversarialGeneration::m_txTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("TxWithAddresses", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&AdversarialGeneration::m_txTraceWithAddresses),
                     "ns3::Packet::TwoAddressTracedCallback")
    .AddTraceSource ("TXSendGlobal", "Time a packet is sent in global time",
                     MakeTraceSourceAccessor (&AdversarialGeneration::m_txTimeGlobal),
                     "ns3::Time::TracedCallback")
    .AddTraceSource ("TXSendLocal", "Time a packet is sent in global time",
                     MakeTraceSourceAccessor (&AdversarialGeneration::m_txTimeLocal),
                     "ns3::Time::TracedCallback")
  ;
  return tid;
}


AdversarialGeneration::AdversarialGeneration ()
  : m_socket (0),
    m_connected (false),
    m_residualBits (0),
    m_lastStartTime (Seconds (0)),
    m_totBytes (0)
{
  NS_LOG_FUNCTION (this);
  m_stop = false;
}

AdversarialGeneration::~AdversarialGeneration()
{
  NS_LOG_FUNCTION (this);
}

void 
AdversarialGeneration::SetMaxBytes (uint64_t maxBytes)
{
  NS_LOG_FUNCTION (this << maxBytes);
  m_maxBytes = maxBytes;
}

Ptr<Socket>
AdversarialGeneration::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}


void
AdversarialGeneration::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  // chain up
  Application::DoDispose ();
}

// Application Methods
void AdversarialGeneration::StartApplication () // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);

  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);
      if (Inet6SocketAddress::IsMatchingType (m_peer))
        {
          if (m_socket->Bind6 () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
        }
      else if (InetSocketAddress::IsMatchingType (m_peer) ||
               PacketSocketAddress::IsMatchingType (m_peer))
        {
          if (m_socket->Bind () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
        }
      m_socket->Connect (m_peer);
      m_socket->SetAllowBroadcast (true);
      m_socket->ShutdownRecv ();

      m_socket->SetConnectCallback (
        MakeCallback (&AdversarialGeneration::ConnectionSucceeded, this),
        MakeCallback (&AdversarialGeneration::ConnectionFailed, this));
    }
  m_cbrRateFailSafe = m_cbrRate;

  // Insure no pending event
  // If we are not yet connected, there is nothing to do here
  // The ConnectionComplete upcall will start timers at that time
  //if (!m_connected) return;
  ScheduleStartEvent ();
}

void AdversarialGeneration::StopApplication () // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);

  CancelEvents ();
  if(m_socket != 0)
    {
      m_socket->Close ();
      m_stop = true;
    }
  else
    {
      NS_LOG_WARN ("OnOffApplication found null socket to close in StopApplication");
    }
}


// Private helpers
void AdversarialGeneration::ScheduleNextTx ()
{
  NS_LOG_FUNCTION (this);

  if (m_maxBytes == 0 || m_totBytes < m_maxBytes)
    {
      uint32_t bits = m_pktSize * 8 - m_residualBits;
      NS_LOG_LOGIC ("bits = " << bits);
      Time nextTime (Seconds (bits /
                              static_cast<double>(m_cbrRate.GetBitRate ()))); // Time till next packet
      NS_LOG_LOGIC ("nextTime = " << nextTime);
      m_sendEvent = Simulator::Schedule (nextTime,
                                         &AdversarialGeneration::SendPacket, this);
    }
  else
    { // All done, cancel any pending events
      StopApplication ();
    }
}

void AdversarialGeneration::CancelEvents ()
{
  NS_LOG_FUNCTION (this);

  if (m_sendEvent.IsRunning () && m_cbrRateFailSafe == m_cbrRate )
    { // Cancel the pending send packet event
      // Calculate residual bits since last packet sent
      Time delta (Simulator::Now () - m_lastStartTime);
      int64x64_t bits = delta.To (Time::S) * m_cbrRate.GetBitRate ();
      m_residualBits += bits.GetHigh ();
    }
  m_cbrRateFailSafe = m_cbrRate;
  Simulator::Cancel (m_sendEvent);
  Simulator::Cancel (m_startStopEvent);
}

void AdversarialGeneration::ScheduleStartEvent ()
{  // Schedules the event to start sending data (switch to the "On" state)
  NS_LOG_FUNCTION (this);

  if (m_stop == false)
  {
    //Do this to make it easier to know times. m_init is global time
    //Ptr<Node> node = GetNode ();
    //Time initLocalTime = node->GetObject<LocalClock> ()->GlobalToLocalAbs (m_initTime);

    //NS_LOG_DEBUG ("Init time local : " << initLocalTime << "Init time global : " << m_initTime);
    //Simulator::Schedule (initLocalTime, &AdversarialGeneration::ScheduleBurst, this);
    ScheduleBurst (); 
  }
}

void
AdversarialGeneration::ScheduleBurst()
{
  NS_LOG_FUNCTION (this);
  if (m_stop == false)
  {
    SendPacket ();
    AdversarialGeneration::ScheduleNextTx ();
    Simulator::Schedule (m_period, &AdversarialGeneration::ScheduleBurst, this);
  } 
}

void 
AdversarialGeneration::SendPacket ()
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> packet = Create<Packet> (m_pktSize);
  m_txTrace (packet);
  m_txTimeGlobal (Simulator::Now ());
  NS_LOG_DEBUG ("Global " << Simulator::Now ());
  
  Ptr<Node> node = GetNode ();
  Time time = node->GetObject<LocalClock> ()->GetLocalTime ();
  m_txTimeLocal (time);
  NS_LOG_DEBUG ("Local " << time);

  NS_LOG_LOGIC ("First packet of the period send at local time: " << time
    << "Global time : " << Simulator::Now ());

  m_socket->Send (packet);
  m_totBytes += m_pktSize;
  Address localAddress;
  m_socket->GetSockName (localAddress);
  if (InetSocketAddress::IsMatchingType (m_peer))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds ()
                   << "s on-off application sent "
                   <<  packet->GetSize () << " bytes to "
                   << InetSocketAddress::ConvertFrom(m_peer).GetIpv4 ()
                   << " port " << InetSocketAddress::ConvertFrom (m_peer).GetPort ()
                   << " total Tx " << m_totBytes << " bytes");
      m_txTraceWithAddresses (packet, localAddress, InetSocketAddress::ConvertFrom (m_peer));
    }
  else if (Inet6SocketAddress::IsMatchingType (m_peer))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds ()
                   << "s on-off application sent "
                   <<  packet->GetSize () << " bytes to "
                   << Inet6SocketAddress::ConvertFrom(m_peer).GetIpv6 ()
                   << " port " << Inet6SocketAddress::ConvertFrom (m_peer).GetPort ()
                   << " total Tx " << m_totBytes << " bytes");
      m_txTraceWithAddresses (packet, localAddress, Inet6SocketAddress::ConvertFrom(m_peer));
    }
  m_lastStartTime = Simulator::Now ();
  m_residualBits = 0;
}


void AdversarialGeneration::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  m_connected = true;
}

void AdversarialGeneration::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}


} // Namespace ns3
