#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "sha1.h"
#include "mqtt/mongoose.h"

typedef struct Sign {
    int signLength;
    unsigned int timestamp;
    char *sign;
} Sign;

static char *s_user_name = NULL;
static char *s_password = NULL;
static char *s_client_id = NULL;

static void ev_handler(struct mg_connection *nc, int ev, void *p);

Sign *NewSign() {
    Sign *sign = (Sign *) malloc(sizeof(Sign));
    sign->sign = (char *) malloc(SHA_DIGEST_LENGTH * 2 + 1);
    sign->sign = memset(sign->sign, 0, SHA_DIGEST_LENGTH * 2 + 1);
    return sign;
}

void FreeSign(Sign *sign) {
    free(sign->sign);
    free(sign);
}

Sign *GetSign(char *productKey, char *deviceName, char *deviceSecret, Sign *sign) {
    time_t timestamp;
    time(&timestamp);
    char *signTplStr = "clientId%sdeviceName%sproductKey%stimestamp%d";
    char toSignStr[512];
    memset(toSignStr, 0, 512);
    sprintf(toSignStr, signTplStr, deviceName, deviceName, productKey, timestamp);
    //printf("productkey : %s,deviceName : %s,deviceSecret : %s;\n", productKey, deviceName, deviceSecret);

    char signStr[64];
    memset(signStr, 0, 64);
    hmac_sha1(toSignStr, strlen(toSignStr), deviceSecret, sign->sign);
    //sign->sign = signStr;
    sign->signLength = strlen(signStr);
    sign->timestamp = timestamp;
    printf("sign : %s timestamp:%d \n", sign->sign, (int)timestamp);
    return sign;
}

#define ERROR_ARGS  1
#define ERROR_CONN  2
int connectIot(char *productKey, char *deviceName, char *deviceSecret) {
    if(strlen(deviceName) > 100){
        return ERROR_ARGS;
    }
    if(strlen(productKey) > 50){
        return ERROR_ARGS;
    }
    if(strlen(productKey) > 50){
        return ERROR_ARGS;
    }

    Sign *sign = NewSign();
    GetSign(productKey, deviceName, deviceSecret, sign);

    char clientIdStr[200];
    memset(clientIdStr, 0, 200);
    sprintf(clientIdStr, "%s|securemode=3,signmethod=hmacsha1,timestamp=%d|", deviceName, sign->timestamp);

    char usernameStr[128];
    memset(usernameStr, 0, 128);
    sprintf(usernameStr, "%s&%s", deviceName, productKey);

    char host[128];
    memset(host, 0, 128);
    sprintf(host, "%s.iot-as-mqtt.cn-shanghai.aliyuncs.com:1883", productKey);

    struct mg_mgr mgr;
    mg_mgr_init(&mgr, NULL);

    s_user_name = usernameStr;
    s_password = sign->sign;


    s_client_id = clientIdStr;
    if (mg_connect(&mgr, host, ev_handler) == NULL) {
        fprintf(stderr, "mg_connect(%s) failed\n", host);
        exit(EXIT_FAILURE);
    }
    //FreeSign(sign);
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }


    return  0;
//    mg_connect_opt();
//    opts := mqtt.NewClientOptions().AddBroker(iot.Host).SetClientID(iot.ClientId).SetUsername(iot.Username).SetPassword(
//            iot.Password)
}


static void ev_handler(struct mg_connection *nc, int ev, void *p) {
    struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;
    (void) nc;

    if (ev != MG_EV_POLL) printf("USER HANDLER GOT EVENT %d\n", ev);

    switch (ev) {
        case MG_EV_CONNECT: {
            struct mg_send_mqtt_handshake_opts opts;
            memset(&opts, 0, sizeof(opts));
            opts.user_name = s_user_name;
            opts.password = s_password;

            mg_set_protocol_mqtt(nc);
            printf("clientId:%s\nusername:%s\npassword:%s\n",s_client_id,s_user_name,s_password);
            mg_send_mqtt_handshake_opt(nc, s_client_id, opts);
            break;
        }
        case MG_EV_MQTT_CONNACK:
            if (msg->connack_ret_code != MG_EV_MQTT_CONNACK_ACCEPTED) {

                printf("Got mqtt connection error: %d  \n", msg->connack_ret_code);
                exit(1);
            }
            //认证成功 开始订阅消息

            break;
        case MG_EV_MQTT_PUBACK:
            printf("Message publishing acknowledged (msg_id: %d)\n", msg->message_id);
            break;
        case MG_EV_MQTT_SUBACK:
            printf("Subscription acknowledged, forwarding to '/test'\n");
            break;
        case MG_EV_MQTT_PUBLISH: {
#if 0
            char hex[1024] = {0};
        mg_hexdump(nc->recv_mbuf.buf, msg->payload.len, hex, sizeof(hex));
        printf("Got incoming message %.*s:\n%s", (int)msg->topic.len, msg->topic.p, hex);
#else
            printf("Got incoming message %.*s: %.*s\n", (int) msg->topic.len,
                   msg->topic.p, (int) msg->payload.len, msg->payload.p);
#endif

            break;
        }
        case MG_EV_CLOSE:
            printf("Connection closed\n");
    }
}

int main() {
    char *productKey = "a1wwwwnRy777bf";
    char *deviceName = "test-0001";
    char *deviceSecret = "aJM5PFtBwsWNXaPx3RmsndJDaF";
    connectIot(productKey, deviceName, deviceSecret);

    return 1;
}



