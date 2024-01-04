#pragma once

#include <arpa/inet.h>
#include <memory>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
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
    throw std::runtime_error("Cannot find port to free");
  }
};

class ServerClass {
 public:
  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  FreePorts free_ports;
  mongocxx::database& db;
  mongocxx::collection db_accounts = db["accounts"];
  std::vector<std::thread> client_vector;

  ServerClass(mongocxx::database& db):  db(db){}


  int init();
  int listen_for_connections();
  int client_handler(int client_id);
  int account_verification();
  int create_account();
};
