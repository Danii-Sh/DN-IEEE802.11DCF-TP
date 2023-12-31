#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include <cmath>

#define PI 3.14159265

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Project1");

double SimTime = 15.0;
double txStartTime = 0.1;
double txStopTime = 9.0;

void
get_throughput(Ptr<PacketSink> sinkApp)
{
	// Supervising the the throughput to PacketSink each 0.1 sec
	Time t = Simulator::Now();
	std::cout << "current time: \t" << t.GetSeconds() << std::endl;
	
	double bytesTotal = sinkApp->GetTotalRx();
	float throughput = (bytesTotal*8.0)/1000/t.GetSeconds();
	std::cout << "Throughput: " << throughput << std::endl;
	
	//reschedule per 0.1 second
	if(t.GetSeconds() < SimTime)
	{
		Simulator::Schedule(Seconds(0.1), &get_throughput, sinkApp);
	}
}

int
main(int argc, char *argv[])
{
  uint32_t nSta = 1;
  uint32_t cwmin = 1;
  uint32_t cwmax = 1023;
  bool trace = false;

  //add command line parameters for enabling or disabling logging components
  //also for changing number of devices
  CommandLine cmd;
  cmd.AddValue ("nSta", "Number of wifi STA devices", nSta);
  cmd.AddValue ("cwmin", "Minimum contention window size", cwmin);
  cmd.AddValue ("cwmax", "Maximum contention window size", cwmax);
  cmd.AddValue ("trace", "supervising the throughput each 0.1 second", trace);
  cmd.Parse (argc, argv);

  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
  Config::SetDefault("ns3::DcaTxop::MinCw", UintegerValue (cwmin));
  Config::SetDefault("ns3::DcaTxop::MaxCw", UintegerValue (cwmax));
  //Config::Set("/NodeList/0/DeviceList/0/Mac/DcaTxop/MinCw", UintegerValue (cwmin));
  
  //create 'nSta' wireless stations and one AP
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nSta);
  NodeContainer wifiApNode;
  wifiApNode.Create (uint32_t (1));

  //construct wifi configuration and channel models
  //create a channel object and associate it to PHY layer object manager
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  //Speed : 500000 m/s
  //channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel","Speed",StringValue("500000"));
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  //make sure all PHY layer objects created by Yan, complete Phy helper configuration
  phy.SetChannel (channel.Create ());
 
  //now, start focusing on Mac Layer, work with non-Qos MACs
  WifiHelper wifi;
  //wifi.EnableLogComponents();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  StringValue phymode = StringValue ("DsssRate1Mbps");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", phymode,"ControlMode",phymode);
  
  //Next, configure type of MAC, SSID of infrastructure network
  NqosWifiMacHelper macSta, macAp;
  Ssid ssid = Ssid ("project-1");
  macSta.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid),
                                  "Sifs", TimeValue(MicroSeconds(28)),
                                  "Slot", TimeValue(MicroSeconds(50)));
  //After configuring MAC and PHY, now invoke Install to create wifi devices
  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, macSta, wifiStaNodes);

  //configure AP node, changing default Attributes of NqosWifiMacHelper
  macAp.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid),
                                  //"MinCw", UintegerValue (cwmin),
                                  "Sifs", TimeValue(MicroSeconds(28)),
                                  "Slot", TimeValue(MicroSeconds(50)));
  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, macAp, wifiApNode);

  Config::SetDefault ("ns3::WifiRemoteStationManager::MaxSsrc", StringValue("10000"));
  Config::SetDefault ("ns3::WifiRemoteStationManager::MaxSlrc", StringValue("10000"));

  //Set constant position of stations & ap
  MobilityHelper mobility;

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  //locate AP at the center with position (0, 0, 0)
  positionAlloc->Add (Vector (0,0,0));
  //locate N stations as a circle with a center at AP
  //for N stations, set rho as radius and theta
  //then calculate position of each station  
  float rho = 0.5;
  for (uint32_t i = 0; i < nSta; i++)
  {
    double theta = i * 2 * PI / nSta;
    positionAlloc->Add (Vector (rho * cos(theta), rho * sin(theta), 0.0));
  }
  mobility.SetPositionAllocator (positionAlloc);
  //tell MobilityHelper to install the mobility models on STA nodes
  mobility.Install (wifiApNode);
  mobility.Install (wifiStaNodes);

  //intall network protocol stacks
  InternetStackHelper stack;
  stack.Install(wifiApNode);
  stack.Install(wifiStaNodes);

  //assign Ipv4 address, use network "10.1.1.0"
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ApInterface = address.Assign(apDevices);
  Ipv4InterfaceContainer StaInterface =	address.Assign(staDevices);

  //station application, seting UDPSocketFactory as protocol, DataRate 5Mb/s, packtSize 512 bytes
  //these stations begin transmitting data
  for (uint32_t i = 0; i < nSta; i++)
  {
	  OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(ApInterface.GetAddress(0), 9)));
	  onoff.SetConstantRate(DataRate ("800kb/s"), uint32_t(512));
	  ApplicationContainer temp = onoff.Install(wifiStaNodes.Get (i));
	  temp.Start(Seconds(txStartTime));
	  temp.Stop(Seconds(txStopTime));
	}

  //similarly, set AP(receiver) application speed, with its protocol, IP address and Port Number
  //and begin receiving data
  PacketSinkHelper sink ("ns3::UdpSocketFactory", 
  											 Address(InetSocketAddress(ApInterface.GetAddress(0), 9)));
  ApplicationContainer Serverapp = sink.Install(wifiApNode.Get (0));
  Serverapp.Start(Seconds (0.0));

  //enable network routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //run simulator and give throughput
  Ptr<PacketSink> sinkApp = DynamicCast<PacketSink> (Serverapp.Get (0));
  FlowMonitorHelper flowmon;
	Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  //configure tracing for each 0.1 seconds
  if (trace == true)
  {
    Simulator::Schedule(Seconds(0.1), &get_throughput, sinkApp);
  }
  Simulator::Stop(Seconds (SimTime));
  Simulator::Run();

  //monitor->CheckForLostPackets ();
  //Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
  double lastRxTime = txStopTime;
  double firstRxTime = SimTime;

  for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator set = stats.begin(); set != stats.end(); set++) 
  {
    if(lastRxTime < set->second.timeLastRxPacket.GetSeconds())
    {
    	lastRxTime = set->second.timeLastRxPacket.GetSeconds();
    }
    if(firstRxTime > set->second.timeFirstRxPacket.GetSeconds())
    {
    	firstRxTime = set->second.timeFirstRxPacket.GetSeconds();
    }
  }
  
  double totalBytes = sinkApp->GetTotalRx();
  float throughput = totalBytes * 8.0/1000/(lastRxTime - firstRxTime);
  std::cout << "cwmin: " << cwmin << ", cwmax: " << cwmax << ", nSta: " << nSta << std::endl;
  std::cout << "firstRxTime: " << firstRxTime << "sec,\t lastRxTime: " << lastRxTime << "sec" << std::endl;
  std::cout << "throughput:\t" << throughput << "kbps" << std::endl;

  Simulator::Destroy ();
}
