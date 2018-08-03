/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Federal University of Uberlandia
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Saulo da Mata <damata.saulo@gmail.com>
 */

#ifndef HTTP_CLIENT_SERVER_HELPER_H_
#define HTTP_CLIENT_SERVER_HELPER_H_

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/http-client.h"
#include "ns3/http-server.h"

namespace ns3 {

class HttpServerHelper
{
public:
  HttpServerHelper (uint16_t port);

  /**
   * Record an attribute to be set in each Application after it is is created.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);


  /**
   * Install an ns3::HttpServerApplication on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param c NodeContainer of the set of nodes on which a HttpServerApplication
   * will be installed.
   */
  ApplicationContainer Install (NodeContainer c);

  /**
   * Install an ns3::HttpServerApplication on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param node The node on which a HttpServerApplication will be installed.
   */
  ApplicationContainer Install (Ptr<Node> node);

private:
  ObjectFactory m_factory;

};


class HttpClientHelper
{

public:
  HttpClientHelper (Ipv4Address ip, uint16_t port);
  HttpClientHelper (Ipv6Address ip, uint16_t port);
  HttpClientHelper (Address ip, uint16_t port);

  /**
   * Record an attribute to be set in each Application after it is is created.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);


  /**
   * Install an ns3::HttpClientApplication on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param c NodeContainer of the set of nodes on which a HttpClientApplication
   * will be installed.
   */
  ApplicationContainer Install (NodeContainer c);

  /**
   * Install an ns3::HttpClientApplication on each node of the input container
   * configured with all the attributes set with SetAttribute.
   *
   * \param node The node on which a HttpClientApplication will be installed.
   */
  ApplicationContainer Install (Ptr<Node> node);

//  Ptr<http::HttpClient> GetClient (void);
//
private:
  ObjectFactory m_factory;
};

} // namespace ns3


#endif /* HTTP_CLIENT_SERVER_HELPER_H_ */
