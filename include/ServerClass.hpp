#pragma once

#include <arpa/inet.h>
#include <mongocxx/collection.hpp>
#include <thread>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#define PORT 8888

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

class ServerClass {
 public:
  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  mongocxx::instance instance{};  // This should be done only once.
  mongocxx::client client{mongocxx::uri{}};
  mongocxx::database db = client["cpp_chatroom"];
  mongocxx::collection collection = db["accounts"];

  std::vector<std::thread> client_vector;

  int init();
  int listen_for_connections();
  int client_handler(int client_id);
  int account_verification();
  int create_account();


};
