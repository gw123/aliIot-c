#include "sha1.h"

/***
 * 编译 gcc sha1.c -o sha1  -lcrypto
 *
 * @return
 */

char *sha1(char *input, int inputLength, char *output) {
    //output[SHA_DIGEST_LENGTH * 2] = 0;
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *) input, inputLength, (unsigned char *) &digest);
    //char mdString[SHA_DIGEST_LENGTH*2+1];
    int i=0;
    for ( i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(&output[i * 2], "%02x", (unsigned int) digest[i]);
    }
    return output;
}

char* hmac_sha1(char *input, int inputLength, char *key, char *output) {
    // Be careful of the length of string with the choosen hash engine. SHA1 needed 20 characters.
    // Change the length accordingly with your choosen hash engine.
    //unsigned char* result;
    unsigned int len = 20;
    //result = (unsigned char*)malloc(sizeof(char) * len);
    unsigned char digest[SHA_DIGEST_LENGTH];
    HMAC_CTX *ctx = HMAC_CTX_new();
    //HMAC_CTX_init(ctx);
    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
    HMAC_Init_ex(ctx, key, strlen(key), EVP_sha1(), NULL);
    HMAC_Update(ctx, (unsigned char*)input, inputLength);
    HMAC_Final(ctx, digest, &len);
    HMAC_CTX_free(ctx);
    int i =0;
    for (i = 0; i < len; i++) {
        sprintf(&output[i * 2], "%02x", (unsigned int) digest[i]);
    }
    return output;
}

//int main() {
//    char digestBuffer[SHA_DIGEST_LENGTH * 2 + 1];
//    memset(digestBuffer ,0 , sizeof(digestBuffer));
//    char *input = "hello world";
//    sha1(input, strlen(input), digestBuffer);
//    printf("SHA1 digest: %s\n", digestBuffer);
//
//    char *key = "0123456789";
//    hmac_sha1(input,strlen(input),key,digestBuffer);
//    printf("hmac_sha1 digest: %s\n" ,digestBuffer);
//    return 0;
//}

//int main()
//{
//    unsigned char digest[SHA_DIGEST_LENGTH];
//    char string[] = "hello world";
//    SHA1((unsigned char*)&string, strlen(string), (unsigned char*)&digest);
//    char mdString[SHA_DIGEST_LENGTH*2+1];
//    for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
//        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
//
//    printf("SHA1 digest: %s\n", mdString);
//    return 0;
//}