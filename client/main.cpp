#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>   // For getaddrinfo

int main() {
    std::cout << "Sending message to server..." << std::endl;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        return 1;
    }

    struct addrinfo hints{}, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
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

    const char* message = "Hi from client!";
    ssize_t sent = sendto(sockfd, message, strlen(message), 0,
                          (const struct sockaddr*)&servaddr, sizeof(servaddr));
    if (sent < 0) {
        perror("send operation failed");
        freeaddrinfo(res);
        close(sockfd);
        return 1;
    }

    std::cout << "Message sent." << std::endl;

    freeaddrinfo(res);
    close(sockfd);
    return 0;
}
