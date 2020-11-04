/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020 Saulo da Mata
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


#include "ns3/uinteger.h"
#include "http-client-server-helper.h"

namespace ns3 {

HttpServerHelper::HttpServerHelper (uint16_t port)
{
  m_factory.SetTypeId (HttpServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

void
HttpServerHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
HttpServerHelper::Install (Ptr<Node> node)
{
  ApplicationContainer apps;
  Ptr<HttpServer> server = m_factory.Create<HttpServer> ();
  node->AddApplication (server);
  apps.Add (server);
  return apps;
}

ApplicationContainer
HttpServerHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;

      Ptr<HttpServer> server = m_factory.Create<HttpServer> ();
      node->AddApplication (server);
      apps.Add (server);
    }
  return apps;
}



HttpClientHelper::HttpClientHelper (Address address, uint16_t port)
{
  m_factory.SetTypeId (HttpClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
}

HttpClientHelper::HttpClientHelper (Ipv4Address address, uint16_t port)
{
  m_factory.SetTypeId (HttpClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}

HttpClientHelper::HttpClientHelper (Ipv6Address address, uint16_t port)
{
  m_factory.SetTypeId (HttpClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}


void
HttpClientHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
HttpClientHelper::Install (Ptr<Node> node)
{
  ApplicationContainer apps;
  Ptr<HttpClient> client = m_factory.Create<HttpClient> ();
  node->AddApplication (client);
  apps.Add (client);
  return apps;
}

ApplicationContainer
HttpClientHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;

      Ptr<HttpClient> client = m_factory.Create<HttpClient> ();
      node->AddApplication (client);
      apps.Add (client);
    }
  return apps;
}

} // namespace ns3


