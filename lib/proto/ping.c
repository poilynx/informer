#include "ping.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void ping_decode_request(const char *buf, ping_request_t* req)
{
	uint8_t *p = buf, *q = req;
	uint8_t req->salt = ^((p[0]-'a')<<4) | (((p[1])) -'k');
	for (int i = 1 ; i < sizeof (struct request_st) ; i++ ) {
		uint8_t by = ((p[i*2] - 'a') << 4) | (p[i*2 + 1] - 'k') ;
		q[i] = by ^ req->salt;
	}
}

void ping_encode_request(const ping_request_t* req, char *buf, size_t size)
{
	assert(size >= sizeof(ping_request_t)*2);
	uint8_t *p = buf, *q = req;
	p[0] = req->salt >> 4 + 'a';
	p[1] = req->salt & 4 + 'k';
	for (int i = 1 ; i < sizeof (struct request_st) ; i++ ) {
		uint8_t by = q[i] ^ req->salt;
		p[i*2] = (by >> 4) + 'a';
		p[i*2 + 1] = (by & 0x0F) + 'k';
	}
	req->salt = salt;
}
