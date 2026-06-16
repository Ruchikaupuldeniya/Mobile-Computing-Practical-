#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/constant-position-mobility-model.h"  //Header of creating a constant position using vector.
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main(int argc, char *argv[]){
	
	CommandLine cmd;
	cmd.Parse(argc,argv);

	Time::SetResolution (Time::NS);
	LogComponentEnable("UdpClient",LOG_LEVEL_INFO);
	LogComponentEnable("UdpServer",LOG_LEVEL_INFO);

	NodeContainer nodes;
	nodes.Create(2);

	//setting the location only
	Vector temp0(1,1,0);   //Creating position for the Node (x,y,z)
	Ptr<Node> n0=nodes.Get(0);  //Get nodes[0] for setting the position in (1,1,0)
	Ptr<ConstantPositionMobilityModel>p0=n0->GetObject<ConstantPositionMobilityModel>();
	p0=CreateObject<ConstantPositionMobilityModel>();
	n0->AggregateObject(p0);
	p0->SetPosition(temp0);

	Vector temp1(2,2,0);
	Ptr<Node> n1=nodes.Get(1);
	Ptr<ConstantPositionMobilityModel>p1=n1->GetObject<ConstantPositionMobilityModel>();
	p1=CreateObject<ConstantPositionMobilityModel>();
	n1->AggregateObject(p1);
	p1->SetPosition(temp1);
	
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay",StringValue("2ms"));

	NetDeviceContainer devices;
	devices=pointToPoint.Install(nodes);

	InternetStackHelper stack;
	stack.Install(nodes);
	
	Ipv4AddressHelper address;
	address.SetBase("192.168.1.0","255.255.255.0");

	Ipv4InterfaceContainer interfaces=address.Assign(devices);
	
	UdpServerHelper echoServer(9);  //UDPServerHelper class is used to create a UDP server

	ApplicationContainer serverApp=echoServer.Install(nodes.Get(1));

	serverApp.Start(Seconds(1.0));
	serverApp.Stop(Seconds(10.0));

	UdpClientHelper echoClient(interfaces.GetAddress(1),9); //UDPClientHelper class is used to create a UDP client
	echoClient.SetAttribute("MaxPackets",UintegerValue(1));
	echoClient.SetAttribute("Interval",TimeValue(Seconds(1.0)));
	echoClient.SetAttribute("PacketSize",UintegerValue(1024));

	ApplicationContainer clientApp=echoClient.Install(nodes.Get(0));
	clientApp.Start(Seconds(2.0));
	clientApp.Stop(Seconds(10.0));

	AnimationInterface anim ("udp.xml");
	//anim.UpdateNodeSize(0,0.2,0.2);
	//anim.UpdateNodeSize(1,0.2,0.2);

	//nodes.EnablePcapAll ("udpPcap");

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

