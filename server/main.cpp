#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

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

    std::cout << "[SERVER] Listening for UDP messages...\n";

    char buffer[1024];
    sockaddr_in clientaddr{};
    socklen_t len = sizeof(clientaddr);

    int last_sequence = -1;

    while (true) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                            (sockaddr*)&clientaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            break;
        }

        buffer[n] = '\0';  // Null-terminate buffer

        std::cout << "[SERVER] Received: " << buffer << std::endl;

        int received_seq = -1;
        if (sscanf(buffer, "SEQ:%d", &received_seq) == 1) {
            if (received_seq > last_sequence) {
                std::cout << "[SERVER] Sequence valid, Current: " << received_seq << "[SERVER] Sequence valid, last: "<< last_sequence<<std::endl;
                last_sequence = received_seq;
            } else {
                std::cout << "[SERVER] Out-of-order or repeated sequence. Last: " << last_sequence << ", Received: " << received_seq << std::endl;
            }
        } else {
            std::cout << "[SERVER] Could not parse sequence number." << std::endl;
        }
    }

    close(sockfd);
    return 0;
}
