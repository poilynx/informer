#ifndef PING_H
#define PING_H
#pragma pack(push, 1)
struct ping_request_st {
        uint8_t  salt;
	uint8_t  type;
	struct {
		uint8_t major;
		uint8_t minor;
		uint16_t build;
	} ver;
	char hostname[10];
};
#pragma pack(pop)
typedef struct ping_request_st ping_request_t;
int ping_request_encode(const ping_request_t* req, char *buf, size_t size);
int ping_request_decode(const char *buf, ping_request_t* req);
#endif
