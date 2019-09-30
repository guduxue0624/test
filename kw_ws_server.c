#include <stdio.h>
#include <string.h>
#include "websocket_common.h"
#include "server_base.h"

enum{
    WEB_MSG_DATA_COMMON_TYPE = 0,
    WEB_MSG_DATA_AUDIO_TYPE,
    WEB_MSG_MESSAGE_HANDS,
    WEB_MSG_MESSAGE_RING,
    WEB_MSG_MESSAGE_RING_END,
    WEB_MSG_MESSAGE_FACE,
    WEB_MSG_MESSAGE_PLAY_END,
    WEB_MSG_DATA_AUDIO_TYPE_PCM,
}; 

typedef struct web_msg{
    char header;
    int len;
    void *data;
}kw_web_msg_t;


static int kw_recv_hello(int fd,char *buf,int buf_size)
{
    char recv_word[16] = {0};
    int ret = 0;
    if (strncmp(buf,"Hi~ World",strlen("Hi~ World")) == 0){
        strncpy(recv_word,"hello world",strlen("hello world") + 1);
        ret = 0;
    }
    else{
        printf("hello error!\n");
        strncpy(recv_word,"hello error",strlen("hello error")+1);
        ret = -1;
    }

    int recv_len = strlen(recv_word);
    char recv_data[128] = {0};
    recv_data[0] = WEB_MSG_MESSAGE_HANDS;
    recv_data[1] = (recv_len >> 24) & 0xff;
    recv_data[2] = (recv_len >> 16) & 0xff;
    recv_data[3] = (recv_len >>  8) & 0xff;
    recv_data[4] = (recv_len) & 0xff;
    strncpy(recv_data+5,recv_word,recv_len); 

    ret = webSocket_send(fd, recv_data, strlen(recv_data), false, WDT_BINDATA);

    return ret;
}

static int kw_play_pcm(char *buf,int buf_size,char type)
{
    printf("get pcm len is %d\n",buf_size);
    int i = 0;
    for ( i = 0; i < buf_size; i++){
        printf(" %x ",buf[i]);
    }
    printf("\n");

    FILE *file = fopen("my_test.pcm","ab+");
    fwrite(buf,buf_size,1,file);
    fclose(file);
    return 0;
}


int kw_server_callBack(int fd, char *buf, unsigned int bufLen)
{
    int ret;
    ret = webSocket_recv(fd , buf , bufLen, NULL);    // 使用websocket recv
    if(ret > 0)
	{
        kw_web_msg_t msg;
        char *data = buf;
        msg.header = data[0];
        memcpy(&msg.len,data+1,4);
//        msg.len = ntohl(msg.len);      //转换网络字节序为本地字节序
        msg.data = data + 5;

        if (bufLen < msg.len + 5){
            printf("buf too short!\n");
            return -1;
        }
        switch (msg.header)
        {
        case WEB_MSG_DATA_COMMON_TYPE:

            break;
        case WEB_MSG_DATA_AUDIO_TYPE:
        
            break;
        case WEB_MSG_MESSAGE_HANDS:
            kw_recv_hello(fd,(char *)msg.data,msg.len);
            break;
        case WEB_MSG_MESSAGE_RING:
    
            break;
        case WEB_MSG_MESSAGE_RING_END:
            break;
        case WEB_MSG_MESSAGE_FACE:
            break;
        case WEB_MSG_MESSAGE_PLAY_END:
            break;
        case WEB_MSG_DATA_AUDIO_TYPE_PCM:
            kw_play_pcm((char *)msg.data,msg.len,0);
            break;
        default:
            break;
        }
	}
    else if(ret < 0)
    {
        if(strncmp(buf, "GET", 3) == 0)	//握手,建立连接
            ret = webSocket_serverLinkToClient(fd, buf, ret);
    }
	return ret;
}