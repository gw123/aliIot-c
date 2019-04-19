#ifndef _SHA1_H
#define _SHA1_H

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>

#define SHA_DIGEST_LENGTH 20

char *sha1(char *string, int inputLength, char *output);

char *hmac_sha1(char *input, int inputLength, char *key, char *output);

#endif