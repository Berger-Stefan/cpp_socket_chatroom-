#pragma once

#include <arpa/inet.h>
#include <stdio.h>
#include <mongocxx/collection.hpp>
#include <stdexcept>
#include <thread>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#define LOGIN_PORT 8888

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

struct FreePorts {
  std::vector<std::pair<const unsigned int, bool>> ports;

  FreePorts() {
    addPort(8889);
    addPort(8890);
    addPort(8891);
    addPort(8893);
  }

  void addPort(const unsigned int port_number) { ports.push_back(std::make_pair(port_number, true)); }

  const unsigned int get_free_port() {
    for (auto& port : ports) {
      if (port.second == true) {
        occupy_port(port.first);
        return port.first;
      }
    }
    throw std::runtime_error("Cannot find free port");
  }

  void occupy_port(const int port_number) {
    for (auto& port : ports) {
      if (port.first == port_number) {
        port.second = false;
        return;
      }
    }
    throw std::runtime_error("Cannot find port to occupy");
  }

  void free_port(const int port_number) {
    for (auto& port : ports) {
      if (port.first == port_number) {
        port.second = true;
        return;
      }
    }
    throw std::runtime_error("Cannot find port to occupy");
  }
};

class ServerClass {
 public:
  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  FreePorts free_ports;

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
