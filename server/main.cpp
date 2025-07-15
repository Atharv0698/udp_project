#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

int main() {

    //initialize a socket for communication
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cout<<"[SERVER] Failed to create socket";
        return 1;
    }
    
    //create server address structure
    sockaddr_in servaddr{};
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    //Binding socket to the port
    if (bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cout<<"[SERVER] bind failed";
        return 1;
    }

    std::cout << "[SERVER] listening to UDP packets\n";

    char buffer[1024];
    sockaddr_in clientaddr{};
    socklen_t len = sizeof(clientaddr);
    
    // initilize a storing variable
    int last_sequence = -1;

    while (true) {

        // receive client messages
        size_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,(sockaddr*)&clientaddr, &len);
        
        if (n < 0) {
            std::cout<<"[SERVER] recieve from client failed";
            break;
        }

        buffer[n] = '\0'; 

        std::cout << "[SERVER] Received: " << buffer << std::endl;

        int received_seq = -1;

        if (sscanf(buffer, "SEQUENCE:%d", &received_seq) == 1) 
        {
            if (received_seq > last_sequence) {
                std::cout << "[SERVER] Sequence valid, Current: " << received_seq << ", Previous: "<< last_sequence<<std::endl;
                last_sequence = received_seq;
            } 
            else if (received_seq == last_sequence) {
                std::cout << "[SERVER] Repeated sequence. Previous: " << last_sequence << ", Received: " << received_seq << std::endl;
            }
            else{
                std::cout << "[SERVER] Out of Order"<<std::endl;
                last_sequence = received_seq;
            }
        } else {
            std::cout << "[SERVER] Could not parse sequence number." << std::endl;
        }
    }

    close(sockfd);
    return 0;
}
