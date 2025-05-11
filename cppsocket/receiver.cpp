#include <chrono>
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 4096


int main()
{
    int server_fd, client_socket;
    sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen for connections
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "Waiting for connection...\n";

    // Accept connection
    if ((client_socket = accept(server_fd, (sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        return 1;
    }

    // Receive filename size
    uint32_t filename_size_net;
    recv(client_socket, &filename_size_net, sizeof(filename_size_net), 0);
    uint32_t filename_size = ntohl(filename_size_net);

    // Receive filename
    char filename[256] = {0};
    auto rbfn = recv(client_socket, filename, filename_size, 0);
    std::cout << "Received bytes for filename: " << rbfn << std::endl;
    if (rbfn <= 0) {
        perror("Filename receive failed");
        return 1;
    }
    std::string filename_str(filename);
    filename_str += "_COPY";
    std::strcpy(filename, filename_str.c_str());
    std::cout << "Receiving file: " << filename << std::endl;

    // Open file
    std::ofstream output_file(filename, std::ios::binary);

    // Receive file data
    char buffer[BUFFER_SIZE];
    int bytes_received;
    auto start_time = std::chrono::high_resolution_clock::now();
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        output_file.write(buffer, bytes_received);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();

    std::cout << "File received successfully.\n";
    std::cout << "File size: " << output_file.tellp() << " bytes\n";
    std::cout << "Received file saved as: " << filename << std::endl;
    std::cout << "Received bytes: " << bytes_received << std::endl;
    std::cout << "Time taken: " << duration << " seconds\n";

    // Close connections
    output_file.close();
    close(client_socket);
    close(server_fd);

    return 0;
}
