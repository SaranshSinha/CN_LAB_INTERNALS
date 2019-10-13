#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main(int argc, char *argv[]){
	CommandLine cmd;
	cmd.Parse(argc,argv);

	Time::SetResolution(Time::NS);
	LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);

	NodeContainer nodes;
	nodes.Create(4);

	PointToPointHelper pointToPoint , pointToPoint1,pointToPoint2;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
	pointToPoint1.SetDeviceAttribute("DataRate",StringValue("7Mbps"));
	pointToPoint1.SetChannelAttribute("Delay", StringValue("1ms"));
  pointToPoint2.SetDeviceAttribute("DataRate",StringValue("7Mbps"));
	pointToPoint2.SetChannelAttribute("Delay", StringValue("1ms"));

	NetDeviceContainer devices,devices1,devices2;
	devices = pointToPoint.Install(nodes.Get(0),nodes.Get(1));
	devices1 = pointToPoint1.Install(nodes.Get(2),nodes.Get(1));
  devices2 = pointToPoint2.Install(nodes.Get(3),nodes.Get(1));

	InternetStackHelper stack;
	stack.Install(nodes);

	Ipv4AddressHelper address, address1;
	address.SetBase("10.0.1.0","255.255.255.0");
	address1.SetBase("10.0.2.0", "255.255.255.0");

	Ipv4InterfaceContainer interfaces, interfaces1,interfaces2;
	interfaces = address.Assign(devices);
	interfaces1 = address1.Assign(devices1);
  interfaces2 = address1.Assign(devices2);

	UdpEchoServerHelper echoServer(9), echoServer1(10),echoServer2(11);

	ApplicationContainer serverApps, serverApps1,serverApps2;
	serverApps = echoServer.Install(nodes.Get(1));
	serverApps1 = echoServer1.Install(nodes.Get(1));
  serverApps2 = echoServer2.Install(nodes.Get(1));
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));
	serverApps1.Start(Seconds(10.0));
	serverApps1.Stop(Seconds(20.0));
  serverApps2.Start(Seconds(20.0));
	serverApps2.Stop(Seconds(30.0));

	UdpEchoClientHelper echoClient(interfaces.GetAddress(1),9);
	UdpEchoClientHelper echoClient1(interfaces1.GetAddress(1),10);
  UdpEchoClientHelper echoClient2(interfaces2.GetAddress(1),11);

	echoClient.SetAttribute("MaxPackets",UintegerValue(1));
	echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
	echoClient.SetAttribute("PacketSize",UintegerValue(1024));
	echoClient1.SetAttribute("MaxPackets",UintegerValue(1));
	echoClient1.SetAttribute("Interval", TimeValue(Seconds(1.0)));
	echoClient1.SetAttribute("PacketSize",UintegerValue(1024));
  echoClient2.SetAttribute("MaxPackets",UintegerValue(1));
  echoClient2.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  echoClient2.SetAttribute("PacketSize",UintegerValue(1024));

	ApplicationContainer clientApps, clientApps1,clientApps2;
	clientApps = echoClient.Install(nodes.Get(0));
	clientApps1 = echoClient1.Install(nodes.Get(2));
  clientApps2 = echoClient2.Install(nodes.Get(3));

	clientApps.Start(Seconds(2.0));
	clientApps.Stop(Seconds(10.0));
	clientApps1.Start(Seconds(11.0));
	clientApps1.Stop(Seconds(20.0));
  clientApps2.Start(Seconds(21.0));
  clientApps2.Stop(Seconds(30.0));


	Simulator::Run();
	Simulator::Destroy();
	return 0;
}//end of main

/*
At time 2s client sent 1024 bytes to 10.0.1.2 port 9
At time 2.00369s server received 1024 bytes from 10.0.1.1 port 49153
At time 2.00369s server sent 1024 bytes to 10.0.1.1 port 49153
At time 2.00737s client received 1024 bytes from 10.0.1.2 port 9
At time 11s client sent 1024 bytes to 10.0.2.2 port 10
At time 11.0022s server received 1024 bytes from 10.0.2.1 port 49153
At time 11.0022s server sent 1024 bytes to 10.0.2.1 port 49153
At time 21s client sent 1024 bytes to 10.0.2.4 port 11
At time 21.0022s server received 1024 bytes from 10.0.2.3 port 49153
At time 21.0022s server sent 1024 bytes to 10.0.2.3 port 49153
At time 21.0044s client received 1024 bytes from 10.0.2.4 port 11
*/
