#include "../include/ClientClass.hpp"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <stdexcept>

#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8888

// Function to send data to
// server socket.
void ClientClass::connect_and_run() {

  // Create a stream socket
  network_socket = socket(AF_INET, SOCK_STREAM, 0);

  // Initialise port number and address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT);

  int connection_status = connect(network_socket, (struct sockaddr*)&server_address, sizeof(server_address));
  if (connection_status < 0) {
    throw std::runtime_error("Cannot connect to server");
    return;
  }

  // Get new port
  int new_port;
  recv(network_socket, &new_port, sizeof(new_port), 0);
  printf("New Port: %d\n", new_port);

  // Close old connection and start new one
  close(network_socket);

  int new_network_socket = socket(AF_INET, SOCK_STREAM, 0);
  server_address.sin_port = htons(new_port);
  connection_status = connect(new_network_socket, (struct sockaddr*)&server_address, sizeof(server_address));
  if (connection_status < 0) {
    perror("Cannot connect to server");
  }
  printf("Connection established\n");

  while (1) {
    printf("Input:\t");
    std::string buffer;
    std::getline(std::cin, buffer);
    if (!strcmp("exit", buffer.data()))
      break;
    send(new_network_socket, buffer.data(), size(buffer), 0);
  }

  // Close the connection
  close(network_socket);

  return;
}
