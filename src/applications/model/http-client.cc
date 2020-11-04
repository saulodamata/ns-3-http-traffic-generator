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


#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/ptr.h"
#include "ns3/ipv4.h"

#include "http-client.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HttpClientApplication");

TypeId
HttpClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::HttpClient")
    .SetParent<Application> ()
    .AddConstructor<HttpClient> ()
    .AddAttribute ("RemoteAddress", "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&HttpClient::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound packets",
                   UintegerValue (80),
                   MakeUintegerAccessor (&HttpClient::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

HttpClient::HttpClient ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_contentLength = 0;
  m_bytesReceived = 0;
  m_numOfInlineObjects = 0;
  m_inlineObjLoaded = 0;

  //Mu and Sigma data was taken from paper "An HTTP Web Traffic Model Based on the
  //Top One Million Visited Web Pages" by Rastin Pries et. al (Table II).
  m_readingTimeStream = CreateObject<LogNormalRandomVariable> ();
  m_readingTimeStream->SetAttribute("Mu", DoubleValue (-0.495204));
  m_readingTimeStream->SetAttribute("Sigma", DoubleValue (2.7731));
}

HttpClient::~HttpClient ()
{
  NS_LOG_FUNCTION (this);
}

void
HttpClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
HttpClient::StartApplication ()
{
  NS_LOG_FUNCTION (this);
  // Create the socket if not already
  if (!m_socket)
    {
      TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");

      m_socket = Socket::CreateSocket (GetNode (), tid);

      // Fatal error if socket type is not NS3_SOCK_STREAM or NS3_SOCK_SEQPACKET
      if (m_socket->GetSocketType () != Socket::NS3_SOCK_STREAM &&
          m_socket->GetSocketType () != Socket::NS3_SOCK_SEQPACKET)
        {
          NS_FATAL_ERROR ("Using HttpClient with an incompatible socket type. "
                          "HttpClient requires SOCK_STREAM or SOCK_SEQPACKET. "
                          "In other words, use TCP instead of UDP.");
        }

      if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
        {
          m_socket->Bind ();
          m_socket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
      else if (Ipv6Address::IsMatchingType(m_peerAddress) == true)
        {
          m_socket->Bind6 ();
          m_socket->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom(m_peerAddress), m_peerPort));
        }

    }

  m_socket->SetConnectCallback (MakeCallback (&HttpClient::ConnectionSucceeded, this),
                                MakeCallback (&HttpClient::ConnectionFailed, this));
}

void
HttpClient::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      m_socket->Close ();
    }
  else
    {
      NS_LOG_WARN ("HttpClientApplication found null socket to close in StopApplication");
    }
}


void
HttpClient::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ipv4Address temp = socket->GetNode()->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
  m_clientAddress = temp;
  NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> Server accepted connection request!");
  socket->SetRecvCallback (MakeCallback (&HttpClient::HandleReceive, this));
  SendRequest(socket, "main/object");
}

void
HttpClient::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_LOG_ERROR ("HttpClient >> Server did not accepted connection request!");
}

void
HttpClient::SendRequest(Ptr<Socket> socket, string url)
{
  NS_LOG_FUNCTION (this);

  //Setting request message
  m_httpHeader.SetRequest(true);
  m_httpHeader.SetMethod("GET");
  m_httpHeader.SetUrl(url);
  m_httpHeader.SetVersion("HTTP/1.1");


  Ptr<Packet> packet = Create<Packet> ();
  packet->AddHeader (m_httpHeader);
  NS_LOG_INFO("HttpClient (" << m_clientAddress << ") >> Sending request for "
              << url << " to server (" << Ipv4Address::ConvertFrom (m_peerAddress) << ").");
  socket->Send(packet);
}


void
HttpClient::HandleReceive (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  double readingTime;
  HttpHeader httpHeaderIn;
  Ptr<Packet> packet = socket->Recv();

  packet->PeekHeader(httpHeaderIn);

  string statusCode = httpHeaderIn.GetStatusCode();

  uint32_t bytesReceived = packet->GetSize ();

  if (statusCode == "200")
    {
      m_contentType = httpHeaderIn.GetHeaderField("ContentType");
      m_contentLength = atoi(httpHeaderIn.GetHeaderField("ContentLength").c_str());

      m_bytesReceived = bytesReceived - httpHeaderIn.GetSerializedSize();

      if(m_contentType == "main/object")
        {
          m_numOfInlineObjects = atoi(httpHeaderIn.GetHeaderField("NumOfInlineObjects").c_str());
        }

      if(m_bytesReceived == m_contentLength)
        {
    	  NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> " << m_contentType << ": "
    	                << m_bytesReceived << " bytes of " << m_contentLength << " received.");
          m_contentLength = 0;

          if(m_contentType == "main/object")
            {
              NS_LOG_INFO ("HttpClient (" << m_clientAddress << ") >> " << m_contentType <<
                           " successfully received. There are " << m_numOfInlineObjects << " inline objects to request.");
              NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> Requesting inline/object 1...");
              m_inlineObjLoaded = 0;
              SendRequest(socket, "inline/object");
            }
          else
            {
              m_inlineObjLoaded++;
              if(m_inlineObjLoaded < m_numOfInlineObjects)
                {
                  NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> Requesting inline/object "
                                << m_inlineObjLoaded + 1 << "...");
                  SendRequest(socket, "inline/object");
                }
              else
                {
                  readingTime = m_readingTimeStream->GetValue();
                  //Limiting reading time to 10000 seconds according to paper "An HTTP Web Traffic
                  //Model Based on the Top One Million Visited Web Pages" by Rastin Pries et. al (Table II).
                  if(readingTime > 10000)
                    {
                      readingTime = 10000;
                    }

                  NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> Reading time: " << readingTime << " seconds.");
                  Simulator::Schedule (Seconds(readingTime), &HttpClient::SendRequest, this, socket, "main/object");
                }
            }
        }
      else
        {
          NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> " << m_contentType << ": "
                        << m_bytesReceived << " bytes of " << m_contentLength << " received.");
        }
    }
  else
    {
      m_bytesReceived += bytesReceived;

      if(m_bytesReceived == m_contentLength)
        {
    	  NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> " << m_contentType << ": "
    	                << m_bytesReceived << " bytes of " << m_contentLength << " received.");
          m_contentLength = 0;

          if(m_contentType == "main/object")
            {
              NS_LOG_INFO ("HttpClient (" << m_clientAddress << ") >> " << m_contentType <<
                           " successfully received. There are " << m_numOfInlineObjects << " inline objects to request.");
              NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> Requesting inline/object 1...");
              m_inlineObjLoaded = 0;
              SendRequest(socket, "inline/object");
            }
          else
            {
              m_inlineObjLoaded++;
              if(m_inlineObjLoaded < m_numOfInlineObjects)
                {
                  NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> " << m_contentType << " " << m_inlineObjLoaded <<
                                " of " << m_numOfInlineObjects << " successfully received."
                                "\nHttpClient >> Requesting inline/object " << m_inlineObjLoaded + 1);
                  SendRequest(socket, "inline/object");
                }
              else
                {
                  NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> " << m_contentType <<
                                " " << m_inlineObjLoaded << " of " << m_numOfInlineObjects << " successfully received.");
                  readingTime = m_readingTimeStream->GetValue();
                  //Limiting reading time to 10000 seconds according to paper "An HTTP Web Traffic
                  //Model Based on the Top One Million Visited Web Pages" by Rastin Pries et. al (Table II).
                  if(readingTime > 10000)
                    {
                      readingTime = 10000;
                    }

                  NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> Reading time: " << readingTime << " seconds.");
                  Simulator::Schedule (Seconds(readingTime), &HttpClient::SendRequest, this, socket, "main/object");
                }
            }
        }
      else
        {
          NS_LOG_DEBUG ("HttpClient (" << m_clientAddress << ") >> " << m_contentType << ": " <<
                        m_bytesReceived << " bytes of " << m_contentLength << " received.");
        }
    }
}

}



