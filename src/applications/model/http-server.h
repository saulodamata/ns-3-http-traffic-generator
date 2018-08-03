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

#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include "ns3/application.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"
#include "ns3/http-header.h"
#include "ns3/double.h"

using namespace std;

namespace ns3 {

/**
 * \ingroup applications
 * \defgroup http HttpServerApplication
 *
 * This is the server side of a HTTP Traffic Generator. The server
 * establishes a TCP connection with the client and waits for the
 * object requests. The implementation of this application is simplistic
 * and it does not support pipelining in this current version. The
 * model used is based on the distributions indicated in the paper
 * "An HTTP Web Traffic Model Based on the Top One Million Visited
 * Web Pages" by Rastin Pries et. al. This simplistic approach was
 * taken since this traffic generator was developed primarily to help
 * users evaluate their proposed algorithm in other modules of NS-3.
 * To allow deeper studies about the HTTP Protocol it needs some improvements.
 */
class HttpServer : public Application
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
  HttpServer ();

  /**
   * \brief Destructor.
   */
  virtual ~HttpServer ();

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
  virtual void StopApplication (void);            // Called at time specified by Stop

  /**
   * \brief Processes the request of client to establish a TCP connection.
   * \param socket socket that receives the TCP request for connection.
   */
  bool HandleRequest (Ptr<Socket> s, const Address& address);

  /**
   * \brief Handle the acceptance or denial of the TCP connection.
   * \param socket socket for the TCP connection.
   * \param address address of the client
   */
  void HandleAccept (Ptr<Socket> s, const Address& address);

  /**
   * \brief Receive method.
   * \param socket socket that receives packets from client.
   */
  void HandleReceive (Ptr<Socket> s);

  /**
   * \brief Local socket.
   */
  Ptr<Socket> m_socket;

  /**
   * \brief Local port.
   */
  uint16_t m_port;
};


}
#endif /* HTTP_SERVER_H_ */
