/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
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
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef ADVERSARIAL_GENERATION_HELPER_H
#define ADVERSARIAL_GENERATION_HELPER_H

#include <stdint.h>
#include <string>
#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/adversarial-generation.h"

namespace ns3 {

class DataRate;

/**
 * \ingroup onoff
 * \brief A helper to make it easier to instantiate an ns3::OnOffApplication 
 * on a set of nodes.
 */
class AdversarialGenerationHelper
{
public:
  /**
   * Create an OnOffHelper to make it easier to work with OnOffApplications
   *
   * \param protocol the name of the protocol to use to send traffic
   *        by the applications. This string identifies the socket
   *        factory type used to create sockets for the applications.
   *        A typical value would be ns3::UdpSocketFactory.
   * \param address the address of the remote node to send traffic
   *        to.
   */
  AdversarialGenerationHelper (std::string protocol, Address address);

  /**
   * Helper function used to set the underlying application attributes.
   *
   * \param name the name of the application attribute to set
   * \param value the value of the application attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Helper function to set a constant rate source.  Equivalent to
   * setting the attributes OnTime to constant 1000 seconds, OffTime to 
   * constant 0 seconds, and the DataRate and PacketSize set accordingly
   *
   * \param dataRate DataRate object for the sending rate
   * \param packetSize size in bytes of the packet payloads generated
   */
  void SetConstantRate (DataRate dataRate, uint32_t packetSize = 512);

  /**
   * Install an ns3::OnOffApplication on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param c NodeContainer of the set of nodes on which an OnOffApplication 
   * will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (NodeContainer c) const;

  /**
   * Install an ns3::OnOffApplication on the node configured with all the 
   * attributes set with SetAttribute.
   *
   * \param node The node on which an OnOffApplication will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * Install an ns3::OnOffApplication on the node configured with all the 
   * attributes set with SetAttribute.
   *
   * \param nodeName The node on which an OnOffApplication will be installed.
   * \returns Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (std::string nodeName) const;


private:
  /**
   * Install an ns3::OnOffApplication on the node configured with all the 
   * attributes set with SetAttribute.
   *
   * \param node The node on which an OnOffApplication will be installed.
   * \returns Ptr to the application installed.
   */
  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  ObjectFactory m_factory; //!< Object factory.
};

} // namespace ns3

#endif /* ON_OFF_HELPER_H */

