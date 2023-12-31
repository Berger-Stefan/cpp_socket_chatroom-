#include "../include/ServerClass.hpp"

#include <stdio.h>
#include <stdexcept>
#include <thread>

int ServerClass::init() {
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);

  // Bind the socket to the address and port number.
  if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
    throw std::runtime_error("cannot bind to server");
  }

  // Listen on the socket,
  if (listen(serverSocket, 50) == 0)
    printf("Listening\n");
  else
    throw std::runtime_error("cannot listen to port");
  return 0;
}

int ServerClass::listen_for_connections() {
  while (1) {
    newSocket = accept(serverSocket, (struct sockaddr*)&serverStorage, &addr_size);

    std::thread t(&ServerClass::client_handler, this, newSocket);
    client_vector.push_back(std::move(t));
    printf("New client_handler thread started\n");
  }
}

int ServerClass::client_handler(int client_id) {
  printf("Got new Connection\n");
  char input_buffer[1000];

  while (1) {
    int bytes_received = recv(client_id, &input_buffer, sizeof(input_buffer), 0);
    if (bytes_received < 1)
      continue;
    input_buffer[bytes_received] = '\0';
    printf("Client %d:\t%s\n", client_id, input_buffer);
  }
  return 0;
}
