#include <iostream>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int kq, sockfd1, sockfd2;
    struct kevent change_event, event_list[MAX_EVENTS];
    struct sockaddr_in addr1, addr2;
    char buffer[BUFFER_SIZE];

    // Create kqueue
    kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
        return 1;
    }

    // Create and bind socket 1
    sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd1 == -1) {
        perror("socket");
        return 1;
    }

    addr1.sin_family = AF_INET;
    addr1.sin_addr.s_addr = htonl(INADDR_ANY);
    addr1.sin_port = htons(8080); // Port 8080
    if (bind(sockfd1, (struct sockaddr*)&addr1, sizeof(addr1)) == -1) {
        perror("bind");
        return 1;
    }

    // Create and bind socket 2
    sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd2 == -1) {
        perror("socket");
        return 1;
    }

    addr2.sin_family = AF_INET;
    addr2.sin_addr.s_addr = htonl(INADDR_ANY);
    addr2.sin_port = htons(8081); // Port 8081
    if (bind(sockfd2, (struct sockaddr*)&addr2, sizeof(addr2)) == -1) {
        perror("bind");
        return 1;
    }

    // Set sockets to non-blocking mode
    set_nonblocking(sockfd1);
    set_nonblocking(sockfd2);

    // Register events for socket 1
    EV_SET(&change_event, sockfd1, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        perror("kevent");
        return 1;
    }

    // Register events for socket 2
    EV_SET(&change_event, sockfd2, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        perror("kevent");
        return 1;
    }

    while (true) {
        int num_events = kevent(kq, NULL, 0, event_list, MAX_EVENTS, NULL);
        if (num_events == -1) {
            perror("kevent");
            return 1;
        }

        for (int i = 0; i < num_events; ++i) {
            int sockfd = event_list[i].ident;

            // Accept new connections
            if (sockfd == sockfd1 || sockfd == sockfd2) {
                struct sockaddr_in client_addr;
                socklen_t client_addrlen = sizeof(client_addr);
                int client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addrlen);
                if (client_sockfd == -1) {
                    perror("accept");
                    return 1;
                }

                // Set client socket to non-blocking mode
                set_nonblocking(client_sockfd);

                // Register client socket for read events
                EV_SET(&change_event, client_sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
                    perror("kevent");
                    return 1;
                }

                std::cout << "New connection accepted. Socket: " << client_sockfd << std::endl;
            }
            // Read data from connected clients
            else {
                memset(buffer, 0, sizeof(buffer));
                int num_bytes = read(sockfd, buffer, sizeof(buffer));
                if (num_bytes == -1) {
                    perror("read");
                    return 1;
                } else if (num_bytes == 0) {
                    // Connection closed by client
                    std::cout << "Connection closed. Socket: " << sockfd << std::endl;
                    close(sockfd);
                } else {
                    // Process received data
                    std::cout << "Received data from socket " << sockfd << ": " << buffer << std::endl;
                }
            }
        }
    }

    return 0;
}
