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

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/random-variable-stream.h"
#include "ns3/double.h"
#include "ns3/http-header.h"

using namespace std;
namespace ns3 {

/**
 * \ingroup applications
 * \defgroup http HttpClientApplication
 *
 * This is the client side of a HTTP Traffic Generator. The client
 * establishes a TCP connection with the server and sends a request
 * for the main object of a given web page. When client gets the main
 * object, it process the message and start to request the inline objects
 * of the given web page. After receiving all inline objects, the client
 * waits an interval (reading time) before it requests a new main object
 * of a new web page. The implementation of this application is simplistic
 * and it does not support pipelining in this current version. The
 * model used is based on the distributions indicated in the paper
 * "An HTTP Web Traffic Model Based on the Top One Million Visited
 * Web Pages" by Rastin Pries et. al. This simplistic approach was
 * taken since this traffic generator was developed primarily to help
 * users evaluate their proposed algorithm in other modules of NS-3.
 * To allow deeper studies about the HTTP Protocol it needs some improvements.
 */
class HttpClient : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return type ID
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor.
   */
  HttpClient ();

  /**
   * \brief Destructor.
   */
  virtual ~HttpClient ();

protected:
  /**
   * \brief Dispose this object;
   */
  virtual void DoDispose (void);

private:
  /**
   * \brief Start the application.
   */
  virtual void StartApplication (void);

  /**
   * \brief Stop the application.
   */
  virtual void StopApplication (void);


  void ConnectionSucceeded (Ptr<Socket> socket);
  void ConnectionFailed (Ptr<Socket> socket);

  /**
   * \brief Send the request to server side.
   * \param socket socket that sends requests.
   * \param url URL of the object requested.
   */
  void SendRequest (Ptr<Socket> socket, string url);

  /**
   * \brief Receive method.
   * \param socket socket that receives packets from server.
   */
  void HandleReceive (Ptr<Socket> socket);

  /**
   * \brief Local socket.
   */
  Ptr<Socket> m_socket;

  /**
   * \brief Address of the server.
   */
  Address m_peerAddress;

  /**
   * \brief Remote port in the server.
   */
  uint16_t m_peerPort;

  /**
   * \brief HTTP Header.
   */
  HttpHeader m_httpHeader;

  /**
   * \brief Content-Length header line.
   */
  uint32_t m_contentLength;

  /**
   * \brief Content-Type header line.
   */
  string m_contentType;

  /**
   * \brief Number-of-Inline-Objects header line.
   */
  uint32_t m_numOfInlineObjects;

  /**
   * \brief Number of bytes received from server.
   */
  uint32_t m_bytesReceived;

  /**
   * \brief Number of inline objects already loaded.
   */
  uint32_t m_inlineObjLoaded;

  /**
   * \brief Random Variable Stream for reading time.
   */
  Ptr<LogNormalRandomVariable> m_readingTimeStream;

  /**
   * \brief client Address.
   */
  Ipv4Address m_clientAddress;
};

}

#endif /* HTTP_CLIENT_H_ */
