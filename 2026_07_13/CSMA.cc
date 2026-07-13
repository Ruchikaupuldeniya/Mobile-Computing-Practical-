#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("CsmaExercise");

int main(int argc, char *argv[])
{

LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
LogComponentEnable("UdpServer", LOG_LEVEL_INFO);

NodeContainer nodes;
nodes.Create(4);


CsmaHelper csma;
csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
csma.SetChannelAttribute("Delay", TimeValue(MicroSeconds(5)));

NetDeviceContainer devices;
devices = csma.Install(nodes);


InternetStackHelper internet;
internet.Install(nodes);


Ipv4AddressHelper address;
address.SetBase("10.1.1.0", "255.255.255.0");

Ipv4InterfaceContainer interfaces;
interfaces = address.Assign(devices);


uint16_t port = 9;

UdpServerHelper server(port);
ApplicationContainer serverApp = server.Install(nodes.Get(3));

serverApp.Start(Seconds(1.0));
serverApp.Stop(Seconds(10.0));


UdpClientHelper client(interfaces.GetAddress(3), port);

client.SetAttribute("MaxPackets", UintegerValue(100));
client.SetAttribute("Interval", TimeValue(Seconds(1.0)));
client.SetAttribute("PacketSize", UintegerValue(1024));

ApplicationContainer clientApp = client.Install(nodes.Get(0));

clientApp.Start(Seconds(2.0));
clientApp.Stop(Seconds(10.0));


csma.EnablePcapAll("csma-network", true);


Simulator::Stop(Seconds(10.0));
Simulator::Run();
Simulator::Destroy();

return 0;
}