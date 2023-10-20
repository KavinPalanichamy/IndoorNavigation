#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int tcp_connection(char* message) {

    int clientSocket_send = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket_send == -1) {
        perror("Error creating socket");
        return 1;
    }
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(4568);  
    serverAddress.sin_addr.s_addr = inet_addr("10.2.1.223");  

    if (connect(clientSocket_send, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error connecting to the server");
        close(clientSocket_send);
        return 1;
    }

    std::cout<<"Hello";
    
    if (send(clientSocket_send, message, strlen(message), 0) == -1) {
        perror("Error sending data to the server");
        close(clientSocket_send);
        return 1;
    }
  
    char buffer[1024];
    int bytesRead = recv(clientSocket_send, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        perror("Error receiving data from the server");
        close(clientSocket_send);
        return 1;
    }
    buffer[bytesRead] = '\0';

    std::cout << "Received from server: " << buffer << std::endl;
   
    close(clientSocket_send);

    return 0;
}
