//Node(0) Client --------> Node(1) Server
//libraries/header files
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h" //point to point
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

//define a namespace 
using namespace ns3;

//for documentation
NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main (int argc, char *argv[])   //main class
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  //take logs, logs are for what we are doing the research

  Time::SetResolution (Time::NS);  //Total time your application is going to consume
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO); 
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  //Creating nodes, Taking n number of computers
  NodeContainer nodes;  //NodeContainer-Class nodes-Object
  nodes.Create (2);
     
  //Choose your technology to communicate
  //Assigning what type of channel we wanto have? wifi, csma, ethernet, p2p etc...
  PointToPointHelper pointToPoint;   //PointToPointHelper helps to create a p2p channel
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  //Install technology on computers
  //Have to install few containers on top of that nodes
  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);  // Installing channel on top of the devices

  //Ask devices to follow some rules (eg:TCp,UDP,Ip...)
  InternetStackHelper stack;
  stack.Install (nodes);

  //Assign ip address to communicate
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0"); //Network address, subnetmask

  Ipv4InterfaceContainer interfaces = address.Assign (devices);  //performs actual address assignment


  //what type of server on port x?
  UdpEchoServerHelper echoServer (9);

  //Install server on a node then start and stop the server
  //Run server apps on top of 2nd node
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));  //Access nodes through nodes.Get (1) method. 0-first node 1-second node
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //Create x type of client and  set its attributes
  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  //Install client on a node then start and stop it.
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));  //start to send req at 2 sec
  clientApps.Stop (Seconds (10.0));
  
  //xml 
  
  AnimationInterface anim("p2p.xml");



  //Run simulation
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
