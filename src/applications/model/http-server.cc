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
#include "ns3/pointer.h"
#include "ns3/packet.h"
#include "ns3/tcp-socket-base.h"
#include "ns3/drop-tail-queue.h"
#include "http-server.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HttpServerApplication");

TypeId
HttpServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::HttpServer")
    .SetParent<Application> ()
    .AddConstructor<HttpServer> ()
    .AddAttribute ("Port",
                   "Port on which we listen for incoming packets.",
                   UintegerValue (80),
                   MakeUintegerAccessor (&HttpServer::m_port),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

HttpServer::HttpServer ()
{
  NS_LOG_FUNCTION (this);

  m_socket = 0;
}

HttpServer::~HttpServer ()
{
  NS_LOG_FUNCTION (this);
}

void
HttpServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void HttpServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (!m_socket)
    {
      TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");

      m_socket = Socket::CreateSocket (GetNode (), tid);

      // Fatal error if socket type is not NS3_SOCK_STREAM or NS3_SOCK_SEQPACKET
      if (m_socket->GetSocketType () != Socket::NS3_SOCK_STREAM &&
          m_socket->GetSocketType () != Socket::NS3_SOCK_SEQPACKET)
        {
          NS_FATAL_ERROR ("Using HttpServer with an incompatible socket type. "
                          "HttpServer requires SOCK_STREAM or SOCK_SEQPACKET. "
                          "In other words, use TCP instead of UDP.");
        }

      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (),
                                                         m_port);
      m_socket->Bind (local);
      m_socket->Listen ();
      m_socket->SetAcceptCallback (MakeCallback (&HttpServer::HandleRequest, this),
                                   MakeCallback (&HttpServer::HandleAccept, this));
    }
}

void HttpServer::StopApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      m_socket->Close ();
    }
  else
    {
      NS_LOG_WARN ("HttpServerApplication found null socket to close in StopApplication");
    }
}


bool
HttpServer::HandleRequest (Ptr<Socket> s, const Address& address)
{
  NS_LOG_FUNCTION (this << s << address);
  NS_LOG_DEBUG ("HttpServer >> Request for connection from " << InetSocketAddress::ConvertFrom (address).GetIpv4 () << " received.");
  return true;
}


void
HttpServer::HandleAccept (Ptr<Socket> s, const Address& address)
{
  NS_LOG_FUNCTION (this << s << address);

  NS_LOG_DEBUG ("HttpServer >> Connection with Client (" <<  InetSocketAddress::ConvertFrom (address).GetIpv4 () << ") successfully established!");
  s->SetRecvCallback (MakeCallback (&HttpServer::HandleReceive, this));
}


void
HttpServer::HandleReceive (Ptr<Socket> s)
{
  NS_LOG_FUNCTION (this << s);

  HttpHeader httpHeaderIn;
  Ptr<Packet> packet = s->Recv();

  // Getting TCP Sending Buffer Size.
  Ptr<TcpSocketBase> tcp = CreateObject<TcpSocketBase> ();
  NS_ASSERT (tcp != 0);
  UintegerValue bufSizeValue;
  tcp->GetAttribute ("SndBufSize", bufSizeValue);
  uint32_t tcpBufSize = bufSizeValue.Get();

  packet->PeekHeader(httpHeaderIn);

  string url = httpHeaderIn.GetUrl();

  NS_LOG_INFO ("HttpServer >> Client requesting a " + url);

  if (url == "main/object")
    {
      //Scale, Shape and Mean data was taken from paper "An HTTP Web Traffic Model Based on the
      //Top One Million Visited Web Pages" by Rastin Pries et. al (Table II).
      Ptr<WeibullRandomVariable> mainObjectSizeStream = CreateObject<WeibullRandomVariable> ();
      mainObjectSizeStream->SetAttribute("Scale", DoubleValue (19104.9));
      mainObjectSizeStream->SetAttribute("Shape", DoubleValue (0.771807));
      uint32_t mainObjectSize = mainObjectSizeStream->GetInteger();

      Ptr<ExponentialRandomVariable> numOfInlineObjStream = CreateObject<ExponentialRandomVariable> ();
      numOfInlineObjStream->SetAttribute ("Mean", DoubleValue (31.9291));
      uint32_t numOfInlineObj = numOfInlineObjStream->GetInteger();

      //Setting response
      HttpHeader httpHeaderOut;
      httpHeaderOut.SetRequest(false);
      httpHeaderOut.SetVersion("HTTP/1.1");
      httpHeaderOut.SetStatusCode("200");
      httpHeaderOut.SetPhrase("OK");
      httpHeaderOut.SetHeaderField("ContentLength", mainObjectSize);
      httpHeaderOut.SetHeaderField("ContentType", "main/object");
      httpHeaderOut.SetHeaderField("NumOfInlineObjects", numOfInlineObj);

      //Verifying if the buffer can store this packet size
      if (mainObjectSize > tcpBufSize)
        {
          mainObjectSize = tcpBufSize - httpHeaderOut.GetSerializedSize();
          httpHeaderOut.SetHeaderField("ContentLength", mainObjectSize);
        }

      Ptr<Packet> p = Create<Packet> (mainObjectSize);
      p->AddHeader(httpHeaderOut);

      NS_LOG_INFO ("HttpServer >> Sending response to client. Main Object Size ("
                   << mainObjectSize << " bytes). NumOfInlineObjects ("
                   << numOfInlineObj << ").");
      s->Send(p);
    }
  else
    {
      //Mu and Sigma data was taken from paper "An HTTP Web Traffic Model Based on the
      //Top One Million Visited Web Pages" by Rastin Pries et. al (Table II).
      Ptr<LogNormalRandomVariable> logNormal = CreateObject<LogNormalRandomVariable> ();
      logNormal->SetAttribute("Mu", DoubleValue (8.91365));
      logNormal->SetAttribute("Sigma", DoubleValue (1.24816));

      uint32_t inlineObjectSize = logNormal->GetInteger();

      //Setting response
      HttpHeader httpHeaderOut;
      httpHeaderOut.SetRequest(false);
      httpHeaderOut.SetVersion("HTTP/1.1");
      httpHeaderOut.SetStatusCode("200");
      httpHeaderOut.SetPhrase("OK");
      httpHeaderOut.SetHeaderField("ContentLength", inlineObjectSize);
      httpHeaderOut.SetHeaderField("ContentType", "inline/object");
      httpHeaderOut.SetHeaderField("NumOfInlineObjects", 0);

      //Verifying if the buffer can store this packet size
      if (inlineObjectSize > tcpBufSize)
        {
          inlineObjectSize = tcpBufSize - httpHeaderOut.GetSerializedSize();
          httpHeaderOut.SetHeaderField("ContentLength", inlineObjectSize);
        }

      Ptr<Packet> p = Create<Packet> (inlineObjectSize);
      p->AddHeader(httpHeaderOut);

      NS_LOG_INFO ("HttpServer >> Sending response to client. Inline Objectsize ("
                   << inlineObjectSize << " bytes).");
      s->Send(p);
    }
}

}

