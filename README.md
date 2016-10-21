This README gives the detailed description of how to build the kernel module "Firewall" and insert it into the kernel.

##System used

The topology used in GENI is given in the file "Topology".
All the GENI nodes use Ubuntu 14.04 image

The system description for all the nodes is given below

root@Host1:~# uname -a
Linux Host1 3.13.0-24-generic #46-Ubuntu SMP Thu Apr 10 19:11:08 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux
root@Host1:~# 

root@Host2:~# uname -a
Linux Host2 3.13.0-24-generic #46-Ubuntu SMP Thu Apr 10 19:11:08 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux
root@Host2:~#

root@WebServer:~# uname -a
Linux WebServer 3.13.0-24-generic #46-Ubuntu SMP Thu Apr 10 19:11:08 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux
root@WebServer:~# 

root@ovs:~# uname -a
Linux ovs 3.13.0-24-generic #46-Ubuntu SMP Thu Apr 10 19:11:08 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux
root@ovs:~# 

root@FirewallGateway:~# uname -a
Linux FirewallGateway 3.13.0-24-generic #46-Ubuntu SMP Thu Apr 10 19:11:08 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux
root@FirewallGateway:~#

root@RemoteClient:~# uname -a
Linux RemoteClient 3.13.0-24-generic #46-Ubuntu SMP Thu Apr 10 19:11:08 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux
root@RemoteClient:~# 

##Firewall installation instruction
![Alt text](topology.png?raw=true "Topology")

Step1: Creating topology
	- The file "topology" is used to create the topology in GENI.

Step2: Install the required packages and cofigure the switch 
	- The detailed explaination about the steps followed on each node is given in the answer of question 1.

Step3: Copy the Firewall.c and Makefile to the node "FirewallGateway"

Step4: Issue "make" command to build the kernel module
	This should output Firewall.ko file along with other object and module files.

Step5: Issue "insmod Firewall.ko INTERFACE=<interface>" command to insert the module into the kernel.
	Where <interface> is the name of the interface towards the public network (i.e. the interface to which the RemoteClient is connected).

	Example: "insmod Firewall.ko INTERFACE=eth1"


##Web Server IP address modification

The WebServer address is hardcoded into the Firewall.c kernel program. 
If the IP address of the WebServer is other than 192.168.1.12 the Macro in the Firewall.c program should be changed to the appropriate ip address in hexadecimal format.

    Example: If the Server IP address is 10.0.0.1
	   In Firewall.c file change #define WEB_SERVER_ADDRESS "\xC0\xA8\x01\x0C" to #define WEB_SERVER_ADDRESS "\x0A\x00\x00\x01"
