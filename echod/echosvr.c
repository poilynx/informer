#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <linux/icmp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include "echosvr.h"

#define ICMP_DATA_OFFSET (sizeof(struct iphdr) + sizeof(struct icmphdr))

static int sock;

static uint16_t in_cksum(uint16_t *addr, int len)
{
	int nleft = len;
	uint32_t sum = 0;
	uint16_t *w = addr;
	uint16_t answer = 0;

	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *)(&answer) = *(unsigned char *)w ;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return(answer);
}

void echosvr_init(void)
{
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock == -1) {
		perror("socket");
		exit(-1);
	}
	int optval;
	setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));	
	(void)optval;
}

int echosvr_listen(echo_request_t* req)
{
	struct icmphdr*	icmp_ptr;
	int len;
	unsigned char buf[4096];

	struct iphdr* pip = (struct iphdr*) buf;
	icmp_ptr = (struct icmphdr*) (buf + sizeof(struct iphdr));

	for (;;) {
		len = recvfrom(sock,buf,sizeof(buf), 0, NULL, NULL);
		if (len == -1) {
			perror("recvfrom");
			return 1;
		}

		if( icmp_ptr->code == 0 ) 
			break;;
	}
	if (len - ICMP_DATA_OFFSET > 0)
		req->data = malloc(len - ICMP_DATA_OFFSET);
	req->len = len - ICMP_DATA_OFFSET;

	memcpy(req->data, buf + ICMP_DATA_OFFSET, len - ICMP_DATA_OFFSET);
	req->len = len - ICMP_DATA_OFFSET;

	req->addr.s_addr = pip->saddr;
	req->daddr.s_addr = pip->daddr;

	req->seq = ntohs(icmp_ptr->un.echo.sequence);
	req->id = ntohs(icmp_ptr->un.echo.id);

	return 0;
}

int echosvr_reply(echo_request_t* req, void* data, size_t size)
{
	struct iphdr* pip;
	unsigned char buf[4096];

	struct icmphdr*	icmp_ptr = (struct icmphdr*)buf;

	assert(sizeof(struct icmphdr) + size > sizeof(buf));
	
	int len = sizeof(struct icmphdr) + size;
	/*
	pip->ihl =		5;
	pip->version = 		4;
	pip->tos = 		0;
	pip->tot_len =		htons(len);
	pip->frag_off =		0;
	pip->protocol =		1;
	pip->ttl =		htons(112);
	pip->saddr = 		req->daddr.s_addr;
	pip->daddr = 		req->addr.s_addr;
	pip->check = 		0;
	pip->check = 		in_cksum((uint16_t*)pip,sizeof(struct iphdr));
	*/
	icmp_ptr->type = ICMP_ECHOREPLY;
	icmp_ptr->code = 0;
	icmp_ptr->checksum = 0;
	icmp_ptr->un.echo.id = 	htons(req->id);
	icmp_ptr->un.echo.sequence = htons(req->seq);

	memcpy(buf + sizeof(struct icmphdr), data, size);

	icmp_ptr->checksum = in_cksum((uint16_t*)icmp_ptr,
					sizeof(struct icmphdr) + size);
	
	struct sockaddr_in sinaddr;
	sinaddr.sin_family = AF_INET;
	sinaddr.sin_addr.s_addr = req->addr.s_addr;

	int ret = sendto(sock, buf, len , 0, (struct sockaddr *)&sinaddr,sizeof(struct sockaddr));

	return ! (ret == 0);
}


void echo_request_clear(echo_request_t* req)
{
	assert(req);
	if(req->data) {
		free(req->data);
		req->data = NULL;
	}
}

