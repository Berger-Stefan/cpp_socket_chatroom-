#include "../include/ServerClass.hpp"

#include <stdio.h>
#include <stdexcept>
#include <thread>

int ServerClass::init() {
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(LOGIN_PORT);

  if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
    throw std::runtime_error("cannot bind to server");
  }

  if (listen(serverSocket, 50) == 0)
    printf("Listening\n");
  else
    throw std::runtime_error("cannot listen to port");
  return 0;
}

int ServerClass::listen_for_connections() {
  while (1) {
    newSocket = accept(serverSocket, (struct sockaddr*)&serverStorage, &addr_size);
    // Send port for connection
    const unsigned int current_free_port = free_ports.get_free_port();
    send(newSocket, &current_free_port, sizeof(current_free_port), 0);

    int new_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in new_addr = serverAddr;
    new_addr.sin_port = htons(current_free_port);

    if (bind(new_server_socket, (struct sockaddr*)&new_addr, sizeof(new_addr)) != 0) {
      throw std::runtime_error("cannot bind to server");
    }

    listen(new_server_socket, 5);

    int new_client_socket = accept(new_server_socket, nullptr, nullptr);

    std::thread t(&ServerClass::client_handler, this, new_client_socket);
    printf("New client_handler thread started\n");

    client_vector.push_back(std::move(t));
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
