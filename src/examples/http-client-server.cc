/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 Federal University of Uberlandia
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


#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-helper.h"

//#include "ns3/lte-helper.h"
//#include "ns3/epc-helper.h"
//#include "ns3/network-module.h"
//#include "ns3/ipv4-global-routing-helper.h"
//#include "ns3/mobility-module.h"
//#include "ns3/lte-module.h"
//#include "ns3/config-store.h"
//#include "ns3/random-variable-stream.h"
//#include "ns3/buildings-helper.h"
//#include "ns3/flow-monitor-helper.h"
//#include "ns3/evalvid-client-server-helper.h"
//#include <map>
//#include "ns3/output-stream-wrapper.h"


using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("HttpClientServerExample");


int
main (int argc, char *argv[])
{
  //Enabling logging
  LogComponentEnable ("HttpClientServerExample", LOG_LEVEL_INFO);
  LogComponentEnable ("HttpClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("HttpServerApplication", LOG_LEVEL_INFO);


  NS_LOG_INFO ("Creating nodes...");
  NodeContainer n;
  n.Create (2);

  InternetStackHelper internetStack;
  internetStack.Install (n);

  NS_LOG_INFO ("Creating point to point link...");
  // Create p2p links
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.000001)));
  NetDeviceContainer c = p2ph.Install (n);

  NS_LOG_INFO ("Assigning IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (c);


  NS_LOG_INFO ("Creating Applications.");
  uint16_t httpServerPort = 80;
  ApplicationContainer httpServerApps;
  ApplicationContainer httpClientApps;

  HttpServerHelper httpServer (httpServerPort);
  httpServerApps.Add (httpServer.Install (n.Get (0)));

  HttpClientHelper httpClient (i.GetAddress (0), httpServerPort);
  httpClientApps.Add (httpClient.Install (n.Get (1)));


  httpServerApps.Start (Seconds(1.0));
  httpServerApps.Stop (Seconds(10.0));

  httpClientApps.Start (Seconds(2.0));
  httpClientApps.Stop (Seconds(10.0));


  Simulator::Stop(Seconds(10.0));


  NS_LOG_INFO("Starting Simulation...");
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO("\ndone!");
  return 0;

}
