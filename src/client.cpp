#include <sys/socket.h>
#include <thread>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

#include "../include/ClientClass.hpp"

int client_thread() {

  ClientClass client;
  client.connect_and_run();

  return 0;
}

// Driver Code
int main() {
  std::thread t(client_thread);

  t.join();
  return 0;
}
