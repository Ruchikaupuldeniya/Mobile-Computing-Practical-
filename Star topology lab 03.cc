#include "ns3/core-module.h"      // Core ns-3 module: provides Simulator, time, logging, etc.
#include "ns3/network-module.h"   // Network module: provides Node, NodeContainer, NetDevice, etc.
#include "ns3/internet-module.h"  // Internet module: provides IPv4, TCP/UDP stack installation
#include "ns3/point-to-point-module.h" // Point-to-point module: provides P2P link helpers and devices
#include "ns3/applications-module.h"   // Applications module: provides BulkSendHelper, PacketSinkHelper, etc.
#include "ns3/netanim-module.h" 

using namespace ns3;

int main(int argc, char *argv[]){
	
	PointToPointHelper p2p;                                      // Create a helper to configure P2P links
    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));   // Set link bandwidth to 10 Mbps
    p2p.SetChannelAttribute("Delay", StringValue("2ms")); 
	
	//node
	NodeContainer starNodes;  // Container to hold all nodes in the star topology
    starNodes.Create(6);  
	
	//net device
	
	NetDeviceContainer channel1 = p2p.Install(starNodes.Get(0),starNodes.Get(1)); // Install P2P link between central hub and leaf 1
	NetDeviceContainer channel2 = p2p.Install(starNodes.Get(0),starNodes.Get(2)); // Install P2P link between central hub and leaf 1
	NetDeviceContainer channel3 = p2p.Install(starNodes.Get(0),starNodes.Get(3)); // Install P2P link between central hub and leaf 1
	NetDeviceContainer channel4 = p2p.Install(starNodes.Get(0),starNodes.Get(4)); // Install P2P link between central hub and leaf 1
	
	NetDeviceContainer channel[5];
	for(int i=0;i<5;i++){
		
		channel[i] = p2p.Install(starNodes.Get(0),starNodes.Get(i+1));
	}
	
	//internet stack
	InternetStackHelper stack;
	stack.Install(starNodes);
	
	
	
	//ip addressing and interface assign
	Ipv4AddressHelper address;
	address.SetBase("10.0.1.0","255.255.255.0");
	
	
	Ipv4InterfaceContainer interface[5];
	for(int i=0;i<5;i++){
		interface[i] = address.Assign(channel[i]);
		address.NewNetwork();
	}
	
	
	//server
	uint16_t port = 8080;
	ApplicationContainer serverApp;
	for(int i=0;i<5;i++){
		PacketSinkHelper server("ns3::TcpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(),port));
		serverApp.Add(server.Install(starNodes.Get(i+1)));
	}
	
	
	//client
	ApplicationContainer clientApp;
	for(int i=0;i<5;i++){
		Address serverAddress(InetSocketAddress(interface[i].GetAddress(1),port));
		
		BulkSendHelper client("ns3::TcpSocketFactory",serverAddress);
		client.SetAttribute("MaxBytes",UintegerValue(0));
		
		clientApp.Add(client.Install(starNodes.Get(0)));
		
	}
	
	serverApp.Start(Seconds(0.0));
	serverApp.Stop(Seconds(10.0));
	
	clientApp.Start(Seconds(1.0));
	clientApp.Stop(Seconds(10.0));
	
	//animation
	AnimationInterface anim("starUshan.xml");
	anim.SetConstantPosition(starNodes.Get(0), 50, 50);  // Central hub: placed at center
    	anim.SetConstantPosition(starNodes.Get(1), 0, 50);   // Leaf 1: left
    	anim.SetConstantPosition(starNodes.Get(2), 50, 20);  // Leaf 2: top
    	anim.SetConstantPosition(starNodes.Get(3), 100, 50); // Leaf 3: right
    	anim.SetConstantPosition(starNodes.Get(4), 80, 80);  // Leaf 4: bottom-right
    	anim.SetConstantPosition(starNodes.Get(5), 20, 80);
	
	Simulator::Stop(Seconds(10.0)); // Tell the simulator to stop at 10 seconds
    Simulator::Run();               // Start and run the simulation event loop
    Simulator::Destroy();           // Clean up all simulation objects and free memory

    return 0;
}