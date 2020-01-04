#include <stdint.h>
typedef struct echo_request_st {
	unsigned short 	seq;
	unsigned short 	id;
	struct in_addr	addr;
	struct in_addr	daddr;
	unsigned char*	data;
	size_t		len;
} echo_request_t;

extern void echosvr_init(void);

extern int echosvr_listen( /*there are no synchronization now*/
	echo_request_t* req;
);

extern int echosvr_reply(echo_request_t* req, void* buf, size_t size);
extern void echo_request_clear(echo_request_t* req);



	



