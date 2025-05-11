#include <chrono>
#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define BUFFER_SIZE 4096

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./sender <Receiver IP> <File path>\n";
        return 1;
    }

    const char* server_ip = argv[1];
    const char* filepath = argv[2];

    // Open file
    std::ifstream input_file(filepath, std::ios::binary);
    if (!input_file) {
        std::cerr << "Cannot open file.\n";
        return 1;
    }

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return 1;
    }

    // Connect to server
    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }
    std::cout << "Connected to receiver.\n";

    // Send filename size and filename
    std::string filename = std::string(filepath).substr(std::string(filepath).find_last_of("/\\") + 1);
    uint32_t filename_size = htonl(filename.size());
    send(sock, &filename_size, sizeof(filename_size), 0);
    send(sock, filename.c_str(), filename.size(), 0);

    // Send file data
    char buffer[BUFFER_SIZE];
    auto start_time = std::chrono::high_resolution_clock::now();
    while (input_file.read(buffer, BUFFER_SIZE) || input_file.gcount() > 0) {
        send(sock, buffer, input_file.gcount(), 0);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();

    std::cout << "File sent successfully.\n";

    // Close connections
    input_file.close();
    close(sock);

    return 0;
}
