#ifndef _DES_H_
#define _DES_H_
#include <stdint.h>

#define DES_ENCRYPT 'e'
#define DES_DECRYPT 'd'
uint64_t des(uint64_t input, uint64_t key, char mode);

#endif
