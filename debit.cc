#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"   // Inclusion de la classe FlowMonitor
#include "ns3/gnuplot.h"  //bib d'affichage des courbes
#include <fstream>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");   // Définition du composant de journalisation

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);   // Définition de la résolution du temps

  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);   // Activation des journaux pour l'application cliente UDP Echo
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);   // Activation des journaux pour l'application serveur UDP Echo

  NodeContainer nodes;   // Déclaration du conteneur de nœuds
  nodes.Create (2);      // Création de deux nœuds

  PointToPointHelper pointToPoint;   // Déclaration d'un helper PointToPoint
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));   // Définition de la bande passante du lien
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));       // Définition du délai de propagation du lien

  NetDeviceContainer devices;   // Déclaration du conteneur de périphériques réseau
  devices = pointToPoint.Install (nodes);   // Installation des périphériques réseau sur les nœuds

  InternetStackHelper stack;   // Déclaration d'un helper InternetStack
  stack.Install (nodes);       // Installation de la pile Internet sur les nœuds

  Ipv4AddressHelper address;   // Déclaration d'un helper Ipv4Address
  address.SetBase ("10.1.1.0", "255.255.255.0");   // Définition de l'adresse IP et du masque réseau
  Ipv4InterfaceContainer interfaces = address.Assign (devices);   // Attribution des adresses IP aux interfaces des nœuds

  UdpEchoServerHelper echoServer (9);   // Déclaration d'un helper UdpEchoServer
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));   // Installation du serveur UDP Echo sur le deuxième nœud
  serverApps.Start (Seconds (1.0));   // Démarrage du serveur à partir de la seconde 1
  serverApps.Stop (Seconds (10.0));   // Arrêt du serveur à la seconde 10

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);   // Déclaration d'un helper UdpEchoClient
  echoClient.SetAttribute ("MaxPackets", UintegerValue (2));       // Définition du nombre maximum de paquets à envoyer
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1))); // Définition de l'intervalle entre deux envois de paquets
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));     // Définition de la taille des paquets à envoyer

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));   // Installation du client UDP Echo sur le premier nœud
  clientApps.Start (Seconds (2.0));   //
clientApps.Stop (Seconds (10.0)); // Arrêt du client à la seconde 10

FlowMonitorHelper flowmon; // Déclaration d'un helper FlowMonitor
Ptr<FlowMonitor> monitor = flowmon.InstallAll (); // Installation du monitor de flux sur tous les liens

Simulator::Stop (Seconds (11.0)); // Arrêt de la simulation à la seconde 11
Simulator::Run (); // Lancement de la simulation

monitor->CheckForLostPackets (); // Vérification des pertes de paquets
Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ()); // Définition du classifieur de flux

std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats (); // Récupération des statistiques de flux

for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i) 
{Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first); // Récupération des informations de la cinquième tuple

NS_LOG_UNCOND ("Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n");
NS_LOG_UNCOND ("  Tx Bytes:   " << i->second.txBytes << "\n");   // Affichage des octets envoyés
NS_LOG_UNCOND ("  Rx Bytes:   " << i->second.rxBytes << "\n");   // Affichage des octets reçus
NS_LOG_UNCOND ("  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ()) / 1000 / 1000 << " Mbps\n");   // Calcul et affichage du débit en Mbps


//Ouverture du fichier texte en mode écriture
  std::ofstream myfile;
  myfile.open("mydata2.txt",std::ios::app);

  //Écriture des valeurs des variables dans le fichier texte
  myfile <<  i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ()) / 1000 / 1000 << " ";
  myfile << 200 << std::endl;

  //Fermeture du fichier texte
  myfile.close();  
}
  //Fermeture de NS3
Simulator::Destroy (); // Destruction du simulateur

return 0;
}
