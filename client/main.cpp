#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int main() {
    std::cout << "[CLIENT] Starting periodic UDP sender...\n";

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        return 1;
    }

    struct addrinfo hints{}, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int status = getaddrinfo("server", nullptr, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        close(sockfd);
        return 1;
    }

    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr = ((sockaddr_in*)res->ai_addr)->sin_addr;


    int sequence = 0;

    while (true) {
        std::string message = "SEQ:" + std::to_string(sequence);
        ssize_t sent = sendto(sockfd, message.c_str(), message.size(), 0,
                              (const struct sockaddr*)&servaddr, sizeof(servaddr));

        if (sent < 0) {
            perror("send operation failed");
            break;
        }

        std::cout << "[CLIENT] Sent message: " << message << std::endl;
       
        
        sequence++;

        sleep(100);

    }

    freeaddrinfo(res);
    close(sockfd);
    return 0;
}
