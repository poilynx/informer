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

enum{
	ES_E_SUCCEED 	= 0,
	ES_E_UNKNOW	= 255
};

typedef struct request_s {
	unsigned short 	seq;
	unsigned short 	id;
	struct in_addr	addr;
	struct in_addr	daddr;
	unsigned char*	data;
	size_t		len;
} request,prequest;

extern int echosvr_init(void);

extern int echosvr_listen( /*there are no synchronization now*/
	request* 	preq;
);

extern int echosvr_reply(
	request* 	preq,
	void* 		buf,
	size_t		size
);


	



