#include "ns3/core-module.h"
#include "ns3/network-module.h"	
#include "ns3/internet-module.h"	
#include "ns3/csma-module.h"	
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
	
using namespace ns3;

int main(int argc,char *argv[]){
	CommandLine cmd;
	cmd.Parse(argc,argv);
	
	Time::SetResolution(Time::NS);
	LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);	
	LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);

	NodeContainer csmaNodes;
	csmaNodes.Create(3);	

	CsmaHelper csma;	
	csma.SetChannelAttribute("DataRate",StringValue("100Mbps"));	
	csma.SetChannelAttribute("Delay",TimeValue(NanoSeconds(6050)));
	
	NetDeviceContainer csmaDevices;
	csmaDevices = csma.Install(csmaNodes);
	
	InternetStackHelper stack;
	stack.Install(csmaNodes);
	
	Ipv4AddressHelper address;
	
	address.SetBase("192.168.1.0","255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces;
	csmaInterfaces = address.Assign(csmaDevices);
	
	UdpEchoServerHelper echoServer(9);
	
	ApplicationContainer serverApp=echoServer.Install(csmaNodes.Get(2));	
	serverApp.Start(Seconds(1.0)); 
	serverApp.Stop(Seconds(10.0));

	UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(2),9);	
	echoClient.SetAttribute("MaxPackets",UintegerValue(1));
	echoClient.SetAttribute("Interval",TimeValue(Seconds(1.0)));
	echoClient.SetAttribute("PacketSize",UintegerValue(1024));
	
	ApplicationContainer clientApp=echoClient.Install(csmaNodes.Get(0));	
	clientApp.Start(Seconds(2.0));
	clientApp.Stop(Seconds(10.0));
	
	AnimationInterface anim("csma.xml");
	anim.SetConstantPosition(csmaNodes.Get(0),40.0,40.0);
	anim.SetConstantPosition(csmaNodes.Get(1),60.0,20.0);
	anim.SetConstantPosition(csmaNodes.Get(2),80.0,60.0);
	
	anim.UpdateNodeDescription(csmaNodes.Get(0),"Client");
	anim.UpdateNodeDescription(csmaNodes.Get(1),"ServerMid");	
	anim.UpdateNodeDescription(csmaNodes.Get(2),"Server");
	
	anim.UpdateNodeSize(0,5,5);
	anim.UpdateNodeSize(1,10,10);
	anim.UpdateNodeSize(2,10,10);
	
	csma.EnablePcapAll("csmaPcap");

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}