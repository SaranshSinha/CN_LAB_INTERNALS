#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

// Default Network Topology
//
//     10.1.1.0                           30.1.3.0
// m0------m1 -------------- n1 n2 n3 n4 -------------- n5--------N6
// point-to-point            |  |  |  |  point-to-point
//                        ================
//                          LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int main (int argc, char *argv[]){
    bool verbose = true;
    uint32_t nCsma = 2;
    CommandLine cmd;
    cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
    cmd.Parse (argc,argv);

    if (verbose){
        LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
    nCsma = nCsma == 0? 1 : nCsma;

    NodeContainer p2pNodes, p2pNodes1, csmaNodes;
    p2pNodes.Create (3); // created m0 and m1 and n1
    p2pNodes1.Create(3); // created n4 and n5 and n6
    csmaNodes.Add (p2pNodes.Get (2)); // added n1
    csmaNodes.Create (nCsma); // created n2 and n3
    csmaNodes.Add(p2pNodes1.Get(0)); // added n4

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));
    PointToPointHelper pointToPoint1;
    pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint1.SetChannelAttribute ("Delay", StringValue ("2ms"));
    PointToPointHelper pointToPoint2;
    pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint2.SetChannelAttribute ("Delay", StringValue ("2ms"));
    PointToPointHelper pointToPoint3;
    pointToPoint3.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint3.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer p2pDevices, p2pDevices1, csmaDevices, p2pDevices2, p2pDevices3;
    p2pDevices = pointToPoint.Install (p2pNodes.Get(0), p2pNodes.Get(1));
    p2pDevices1 = pointToPoint1.Install (p2pNodes.Get(1), p2pNodes.Get(2));
    csmaDevices = csma.Install (csmaNodes);
    p2pDevices2 = pointToPoint2.Install(p2pNodes1.Get(0),p2pNodes1.Get(1));
    p2pDevices3 = pointToPoint3.Install (p2pNodes1.Get(1), p2pNodes1.Get(2));

    InternetStackHelper stack;
    stack.Install (p2pNodes.Get (0)); // node m0 and m1
    stack.Install (p2pNodes.Get (1)); // node m1
    stack.Install (csmaNodes); // nodes n1,n2, n3, n4
    stack.Install (p2pNodes1.Get(1)); // node n5
    stack.Install (p2pNodes1.Get (2)); // node n6

    Ipv4AddressHelper address;
    Ipv4InterfaceContainer p2pInterfaces, p2pInterfaces1, csmaInterfaces, p2pInterfaces2, p2pInterfaces3;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    p2pInterfaces = address.Assign (p2pDevices);
    p2pInterfaces1 = address.Assign (p2pDevices1);
    address.SetBase ("20.1.2.0", "255.255.255.0");
    csmaInterfaces = address.Assign (csmaDevices);
    address.SetBase ("30.1.3.0", "255.255.255.0");
    p2pInterfaces2 = address.Assign (p2pDevices2);
    p2pInterfaces3 = address.Assign (p2pDevices3);

    UdpEchoServerHelper echoServer (9), echoServer1(10); // 9 and 10 are port numbers

    ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (10.0));
    ApplicationContainer serverApps1 = echoServer1.Install (csmaNodes.Get (nCsma));
    serverApps1.Start (Seconds (1.0));
    serverApps1.Stop (Seconds (10.0));

    UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
    ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (1));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));
    UdpEchoClientHelper echoClient1 (csmaInterfaces.GetAddress (nCsma), 10);
    echoClient1.SetAttribute ("MaxPackets", UintegerValue (2));
    echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient1.SetAttribute ("PacketSize", UintegerValue (512));
    ApplicationContainer clientApps1 = echoClient1.Install (p2pNodes1.Get (1));
    clientApps1.Start (Seconds (2.0));
    clientApps1.Stop (Seconds (10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    pointToPoint.EnablePcapAll ("second");
    csma.EnablePcap ("second", csmaDevices.Get (1), true);
    pointToPoint1.EnablePcapAll ("second");
    //pointToPoint1.EnablePcap ("second_new", p2pNodes1.Get (0), true);

    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}// end of main

/*
At time 2s client sent 1024 bytes to 20.1.2.3 port 9
At time 2s client sent 512 bytes to 20.1.2.3 port 10
At time 2.0038s server received 1024 bytes from 10.1.1.3 port 49153
At time 2.0038s server sent 1024 bytes to 10.1.1.3 port 49153
At time 2.00494s server received 512 bytes from 30.1.3.2 port 49153
At time 2.00494s server sent 512 bytes to 30.1.3.2 port 49153
At time 2.00789s client received 512 bytes from 20.1.2.3 port 10
At time 2.01361s client received 1024 bytes from 20.1.2.3 port 9
At time 3s client sent 512 bytes to 20.1.2.3 port 10
At time 3.00292s server received 512 bytes from 30.1.3.2 port 49153
At time 3.00292s server sent 512 bytes to 30.1.3.2 port 49153
At time 3.00584s client received 512 bytes from 20.1.2.3 port 10

*/
