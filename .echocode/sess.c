#include "sess.h"
#include "md5.h"
//#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
struct session_t {
	char free;
	char sid[16];
	void *data;
};

char sidsalt[16];
struct session_t * sessvect = NULL;
int sv_top, sv_len, sv_size;
struct itimerval timer;

static void makesid(uint16_t no, char sid[16]) {
	char buf[16];
	*((uint16_t *)sid) = no;
	*((uint16_t *)sid + 1) = rand() % 0xFFFF;
	*((uint16_t *)sid + 2) = rand() % 0xFFFF;
	*((uint16_t *)sid + 3) = rand() % 0xFFFF;
	md5(sid + 2, 6, buf);
	memcpy(sid + 8, buf, 8);
}

static int isvalidsid(char sid[16]) {
	char buf[16];
	md5(sid + 2, 6, buf);
	if(strncmp(sid + 8, buf, 8) == 0)
		return 1;
	else 
		return 0;
}

void timer_proc(int signo) {
	printf("timer\n");
}

int sess_init(int max_sess, int ttl) {
	int res;
	assert(max_sess > 0);
	assert(ttl > 0);
	if(sessvect != NULL) {
		perror("session lib already initialized.");
		exit(-1);
	}

	if(SIG_ERR == signal(SIGALRM, timer_proc)) {
		perror("signal");
		exit(-1);
	}
	timer.it_interval.tv_sec	= 1;
	timer.it_interval.tv_usec	= 0;
	timer.it_value.tv_sec		= 1;
	timer.it_value.tv_usec		= 0;
	res = setitimer(ITIMER_REAL, &timer, NULL);
	if(res == -1) {
		perror("setitimer");
		exit(-1);
	}

	sessvect = malloc(sizeof(struct session_t) * max_sess);
	if(sessvect == NULL) {
		perror("malloc");
		exit(-1);
	}
	/* initialize sessvect queue front and rear pointer */
	sv_top = sv_len = 0;
	sv_size = max_sess;

	/* initialize random seed */
	srand(time(NULL));

	return 0;
}
int sess_destroy() {
	if(sessvect == NULL) {
		perror("session lib not initialized.");
		exit(-1);
	}
	
	free(sessvect);
	sessvect = NULL;
	timer.it_interval.tv_sec	= 0;
	timer.it_interval.tv_usec	= 0;
	setitimer(ITIMER_REAL, &timer, NULL);

	return 0;
}

int sess_removed_do(/*callback*/) {
	kkk
}

int sess_new(char sid[16], void* data) {
	if(sv_len == sv_size)
		return -1;
	//memcpy(sid+4,/*hash*/,16);
	makesid(sv_top, sid );
	memcpy(sessvect[sv_top].sid, sid+4, 16);
	sessvect[sv_top].data = data;
	sv_top = (sv_top+1) % sv_size;
	sv_len ++;
	return sv_top;
}

int sess_checkout(char sid[20], void** pdata) {
	int i, no;
	no = *(int*) sid;
	if(no < 0 || no >= sv_size)
		return -1;

	for(i=0;i<16;i++) 
		if(sid[4 + i] != sessvect[no].sid[i])
			return -1;

	*pdata = sessvect[no].data;
	return 0;

}

/* 3des(id2+rand6)+md5(rand)8 */

/* id2+rand6+md5(rand)8 not good */

