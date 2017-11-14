#ifndef HASHFUNCTIONS_H
#define HASHFUNCTIONS_H

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

unsigned int MurmurHash2(const char *key, int len, unsigned int seed);
uint32_t murmurHash3 (const char *key, uint32_t len, uint32_t seed)

#endif //HASHFUNCTIONS_H
