#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int a, char *b[]){
	
	CommandLine cmd;
	cmd.Parse(a,b);

	NodeContainer nodes;
	nodes.Create(2);

	//setting the location only
	Vector temp0(1,1,0);
	Ptr<Node> n0=nodes.Get(0);
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
	pointToPoint.SetDeviceAttribute("DataRate",StringValue("2Mbps"));
	pointToPoint.SetChannelAttribute("Delay",StringValue("20ms"));
	pointToPoint.SetQueue("ns3::DropTailQueue","MaxSize",StringValue("50p"));

	NetDeviceContainer devices;
	devices=pointToPoint.Install(nodes);

	InternetStackHelper stack;
	stack.Install(nodes);
	
	Ipv4AddressHelper address;
	address.SetBase("109.11.12.0","255.255.255.0");

	Ipv4InterfaceContainer interfaces=address.Assign(devices);
	
	PacketSinkHelper sink("ns3::TcpSocketFactory",Address(InetSocketAddress(Ipv4Address::GetAny(),10)));
	//PacketSinkHelper sink("ns3::TcpSocketFactory",Address(InetSocketAddress(Ipv4Address("0.0.0.0"),10)));

	ApplicationContainer server=sink.Install(nodes.Get(1));
	server.Start(Seconds(1.0));
	server.Stop(Seconds(10.0));

	OnOffHelper onOff("ns3::TcpSocketFactory",Address(InetSocketAddress(Ipv4Address("109.11.12.2"),10)));

	onOff.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	onOff.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	onOff.SetAttribute("DataRate",StringValue("2Mbps"));
	onOff.SetAttribute("PacketSize",UintegerValue(1024));

	ApplicationContainer client=onOff.Install(nodes.Get(0));
	client.Start(Seconds(1.0));
	client.Stop(Seconds(5.0));

	AnimationInterface anim ("TCP.xml");
	anim.UpdateNodeSize(0,0.1,0.1);
	anim.UpdateNodeSize(1,0.1,0.1);

	anim.UpdateNodeDescription(0,"TCP-Sink-Receiver");
	anim.UpdateNodeDescription(1,"TCP-onOff-Sender");

	pointToPoint.EnablePcapAll ("tcpPcap");

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
