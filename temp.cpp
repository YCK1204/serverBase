#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/event.h>

#define BUF_SIZE 1024
#define MAX_EVENTS 100
#define SMALL_BUF 2048

void send_data(int clnt_sock, char* ct, char* file_name);
char* content_type(char* file);
void send_error(int clnt_sock);
void error_handling(const char* message);

int main(int argc, char *argv[])
{
    int serv_sock[2], clnt_sock;
    struct sockaddr_in serv_adr[2], clnt_adr;
    socklen_t clnt_adr_size;
    char buf[BUF_SIZE];

    if(argc!=3) {
        printf("Usage : %s <port1> <port2>\n", argv[0]);
        exit(1);
    }

    for (int i = 0; i < 2; ++i) {
        serv_sock[i] = socket(PF_INET, SOCK_STREAM, 0);
        memset(&serv_adr[i], 0, sizeof(serv_adr[i]));
        serv_adr[i].sin_family = AF_INET;
        serv_adr[i].sin_addr.s_addr = htonl(INADDR_ANY);
        serv_adr[i].sin_port = htons(atoi(argv[i+1]));
        if(bind(serv_sock[i], (struct sockaddr*)&serv_adr[i], sizeof(serv_adr[i]))==-1)
            error_handling("bind() error");
        if(listen(serv_sock[i], 20)==-1)
            error_handling("listen() error");
    }

    int kq = kqueue();
    if (kq == -1) {
        error_handling("kqueue() error");
    }

    struct kevent evlist[MAX_EVENTS];
    struct kevent changelist[2];
    for (int i = 0; i < 2; ++i) {
        EV_SET(&changelist[i], serv_sock[i], EVFILT_READ, EV_ADD, 0, 0, NULL);
        kevent(kq, &changelist[i], 1, NULL, 0, NULL);
    }

    while(1)
    {
        int nevents = kevent(kq, NULL, 0, evlist, MAX_EVENTS, NULL);
        if (nevents == -1) {
            error_handling("kevent() error");
        }

        for (int i = 0; i < nevents; ++i) {
            int sockfd = evlist[i].ident;
            if (sockfd == serv_sock[0] || sockfd == serv_sock[1]) {
                clnt_adr_size=sizeof(clnt_adr);
                clnt_sock=accept(sockfd, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
                printf("Connection Request : %s:%d\n",
                    inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
                EV_SET(&changelist[i], clnt_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kq, &changelist[i], 1, NULL, 0, NULL);
            } else {
                char req_line[BUF_SIZE];
                ssize_t str_len = read(sockfd, req_line, BUF_SIZE);
                if (str_len == 0) {
                    close(sockfd);
                    continue;
                }

                char method[10];
                char file_name[30];
                sscanf(req_line, "%s %s", method, file_name);

                if (strcmp(method, "GET") != 0) {
                    send_error(sockfd);
                    close(sockfd);
                    continue;
                }

                send_data(sockfd, content_type(file_name), file_name);
                close(sockfd);
            }
        }
    }

    for (int i = 0; i < 2; ++i) {
        close(serv_sock[i]);
    }
    return 0;
}

void send_data(int clnt_sock, char* ct, char* file_name)
{
    char protocol[]="HTTP/1.0 200 OK\r\n";
    char server[]="Server:Linux Web Server \r\n";
    char cnt_len[]="Content-length:2048\r\n";
    char cnt_type[SMALL_BUF];
    char buf[BUF_SIZE];
    FILE* send_file;

    sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
    send_file=fopen(file_name, "r");
    if(send_file==NULL)
    {
        send_error(clnt_sock);
        return;
    }

    /* 헤더 정보 전송 */
    write(clnt_sock, protocol, strlen(protocol));
    write(clnt_sock, server, strlen(server));
    write(clnt_sock, cnt_len, strlen(cnt_len));
    write(clnt_sock, cnt_type, strlen(cnt_type));

    /* 요청 데이터 전송 */
    while(fgets(buf, BUF_SIZE, send_file)!=NULL) 
    {
        write(clnt_sock, buf, strlen(buf));
    }

    fclose(send_file);
}

char* content_type(char* file)
{
    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name, file);
    strtok(file_name, ".");
    strcpy(extension, strtok(NULL, "."));

    if(!strcmp(extension, "html")||!strcmp(extension, "htm")) 
        return "text/html";
    else
        return "text/plain";
}

void send_error(int clnt_sock)
{   
    char protocol[]="HTTP/1.0 400 Bad Request\r\n";
    char server[]="Server:Linux Web Server \r\n";
    char cnt_len[]="Content-length:2048\r\n";
    char cnt_type[]="Content-type:text/html\r\n\r\n";
    char content[]="<html><head><title>NETWORK</title></head>"
           "<body><font size=+5><br>오류 발생! 요청 파일명 및 요청 방식 확인!"
           "</font></body></html>";

    write(clnt_sock, protocol, strlen(protocol));
    write(clnt_sock, server, strlen(server));
    write(clnt_sock, cnt_len, strlen(cnt_len));
    write(clnt_sock, cnt_type, strlen(cnt_type));
    write(clnt_sock, content, strlen(content));
}

void error_handling(const char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}