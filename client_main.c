
#include "websocket_common.h"

#include <stdio.h>
#include <stdlib.h> //exit()
#include <string.h>
#include <errno.h>
#include <unistd.h> //getpid

char ip[24] = {0};//"172.16.23.160";// 本机IP
int port = 8000;

int main(void)
{
    int ret, timeCount = 0;
    int fd;
    char buff[10240];

    strncpy(ip,"192.168.31.61",strlen("192.168.31.61"));
    if((fd = webSocket_clientLinkToServer(ip, port, "/null")) <= 0)
    {
        printf("client link to server failed !\r\n");
        return -1;
    }
    webSocket_delayms(100);
    //
    memset(buff, 0, sizeof(buff));
    char *word = "Hi~ World";
    int recv_len = strlen(word);

    buff[0] = 2;
    buff[1] = (recv_len >> 24) & 0xff;
    buff[2] = (recv_len >> 16) & 0xff;
    buff[3] = (recv_len >>  8) & 0xff;
    buff[4] = (recv_len) & 0xff;
    strncpy(buff+5,word,recv_len); 

    webSocket_send(fd, buff, strlen(buff), true, WDT_BINDATA);
    int is_file_send = 0;
    FILE *file = fopen("abc.pcm","r");
    if (file == NULL){
        is_file_send = 1;
    }
    
    //
    while(1)
    {
        //
        memset(buff, 0, sizeof(buff));
        ret = webSocket_recv(fd, buff, sizeof(buff), NULL);
        if(ret > 0)
        {
            //===== 与服务器的应答 =====
            char header = buff[0];
            int len;
            memcpy(&len,buff+1,4);
            char *data = buff + 5;
            if (header == 2){
                printf("the data is %s\n",data);
            }
            
            memset(buff,0,strlen(buff));
            if (!feof(file) && is_file_send == 0){
                char data[512] = {0};
                ret = fread(data,128,1,file);
                buff[0] = 2;
                buff[1] = (ret >> 24) & 0xff;
                buff[2] = (ret >> 16) & 0xff;
                buff[3] = (ret >>  8) & 0xff;
                buff[4] = (ret) & 0xff;
                strncpy(buff+5,data,ret); 
                ret = webSocket_send(fd, buff, strlen(buff), true, WDT_TXTDATA);
                if (feof(file)){
                    is_file_send = 1;
                }
                if(ret <= 0){
                    close(fd);     // send返回异常, 连接已断开
                    break;
                }

            }
        }
        else    // 检查错误, 是否连接已断开
        {
            if(errno == EAGAIN || errno == EINTR)
                ;
            else
            {
                close(fd);
                break;
            }
        }
        
        //===== 3s客户端心跳 =====
        if(timeCount > 3000)   
        {
            timeCount = 10;
            //
            memset(buff, 0, sizeof(buff));

            // sprintf(buff, "heart from client(%d)", pid);
            // ret = webSocket_send(fd, buff, strlen(buff), true, WDT_TXTDATA);

            strcpy(buff, "123");//即使ping包也要带点数据
            ret = webSocket_send(fd, buff, strlen(buff), true, WDT_PING); //使用ping包代替心跳

            if(ret <= 0)
            {
                close(fd);
                break;
            }
        }
        else
            timeCount += 10;

        //
        webSocket_delayms(10);
    }
    printf("client close !\r\n");
    return 0;
}

