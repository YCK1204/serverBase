#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/event.h>
#include <fcntl.h>
#include <sys/types.h>
#include <vector>
#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

int main() {
    unsigned short tmp = htons(8080);

    std::cout << (((tmp & 0xFF00) >> 8) | ((tmp & 0x00FF) << 8)) << std::endl;
}