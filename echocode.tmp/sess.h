#ifndef SESS_H
#define SESS_H
int sess_init(int max_sess, int ttl);
int sess_destroy();
int sess_new(char hash[20], void* data);
int sess_check(char hash[20], void** pdata);
#endif

