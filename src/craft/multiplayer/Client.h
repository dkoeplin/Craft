#ifndef _client_h_
#define _client_h_

#include "craft/world/Sign.h"
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
    void chunk(const ChunkPos &pos, int key) { return chunk(pos.x, pos.z, key); }

    void block(int x, int y, int z, int w);
    void block(const Block &b) { block(b.x, b.y, b.z, b.w); }

    void light(int x, int y, int z, int w);
    void light(const ILoc3 &loc, int w) { light(loc.x, loc.y, loc.z, w); }

    void sign(int x, int y, int z, int face, const char *text);
    void sign(const Sign &s) { sign(s.x, s.y, s.z, s.side, s.text.c_str()); }

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
