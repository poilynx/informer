#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push,1)
typedef struct heartbeat_s {
        uint8_t  salt;
        uint8_t  mark;
        uint32_t osid;
        char hostname[10];
} heartbeat,*pheartbeat;
#pragma pack(pop)

int parse_request(char *buf,posid poi) {
        int             i;
        heartbeat       hb;
        unsigned char * p = buf;
        uint8_t         salt = ((p[0]-'a')<<4) | (((p[1])) -'k');
        for (i=1;i<sizeof(heartbeat);i++) {
                ((unsigned char*)&hb)[i]
                = ((((p[i*2]-'a')<<4)
                        | ((p[i*2+1]-'k'))) ^ salt);
        }
        hb.salt = salt;
        if (hb.mark != 0xEA) {
                return 1;
                //check mark field failed
        }
        //printf("salt = %hhx\n",salt);
        //printf("mark = %hhx\n",hb.mark);
        poi->iswow64 = hb.osid>>31;
        poi->major = (hb.osid>>24)&~0x80;
        poi->minor = (hb.osid>>16)&0xFF;
        poi->build = hb.osid&0xFFFF;
        memcpy(poi->hostname,hb.hostname,sizeof(hb.hostname));
        poi->hostname[sizeof(poi->hostname)-1] = '\0';
        return 0;
}

