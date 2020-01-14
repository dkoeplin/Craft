#ifndef _client_h_
#define _client_h_

#include "craft/world/State.h"

struct Client {
 public:
  void connect(char *hostname, int port);
  void start();
  void stop();
  void send(char *data);
  char *recv();
  void version(int version);
  void login(const char *username, const char *identity_token);
  void position(State &state);
  void chunk(int p, int q, int key);
  void block(int x, int y, int z, int w);
  void light(int x, int y, int z, int w);
  void sign(int x, int y, int z, int face, const char *text);
  void talk(const char *text);

 private:
  friend int recv_worker(void *arg);

  int sendall(int sendn, char *data, int length);

  bool running = false;
  int sd = 0;
  int bytes_sent = 0;
  int bytes_received = 0;
  char *queue = 0;
  int qsize = 0;
};

#endif
