
#define _BSD_SOURCE
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/tcp.h>
#define WEB_SERVER_ADDRESS "\xC0\xA8\x01\x0C"
//#define IP_SUBNET_START	"\xC0\xA8\x01\x00"
//#define IP_SUBNET_END "\xC0\xA8\x02\x00"
//#define PRIVATE_SIDE_INTERFACE "eth1"
#define SSH_PORT "\x00\x16"
#define HTML_PORT "\x00\x50"


//static unsigned char *ip_subnet_start = IP_SUBNET_START;
//static unsigned char *ip_subnet_end = IP_SUBNET_END;
static unsigned char *ip_address = WEB_SERVER_ADDRESS;
unsigned char *html_port = HTML_PORT;
unsigned char *ssh_port = SSH_PORT;
char PRIVATE_SIDE_INTERFACE[5];

module_param_string(INTERFACE, PRIVATE_SIDE_INTERFACE, 5, 0);


struct nf_hook_ops Firewall;


unsigned int nf_filter_function(unsigned int hooknum,
                               struct sk_buff *skb,
                               const struct net_device *in,
                               const struct net_device *out,
                               int (*okfn)(struct sk_buff *)){

	int is_web_server = 0; //boolean value to indicate if the request is for webserver or not

	struct iphdr *ip_header;
	struct icmphdr *icmp_header;
	struct tcphdr *tcp_header;
	char s_ip[16];
	char d_ip[16];
	//enter only if the packets are from outside we need to allow packets to passthrough from inside to out
	if (strcmp(in->name, PRIVATE_SIDE_INTERFACE) == 0) {

		ip_header = (struct iphdr *)skb_network_header(skb); //IP header
				
		if (ip_header->daddr == *(unsigned int *)ip_address)
			is_web_server = 1;
	
		sprintf(s_ip, "%d.%d.%d.%d",ip_header->saddr & 0xFF, (ip_header->saddr >> 8) & 0xFF,( ip_header->saddr >>16) & 0xFF, (ip_header->saddr >> 24) & 0xFF);			
		sprintf(d_ip, "%d.%d.%d.%d",ip_header->daddr & 0xFF, (ip_header->daddr >> 8) & 0xFF,( ip_header->daddr >>16) & 0xFF, (ip_header->daddr >> 24) & 0xFF);
		//check if it is ICMP packets other than web server/
		if ((ip_header->protocol == IPPROTO_ICMP) && (!is_web_server)){
			icmp_header = (struct icmphdr *)(skb->data + (ip_header->ihl *4));
			if(icmp_header->type == ICMP_ECHO ){
				
				printk ("Dropped: cause: ICMP, interface %s, packet from %s to %s\n",in->name,s_ip, d_ip );
				return NF_DROP;
			}
			
		}
			
		//ssh and HTML access attempts from outside
		if ((ip_header->protocol == IPPROTO_TCP)){	//First check if it is a TCP connection 

			tcp_header = (struct tcphdr *)(skb->data + (ip_header->ihl *4));	//Get tcp header

			if((tcp_header->dest) == (*(unsigned short*)ssh_port)){   // Block if the request is SSH (port 22)
					
				printk ("Dropped: cause: SSH, interface %s, packet from %s to %s\n",in->name, s_ip, d_ip);
				return NF_DROP;

			}

			if((tcp_header->dest) == (*(unsigned short*)html_port) && !is_web_server){ //Block HTTP request (port 80) for any host other than WebServer
				printk ("Dropped: cause: HTTP, interface %s, packet from %s to %s\n",in->name, s_ip, d_ip);
				return NF_DROP;
			}
		}
	}


return NF_ACCEPT; 
}


int init_module(){

	Firewall.hook		=	nf_filter_function;
	Firewall.pf		=	PF_INET;
	Firewall.hooknum		=	NF_INET_PRE_ROUTING;
	Firewall.priority		= 	NF_IP_PRI_FIRST;

	nf_register_hook(&Firewall);
	return 0;
}

void cleanup_module(){
	nf_unregister_hook(&Firewall);
}

