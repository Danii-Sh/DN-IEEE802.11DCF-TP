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
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

// Default Network Topology
//
// Number of wifi or csma nodes can be increased up to 250
//                          |
//                 Rank 0   |   Rank 1
// -------------------------|----------------------------
//   Wifi 10.1.3.0
//                 AP
//                 *
//    Stations
//  *    *    *    
//  |    |    |         10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;


int main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nCsma = 3;
  uint32_t nWifi = 3;
  bool tracing = true;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.Parse (argc,argv);

  // We create two nodes which will be connected point to point later. One of them will be later on set to the wireless Ap and the other one would be part of the CSMA LAN.
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  // Parameters of the point to point connection is set.
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  // Two actual devices with the mentioned parameters are created to run those two nodes.
  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  // We create/define 4 nodes which will be part of the CSMA LAN. The first one is the second point to point node created in lines above. Then we create three other CSMA nodes. 
  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

 // Parameters of the CSMA LAN is set.
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("10Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (10000)));

 // We connect 4 devices defined earlier and name them CSMA devices.  
  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  // We create three nodes which will be WiFi stations.
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);

 // We create one wifi access point which is the first node of the point to pint network as well.
  NodeContainer wifiApNode = p2pNodes.Get (0);

  // Then we set the parameters of the WiFi network.
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  // Mobility setting for the WiFis. we consider all static however in this project.
  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (5.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));


  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  mobility.Install (wifiStaNodes);

 
  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

 // define IP address for diffent nodes 
 Ipv4AddressHelper address;

 // using the following command, two IP addresses (10.1.1.1 and 10.1.1.2) are set for the P2P nodes.
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  // using the following command, four IP addresses (10.1.2.1 to 10.1.2.4) are set for the CSMA nodes. We notice that one CSMA node which is also a P2P node has two IP addresses.
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

 // using the following command, three wifi stations get IP addresses (10.1.3.1 to 10.1.3.3) and the wifi access point get IP address 10.1.3.4. We notice that WiFi access point is also a P2P node and has another IP addresses as well (10.1.1.1).
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer staInterfaces;
  staInterfaces =  address.Assign (staDevices);

  Ipv4InterfaceContainer apInterface;
  apInterface = address.Assign (apDevices);

////////////// UDP Echo Traffic /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// We define a UDP echo traffic flow between third WiFi station and fourt CSMA node. First, we create a server which listens on port 9. We define a UDP traffic flow between third WiFi station and fourt CSMA node.

  // First, we create a server which listent on port 9.
  UdpEchoServerHelper echoServer1 (9);

 // Using the following commands, the frouth CSMA node becomes the server of the UDP. It receives packets from the client and then sends them back to the cleint again.
  ApplicationContainer serverApps1 = echoServer1.Install (csmaNodes.Get (3));
  serverApps1.Start (Seconds (1.0));
  serverApps1.Stop (Seconds (2.0));

  // We create a UDP client which sends packets to the fourtj CMSA node on port 9.
  UdpEchoClientHelper echoClient1 (csmaInterfaces.GetAddress (3), 9);
  //  We set the attribute of this clent. It sends 10 packet with size 1024 bytes every one second.
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (10)) ;
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

// We then attach the created client to the third WiFi station. The cleint start sending paclet towards the server at time 1 and stop at time 2.
  ApplicationContainer clientApps1 = 
    echoClient1.Install (wifiStaNodes.Get (2));
  clientApps1.Start (Seconds (1.0));
  clientApps1.Stop (Seconds (2));

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (2));

  // Using these lines, we create a file named results to log the operations.
  if (tracing == true)
    {
        AsciiTraceHelper ascii;
        csma.EnableAsciiAll (ascii.CreateFileStream ("results.tr"));
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
