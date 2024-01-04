#include <arpa/inet.h>
#include <stdio.h>
#include <memory>
#include <thread>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "../include/ServerClass.hpp"

#define PORT 8888

struct Client {};

int server_thread() {
  mongocxx::client client{mongocxx::uri{}};
  mongocxx::database db = client["cpp_chatroom"];
  mongocxx::collection collection = db["accounts"];

  ServerClass server(db);

  if (server.init() != 0)
    perror("Cannot init server");

  if (server.listen_for_connections() != 0)
    perror("Error while lisening to connection");
  return 0;
}

int main(int argc, char* argv[]) {

  mongocxx::instance instance{};  // This should be done only once.
  

  std::thread t(server_thread);

  t.join();
  return 0;
}
