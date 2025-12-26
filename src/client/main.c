#define _GNU_SOURCE

#include "../../include/packet.h"
#include "../../include/socket.h"
#include <arpa/inet.h>
#include <assert.h>
#include <ncurses.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define ADDRESS "127.0.0.1"
#define PORT "7465"

struct UserInfo {
  ConnectionID id;
};

static int handle_message(int fd, struct UserInfo user) {
  struct Packet packet;

  receive_packet(fd, &packet);

  switch (packet.type) {
  case MESSAGE:;
    struct MessageData *data = (struct MessageData *)packet.data;
    if (strcmp(user.id, data->to) == 0) {
      printf("%s: %s\n", data->from, data->content);
    } else {
      fprintf(stderr, "received a message not addressed to us\n");
    }
    break;
  default:
    break;
  }

  return 0;
}

int main(void) {
  const struct UserInfo user = {.id = "test-client-1"};

  int socket_fd = connect_client_socket(ADDRESS, PORT);

  if (socket_fd == -1) {
    perror("connect_client_socket");
    return EXIT_FAILURE;
  }

  char ip_str[INET6_ADDRSTRLEN];
  if (get_socket_peer_ip(socket_fd, ip_str, sizeof ip_str) != 0) {
    perror("get_socket_peer_ip");
    return EXIT_FAILURE;
  }

  int port = get_socket_peer_port(socket_fd);
  if (port == -1) {
    perror("get_socket_peer_port");
    return EXIT_FAILURE;
  }
  printf("client: connected to %s:%d\n", ip_str, port);

  struct Packet packet;
  packet.type = SET_ID;
  struct SetIDData data;
  memcpy(&data.new_id, user.id, sizeof(data.new_id));
  memcpy(&packet.data, &data, sizeof data);

  send(socket_fd, &packet, sizeof(packet), 0);

  struct pollfd poller;
  poller.fd = socket_fd;
  poller.events = POLLIN | POLLRDHUP;
  poller.revents = 0;
  struct pollfd pollfds[1] = {poller};
  while (1) {
    int num_events = poll(pollfds, 1, -1);
    if (num_events == -1) {
      perror("poll");
      return 1;
    }

    if (pollfds[0].revents & POLLIN) {
      handle_message(pollfds[0].fd, user);
      close(pollfds[0].fd);
      break;
    }

    if (pollfds[0].revents & POLLRDHUP) {
      close(pollfds[0].fd);
      break;
    }
  }

  return 0;
}
