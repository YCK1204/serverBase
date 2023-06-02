#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void send_data(int clnt_sock, char* ct, char* file_name);
char* content_type(char* file);
void send_error(int clnt_sock);
void error_handling(char* message);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_size;
    int kq, nevents;
    struct kevent change_event, event;
    
    char buf[BUF_SIZE];
    if(argc!=2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    
    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");
    if(listen(serv_sock, 20)==-1)
        error_handling("listen() error");
    
    kq = kqueue();
    EV_SET(&change_event, serv_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(kq, &change_event, 1, NULL, 0, NULL);
    
    while(1)
    {
        nevents = kevent(kq, NULL, 0, &event, 1, NULL);
        if(nevents == -1)
            error_handling("kevent() error");
        
        if(event.ident == serv_sock) {
            clnt_adr_size = sizeof(clnt_adr);
            clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
            printf("Connection Request : %s:%d\n", 
                inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
            
            EV_SET(&change_event, clnt_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
            kevent(kq, &change_event, 1, NULL, 0, NULL);
        }
        else {
            clnt_sock = event.ident;
            int clnt_read = clnt_sock;
            int clnt_write = dup(clnt_sock);
            
            char req_line[SMALL_BUF];
            read(clnt_read, req_line, SMALL_BUF);
            if(strstr(req_line, "HTTP/") == NULL)
            {
                send_error(clnt_sock);
                close(clnt_read);
                close(clnt_write);
                EV_SET(&change_event, clnt_sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                kevent(kq, &change_event, 1, NULL, 0, NULL);
                close(clnt_sock);
                continue;
            }
            
            char method[10];
            char ct[15];
            char file_name[30];
            strcpy(method, strtok(req_line, " /"));
            strcpy(file_name, strtok(NULL, " /"));
            strcpy(ct, content_type(file_name));
            if(strcmp(method, "GET") != 0)
            {
                send_error(clnt_sock);
                close(clnt_read);
                close(clnt_write);
                EV_SET(&change_event, clnt_sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                kevent(kq, &change_event, 1, NULL, 0, NULL);
                close(clnt_sock);
                continue;
            }
            
            close(clnt_read);
            send_data(clnt_write, ct, file_name);
            close(clnt_write);
            EV_SET(&change_event, clnt_sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(kq, &change_event, 1, NULL, 0, NULL);
            close(clnt_sock);
        }
    }
    
    close(serv_sock);
    return 0;
}

void send_data(int clnt_sock, char* ct, char* file_name)
{
    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server: Linux Web Server\r\n";
    char cnt_len[] = "Content-length: 2048\r\n";
    char cnt_type[SMALL_BUF];
    char buf[BUF_SIZE];
    int send_file;
    
    sprintf(cnt_type, "Content-type: %s\r\n\r\n", ct);
    send_file = open(file_name, O_RDONLY);
    if(send_file == -1)
    {
        send_error(clnt_sock);
        return;
    }
    
    write(clnt_sock, protocol, strlen(protocol));
    write(clnt_sock, server, strlen(server));
    write(clnt_sock, cnt_len, strlen(cnt_len));
    write(clnt_sock, cnt_type, strlen(cnt_type));
    
    ssize_t read_len;
    while((read_len = read(send_file, buf, BUF_SIZE)) > 0)
    {
        write(clnt_sock, buf, read_len);
    }
}

char* content_type(char* file)
{
    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name, file);
    strtok(file_name, ".");
    strcpy(extension, strtok(NULL, "."));
    
    if(!strcmp(extension, "html") || !strcmp(extension, "htm"))
        return "text/html";
    else
        return "text/plain";
}

void send_error(int clnt_sock)
{
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char server[] = "Server: Linux Web Server\r\n";
    char cnt_len[] = "Content-length: 2048\r\n";
    char cnt_type[] = "Content-type: text/html\r\n\r\n";
    char content[] = "<html><head><title>NETWORK</title></head>"
                     "<body><font size=+5><br>잘못된 요청! 요청 페이지가 없습니다!"
                     "</font></body></html>";
    
    write(clnt_sock, protocol, strlen(protocol));
    write(clnt_sock, server, strlen(server));
    write(clnt_sock, cnt_len, strlen(cnt_len));
    write(clnt_sock, cnt_type, strlen(cnt_type));
    write(clnt_sock, content, strlen(content));
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
