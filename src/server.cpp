#include <arpa/inet.h>
#include <stdio.h>
#include <thread>

#include "../include/ServerClass.hpp"

#define PORT 8888

struct Client {};

int server_thread() {
  ServerClass server;

  if (server.init() != 0)
    perror("Cannot init server");

  if (server.listen_for_connections() != 0)
    perror("Error while lisening to connection");
  return 0;
}

int main(int argc, char* argv[]) {
  std::thread t(server_thread);

  t.join();
  return 0;
}
