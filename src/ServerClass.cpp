#include "../include/ServerClass.hpp"

#include <stdio.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <thread>
#include <unordered_map>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

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

  // User authentification
  std::string message = "Username:\t";
  send(client_id, message.data(), size(message), 0);
  recv(client_id, &input_buffer, sizeof(input_buffer), 0);
  std::string user_name = input_buffer;
  printf("Username:\t %s\n", user_name.c_str());
  memset(&input_buffer[0], 0, sizeof(input_buffer));

  message = "Password:\t";
  send(client_id, message.data(), size(message), 0);
  recv(client_id, &input_buffer, sizeof(input_buffer), 0);
  std::string password = input_buffer;
  printf("Password:\t %s\n", password.c_str());
  memset(&input_buffer[0], 0, sizeof(input_buffer));

  // Check if account with this name exists:
  auto query_result = db_accounts.find_one(make_document(kvp("name", user_name)));
  if (query_result) {
    printf("Account found \n");
    // Check keyword hash
    size_t password_hash = std::hash<std::string>{}(password);
    unsigned int pw = static_cast<int32_t>(password_hash);

    bsoncxx::document::view query_result_view = query_result->view();
    auto pw_db_value = query_result_view["password_hash"];

    // Check if the field exists in the document
    size_t pw_db = static_cast<u_int32_t>(pw_db_value.get_int32());

    if (pw != pw_db) {
      printf("Wrong Password\n");
      message = "Passwort is wrong! Try connecting again.\n";
      send(client_id, message.data(), size(message), 0);
      memset(&input_buffer[0], 0, sizeof(input_buffer));
      close(client_id);
      // free_ports.free_port(client_id);
    } else {
      // Password is correct
      message = "Passwort is correct, welcome back " + user_name + "!\n";
      send(client_id, message.data(), size(message), 0);
      memset(&input_buffer[0], 0, sizeof(input_buffer));
    }
  } else {
    // Create Account
    printf("Create Account \n");
    size_t password_hash = std::hash<std::string>{}(password);
    printf("pw hash %u\n", password_hash);

    // Create a BSON document directly
    auto account = bsoncxx::builder::basic::document{};
    account.append(kvp("name", user_name));
    account.append(kvp("password_hash", static_cast<int32_t>(password_hash)));

    auto insert_one_result = db_accounts.insert_one(account.extract());

    message = "New account has been created";
    send(client_id, message.data(), size(message), 0);
    memset(&input_buffer[0], 0, sizeof(input_buffer));
  }

  while (1) {
    int bytes_received = recv(client_id, &input_buffer, sizeof(input_buffer), 0);
    if (bytes_received < 1)
      continue;
    input_buffer[bytes_received] = '\0';
    printf("%s :\t%s\n", user_name.c_str(), input_buffer);
    memset(&input_buffer[0], 0, sizeof(input_buffer));
  }
  return 0;
}
