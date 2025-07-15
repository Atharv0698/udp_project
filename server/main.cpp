#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        return 1;
    }

    sockaddr_in servaddr{};
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    if (bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        return 1;
    }

    std::cout << "Started server, waiting for messages...\n";

    char buffer[1024];
    sockaddr_in clientaddr{};
    socklen_t len = sizeof(clientaddr);

    while (true) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                             (sockaddr*)&clientaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            break;
        }

        buffer[n] = '\0';
        std::cout << "Server received: " << buffer << std::endl;

        std::string reply = "Reply: Got it ";
        reply += buffer;

        // Replying to client
        ssize_t sent = sendto(sockfd, reply.c_str(), reply.size(), 0, (sockaddr*)&clientaddr, len);
        if (sent < 0) {
            perror("sendto failed");
            break;
        }

    }

    close(sockfd);
    return 0;
}
