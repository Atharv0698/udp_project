#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int main() {

    //initialze socket 
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cout<<"[CLIENT] Failed to create socket";
        return 1;
    }
    
    //define address IPV4 for communication, UDP Socket
    struct addrinfo hints{}, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int status = getaddrinfo("server", nullptr, &hints, &res);
    if (status != 0) {
        std::cout << " [Client] addr info error" << std::endl;
        close(sockfd);
        return 1;
    }
    
    //define server address
    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr = ((sockaddr_in*)res->ai_addr)->sin_addr;
    
    std::cout << "[Client] Starting periodic UDP sender...\n";

    int sequence = 0;

    while (true) {

        //send message
        std::string message = "SEQUENCE:" + std::to_string(sequence);
        size_t sent = sendto(sockfd, message.c_str(), message.size(), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));

        if (sent < 0) {
            std::cout<<"[CLIENT] send operation failed";
            break;
        }

        std::cout << "[CLIENT] Sent message: " << message << std::endl;
             
        sequence++;

        sleep(100); // Set to 0.01 Hz

    }

    freeaddrinfo(res);
    close(sockfd);
    return 0;
}
