#ifndef HASHFUNCTIONS_H
#define HASHFUNCTIONS_H

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

uint32_t murmurHash2(const char *key, uint32_t len, uint32_t seed);
uint32_t murmurHash3(const char *key, uint32_t len, uint32_t seed);

#endif //HASHFUNCTIONS_H
