#include "echosvr.h"
#include <stdint.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <linux/icmp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define ICMP_DATA_OFFSET (sizeof(struct iphdr) + sizeof(struct icmphdr))

static int sock;

static uint16_t in_cksum(uint16_t *addr, int len)
{
	int             nleft = len;
	uint32_t        sum = 0;
	uint16_t        *w = addr;
	uint16_t        answer = 0;

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

extern int echosvr_init(void) {
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock == -1) {
		perror("socket() error");
		return 1;
	}
	int optval;
	setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));	
	return ES_E_SUCCEED;
}

int echosvr_listen(request* preq) {
	struct iphdr*	pip;
	struct icmphdr*	picmp;
	int 		len;
	unsigned char	buf[4096];

	pip = (struct iphdr*) buf;
	picmp = (struct icmphdr*) (buf + sizeof(struct iphdr));

	for (;;) {
		len = recvfrom(sock,buf,sizeof(buf), 0, NULL, NULL);
		if (len == -1) {
			perror("recvfrom() error");
			return 1;
		}

		if( picmp->code == 0 ) 
			break;;
	}
	if (len - ICMP_DATA_OFFSET > 0)
		preq->data = malloc(len - ICMP_DATA_OFFSET);
	preq->len = len - ICMP_DATA_OFFSET;

	memcpy(preq->data, buf + ICMP_DATA_OFFSET, len - ICMP_DATA_OFFSET);
	preq->len = len - ICMP_DATA_OFFSET;

	preq->addr.s_addr = pip->saddr;
	preq->daddr.s_addr = pip->daddr;

	preq->seq = 	ntohs(picmp->un.echo.sequence);
	preq->id =	ntohs(picmp->un.echo.id);

	return ES_E_SUCCEED;
}

int echosvr_reply(request* preq, void* data, size_t size) {
	struct iphdr*	pip;
	struct icmphdr*	picmp;
	int 		len;
	unsigned char	buf[4096];
	struct sockaddr_in sinaddr;

	//pip = (struct iphdr*) buf;
	picmp = (struct icmphdr*)buf;

	if(sizeof(struct icmphdr) + size > sizeof(buf)) {
		printf("param size too big\n");
		return 1;
	}
	
	len = sizeof(struct icmphdr) + size;
	/*
	pip->ihl =		5;
	pip->version = 		4;
	pip->tos = 		0;
	pip->tot_len =		htons(len);
	pip->frag_off =		0;
	pip->protocol =		1;
	pip->ttl =		htons(112);
	pip->saddr = 		preq->daddr.s_addr;
	pip->daddr = 		preq->addr.s_addr;
	pip->check = 		0;
	pip->check = 		in_cksum((uint16_t*)pip,sizeof(struct iphdr));
	*/
	picmp->type = 		ICMP_ECHOREPLY;
	picmp->code = 		0;
	picmp->checksum = 	0;
	picmp->un.echo.id = 	htons(preq->id);
	picmp->un.echo.sequence = htons(preq->seq);

	memcpy(buf + sizeof(struct icmphdr), data, size);

	picmp->checksum = 	in_cksum((uint16_t*)picmp,
					sizeof(struct icmphdr) + size);
	
	sinaddr.sin_family = 	AF_INET;
	sinaddr.sin_addr.s_addr = preq->addr.s_addr;


	int ret = sendto(sock, buf, len , 
			0, (struct sockaddr *)&sinaddr,sizeof(struct sockaddr));
	//free
	

	return ES_E_SUCCEED;

}






















