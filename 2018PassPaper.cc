//2021icts69

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("2018pastpaper");

int main(int argc, char *argv[])
{
CommandLine cmd;
cmd.Parse(argc, argv);

Time::SetResolution(Time::NS);

LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);



NodeContainer nodes;
nodes.Create(4);




InternetStackHelper stack;
stack.Install(nodes);



PointToPointHelper dedicated;
dedicated.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
dedicated.SetChannelAttribute("Delay", StringValue("0.1ms"));


NetDeviceContainer d1;
d1 = dedicated.Install(nodes.Get(0), nodes.Get(1));


NetDeviceContainer d2;
d2 = dedicated.Install(nodes.Get(0), nodes.Get(3));



PointToPointHelper shared;
shared.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
shared.SetChannelAttribute("Delay", StringValue("6500ms"));

// Proxy Server <-> Manager PC
NetDeviceContainer d3;
d3 = shared.Install(nodes.Get(1), nodes.Get(2));

// DNS Server <-> Manager PC
NetDeviceContainer d4;
d4 = shared.Install(nodes.Get(3), nodes.Get(2));



Ipv4AddressHelper address;

// Network 1
address.SetBase("10.1.1.0", "255.255.255.0");
Ipv4InterfaceContainer i1 = address.Assign(d1);

// Network 2
address.SetBase("10.1.2.0", "255.255.255.0");
Ipv4InterfaceContainer i2 = address.Assign(d2);

// Network 3
address.SetBase("10.1.3.0", "255.255.255.0");
Ipv4InterfaceContainer i3 = address.Assign(d3);

// Network 4
address.SetBase("10.1.4.0", "255.255.255.0");
Ipv4InterfaceContainer i4 = address.Assign(d4);



UdpEchoServerHelper serverA(9);

ApplicationContainer serverAppsA;
serverAppsA = serverA.Install(nodes.Get(1));

serverAppsA.Start(Seconds(1.0));
serverAppsA.Stop(Seconds(30.0));



UdpEchoServerHelper serverB(9);

ApplicationContainer serverAppsB;
serverAppsB = serverB.Install(nodes.Get(3));

serverAppsB.Start(Seconds(1.0));
serverAppsB.Stop(Seconds(10.0));



UdpEchoClientHelper clientA(i3.GetAddress(0), 9);

clientA.SetAttribute("MaxPackets", UintegerValue(5));
clientA.SetAttribute("Interval", TimeValue(Seconds(1.0)));
clientA.SetAttribute("PacketSize", UintegerValue(1024));

ApplicationContainer clientAppsA;
clientAppsA = clientA.Install(nodes.Get(2));

clientAppsA.Start(Seconds(1.0));
clientAppsA.Stop(Seconds(30.0));



UdpEchoClientHelper clientB(i4.GetAddress(0), 9);

clientB.SetAttribute("MaxPackets", UintegerValue(5));
clientB.SetAttribute("Interval", TimeValue(Seconds(1.0)));
clientB.SetAttribute("PacketSize", UintegerValue(1024));

ApplicationContainer clientAppsB;
clientAppsB = clientB.Install(nodes.Get(2));

clientAppsB.Start(Seconds(1.0));
clientAppsB.Stop(Seconds(10.0));



Ipv4GlobalRoutingHelper::PopulateRoutingTables();



AnimationInterface anim("2018pastpaper.xml");



Simulator::Run();
Simulator::Destroy ();
  return 0;
}
 

