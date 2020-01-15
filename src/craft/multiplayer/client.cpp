#include "client.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #define close closesocket
    #define sleep Sleep
#else
    #include <netdb.h>
    #include <unistd.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>

extern "C" {
#include "tinycthread.h"
}

#include "craft/util/Logging.h"

#define QUEUE_SIZE 1048576
#define RECV_SIZE 4096

static thrd_t recv_thread;
static mtx_t mutex;

int Client::sendall(int sendn, char *data, int length) {
    int count = 0;
    while (count < length) {
        int n = ::send(sendn, data + count, length, 0);
        if (n == -1) {
            return -1;
        }
        count += n;
        length -= n;
        bytes_sent += n;
    }
    return 0;
}

void Client::send(char *data) {
    int result = sendall(sd, data, strlen(data));
    REQUIRE(result != -1, "send encountered error");
}

void Client::version(int version) {
    char buffer[1024];
    snprintf(buffer, 1024, "V,%d\n", version);
    send(buffer);
}

void Client::login(const char *username, const char *identity_token) {
    char buffer[1024];
    snprintf(buffer, 1024, "A,%s,%s\n", username, identity_token);
    send(buffer);
}

void Client::position(State &state) { if (running) {
    static State prev;
    float distance =
        (prev.x - state.x) * (prev.x - state.x) +
        (prev.y - state.y) * (prev.y - state.y) +
        (prev.z - state.z) * (prev.z - state.z) +
        (prev.rx - state.rx) * (prev.rx - state.rx) +
        (prev.ry - state.ry) * (prev.ry - state.ry);
    if (distance < 0.0001) {
        return;
    }
    prev = state;
    char buffer[1024];
    snprintf(buffer, 1024, "P,%.2f,%.2f,%.2f,%.2f,%.2f\n", state.x, state.y, state.z, state.rx, state.ry);
    send(buffer);
}}

void Client::chunk(int p, int q, int key) { if (running) {
    char buffer[1024];
    snprintf(buffer, 1024, "C,%d,%d,%d\n", p, q, key);
    send(buffer);
}}

void Client::block(int x, int y, int z, int w) { if (running) {
    char buffer[1024];
    snprintf(buffer, 1024, "B,%d,%d,%d,%d\n", x, y, z, w);
    send(buffer);
}}

void Client::light(int x, int y, int z, int w) { if (running) {
    char buffer[1024];
    snprintf(buffer, 1024, "L,%d,%d,%d,%d\n", x, y, z, w);
    send(buffer);
}}

void Client::sign(int x, int y, int z, int face, const char *text) { if (running) {
    char buffer[1024];
    snprintf(buffer, 1024, "S,%d,%d,%d,%d,%s\n", x, y, z, face, text);
    send(buffer);
}}

void Client::talk(const char *text) { if (running) {
    if (strlen(text) == 0) {
        return;
    }
    char buffer[1024];
    snprintf(buffer, 1024, "T,%s\n", text);
    send(buffer);
}}

char *Client::recv() { if (running) {
    char *result = nullptr;
    mtx_lock(&mutex);
    char *p = queue + qsize - 1;
    while (p >= queue && *p != '\n') {
        p--;
    }
    if (p >= queue) {
        int length = p - queue + 1;
        result = (char *)malloc(sizeof(char) * (length + 1));
        memcpy(result, queue, sizeof(char) * length);
        result[length] = '\0';
        int remaining = qsize - length;
        memmove(queue, p + 1, remaining);
        qsize -= length;
        bytes_received += length;
    }
    mtx_unlock(&mutex);
    return result;
} else return nullptr; }

int recv_worker(void *arg) {
    char *data = (char *)malloc(sizeof(char) * RECV_SIZE);
    auto *client = (Client *)arg;
    while (1) {
        int length;
        if ((length = recv(client->sd, data, RECV_SIZE - 1, 0)) <= 0) {
            REQUIRE(!client->running, "Receive: already running.");
            break;
        }
        data[length] = '\0';
        while (1) {
            int done = 0;
            mtx_lock(&mutex);
            if (client->qsize + length < QUEUE_SIZE) {
                memcpy(client->queue + client->qsize, data, sizeof(char) * (length + 1));
                client->qsize += length;
                done = 1;
            }
            mtx_unlock(&mutex);
            if (done) {
                break;
            }
            sleep(0);
        }
    }
    free(data);
    return 0;
}

void Client::connect(char *hostname, int port) {
    struct hostent *host;
    struct sockaddr_in address;
    host = gethostbyname(hostname);
    REQUIRE(host, "Unable to get host");

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = ((struct in_addr *)(host->h_addr_list[0]))->s_addr;
    address.sin_port = htons(port);
    sd = socket(AF_INET, SOCK_STREAM, 0);
    REQUIRE(sd != -1, "socket failed");

    int result = ::connect(sd, (struct sockaddr *)&address, sizeof(address));
    REQUIRE(result != -1, "connect failed");
}

void Client::start() {
    running = true;
    queue = (char *)calloc(QUEUE_SIZE, sizeof(char));
    qsize = 0;
    mtx_init(&mutex, mtx_plain);
    int result = thrd_create(&recv_thread, recv_worker, this);
    REQUIRE(result == thrd_success, "Unable to create client thread.");
}

void Client::stop() {
    running = false;
    close(sd);
    // if (thrd_join(recv_thread, NULL) != thrd_success) {
    //     perror("thrd_join");
    //     exit(1);
    // }
    // mtx_destroy(&mutex);
    qsize = 0;
    free(queue);
    // printf("Bytes Sent: %d, Bytes Received: %d\n",
    //     bytes_sent, bytes_received);
}
