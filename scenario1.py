
#  * This program is free software; you can redistribute it and/or modify
#  * it under the terms of the GNU General Public License version 2 as
#  * published by the Free Software Foundation;
#  *
#  * This program is distributed in the hope that it will be useful,
#  * but WITHOUT ANY WARRANTY; without even the implied warranty of
#  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  * GNU General Public License for more details.
#  *
#  * You should have received a copy of the GNU General Public License
#  * along with this program; if not, write to the Free Software
#  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#  */

import ns.applications
import ns.core
import ns.internet
import ns.network
import ns.point_to_point
import ns.netanim as netanim 
import matplotlib.pyplot as plt
########
# Create a trace file



# // Default Network Topology
# //
# //       10.1.1.0
# // n0 -------------- n1
# //    point-to-point
# //



#echoServer = ns.applications.UdpEchoServerHelper(9)
#serverApps = echoServer.Install(nodes.Get(1))
#serverApps.Start(ns.core.Seconds(1.0))
#serverApps.Stop(ns.core.Seconds(10.0))

packet_sizes = [64, 128, 256, 512, 1024 , 5024 , 10200 , 55555]
y1 = []
y2 = []
for size in packet_sizes:
	ns.core.LogComponentEnable("UdpEchoClientApplication", ns.core.LOG_LEVEL_INFO)
	ns.core.LogComponentEnable("UdpEchoServerApplication", ns.core.LOG_LEVEL_INFO)

	nodes = ns.network.NodeContainer()
	nodes.Create(2)

	pointToPoint = ns.point_to_point.PointToPointHelper()
	pointToPoint.SetDeviceAttribute("DataRate", ns.core.StringValue("5Mbps"))
	pointToPoint.SetChannelAttribute("Delay", ns.core.StringValue("2ms"))

	devices = pointToPoint.Install(nodes)

	stack = ns.internet.InternetStackHelper()
	stack.Install(nodes)

	address = ns.internet.Ipv4AddressHelper()
	address.SetBase(ns.network.Ipv4Address("10.1.1.0"),
		        ns.network.Ipv4Mask("255.255.255.0"))

	interfaces = address.Assign(devices)
	asciiTraceHelper = ns.network.AsciiTraceHelper()
	traceFileStream = asciiTraceHelper.CreateFileStream("trace.tr")
	print(size)
	echoServer = ns.applications.UdpEchoServerHelper(9)
	print(size)
	serverApps = echoServer.Install(nodes.Get(1))
	serverApps.Start(ns.core.Seconds(1.0))
	serverApps.Stop(ns.core.Seconds(10.0))
	print(size)
	echoClient = ns.applications.UdpEchoClientHelper(interfaces.GetAddress(1), 9)
	print(size)
	echoClient.SetAttribute("MaxPackets", ns.core.UintegerValue(1))
	echoClient.SetAttribute("Interval", ns.core.TimeValue(ns.core.Seconds(1.0)))
	echoClient.SetAttribute("PacketSize", ns.core.UintegerValue(size))
	
	clientApps = echoClient.Install(nodes.Get(0))
	clientApps.Start(ns.core.Seconds(2.0))
	clientApps.Stop(ns.core.Seconds(10.0))

########
# Connect the trace sources for each network interface to the trace file
	pointToPoint.EnableAsciiAll(traceFileStream)
	stack.EnableAsciiIpv4All(traceFileStream)

	#anim = ns.netanim.AnimationInterface("animation.xml")
	ns.core.Simulator.Run()
	ns.core.Simulator.Destroy()
	traceFile = open("trace.tr", "r")

# Initialize variables to calculate throughput
	totalBytes = 0
	startTime = 0.0
	endTime = 0.0
	



	for line in traceFile:
	    # Split the line into columns
	    columns = line.split()

	    # Get the current time
	    currentTime = float(columns[1])

	    # Get the packet size
	    #packetSize = int(columns[5])
	    packetSize = size

	    # If this is the first packet, record the start time
	    if startTime == 0.0:
	    	startTime = currentTime

	    # Add the packet size to the total bytes
	    totalBytes += packetSize

	    # Record the end time
	    endTime = currentTime

	# Calculate the throughput
	duration= endTime- startTime     #temps latence 
	throughput= (totalBytes* 8) / (duration* 1000000)   #debit 

	# Print the results
	print("Total Bytes: ", totalBytes)
	print("Start Time: ", startTime)
	print("End Time: ", endTime)
	print("Duration: ", duration)
	print("Throughput: ", throughput, "Mbps")
	y1.append(throughput)
	y2.append(duration)
	
	traceFile.close()
print(y1)
print(y2)
	
	
# Open the trace file and read the data






# Add labels and title
plt.xlabel('taille du paquet')
#plt.ylabel('débit en Mbps')
plt.title('Débit et temps de latence en fonction du taille du paquet ')

# Close the trace file
plt.plot(packet_sizes, y1, "r--", label="débit en Mbps",linewidth=3)
plt.plot(packet_sizes, y2, "b:o", label="temps du latence",linewidth=3)
plt.legend()
plt.grid(True)
plt.show()
