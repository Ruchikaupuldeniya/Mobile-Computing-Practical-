#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TcpSimple");

int main(int argc, char *argv[])
{
    LogComponentEnable("TcpSimple", LOG_LEVEL_INFO);
    
    
    LogComponentEnable("BulkSendApplication", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    LogComponentEnable("TcpL4Protocol", LOG_LEVEL_INFO);
    LogComponentEnable("TcpSocketBase", LOG_LEVEL_INFO);

    // Create 2 nodes
    NodeContainer nodes;
    nodes.Create(2);

    // Point-to-point link
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("20ms"));
    pointToPoint.SetQueue("ns3::DropTailQueue",
                          "MaxSize", StringValue("50p"));

    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    // Internet stack
    InternetStackHelper stack;
    stack.Install(nodes);

    // IP addressing
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces;
    interfaces = address.Assign(devices);

    // Receiver (Server)
    PacketSinkHelper sink(
        "ns3::TcpSocketFactory",
        InetSocketAddress(Ipv4Address::GetAny(), 8080));

    ApplicationContainer serverApp = sink.Install(nodes.Get(1));
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(Seconds(10.0));

    // Sender (Client)
    OnOffHelper client(
        "ns3::TcpSocketFactory",
        InetSocketAddress(interfaces.GetAddress(1), 8080));

    client.SetAttribute("DataRate", StringValue("2Mbps"));
    client.SetAttribute("PacketSize", UintegerValue(1024));

    client.SetAttribute(
        "OnTime",
        StringValue("ns3::ConstantRandomVariable[Constant=1]"));

    client.SetAttribute(
        "OffTime",
        StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    ApplicationContainer clientApp = client.Install(nodes.Get(0));
    clientApp.Start(Seconds(2.0));
    clientApp.Stop(Seconds(8.0));

    // Run simulation
    NS_LOG_INFO("Starting TCP simulation...");
    Simulator::Run();
    NS_LOG_INFO("Simulation finished.");

    Simulator::Destroy();
    return 0;
}