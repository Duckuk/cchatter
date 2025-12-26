#define _GNU_SOURCE

#include "../../include/packet.h"
#include "../../include/socket.h"
#include "../../include/vec.h"
#include "sys/poll.h"
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ADDRESS "127.0.0.1"
#define PORT "7465"
#define BACKLOG 10

#define MAX_CONNECTIONS 1024

struct Connection {
  ConnectionID id;
  struct sockaddr_storage socket_address;
  int socket_fd;
};

static void child_handler(int s) {
  while (waitpid(-1, NULL, WNOHANG) > 0) {
  }
}

static int find_connection_list_fd(struct vec *connection_list, int fd) {
  for (size_t i = 0; i < connection_list->len; i++) {
    if (((struct Connection *)vec_get(connection_list, i))->socket_fd == fd) {
      return i;
    }
  }

  return -1;
}

static int find_connection_list_id(struct vec *connection_list,
                                   ConnectionID id) {
  for (size_t i = 0; i < connection_list->len; i++) {
    ConnectionID *this_id =
        &((struct Connection *)vec_get(connection_list, i))->id;
    if (strcmp(*this_id, id) == 0) {
      return i;
    }
  }

  return -1;
}

static int accept_and_create_connection(struct vec *connections_list,
                                        struct Connection *c, int socket_fd) {
  struct sockaddr_storage client_address;
  unsigned int size = sizeof client_address;
  int client_fd = accept(socket_fd, (struct sockaddr *)&client_address, &size);
  if (client_fd == -1) {
    return -1;
  }

  {
    char ip[INET6_ADDRSTRLEN];
    if (get_socket_peer_ip(client_fd, ip, sizeof ip)) {
      return -1;
    }

    int port = get_socket_peer_port(client_fd);
    if (port == -1) {
      return -1;
    }

    printf("starting connection with %s:%d\n", ip, port);
  }

  c->socket_address = client_address;
  c->socket_fd = client_fd;

  srandom(time(NULL));
  ConnectionID id;
  while (1) {
    sprintf(id, "%.8lx", (unsigned long)random());
    if (find_connection_list_id(connections_list, id) == -1) {
      break;
    }
  }

  memcpy(c->id, id, sizeof c->id);

  return 0;
}

static int handle_message(struct vec *connections_list, int fd) {
  struct Connection *conn =
      vec_get(connections_list,
              (unsigned long)find_connection_list_fd(connections_list, fd));

  struct Packet packet;

  if (receive_packet(fd, &packet) == -1) {
    return -1;
  };

  switch (packet.type) {
  case SET_ID:;
    printf("received set id request\n");

    struct SetIDData *data = (struct SetIDData *)packet.data;

    if (find_connection_list_id(connections_list, data->new_id) != -1) {
      printf("%s already in use!\n", data->new_id);
      return -1;
    }

    ConnectionID old_id;
    memcpy(old_id, conn->id, sizeof old_id);

    printf("setting id of %s to %s\n", conn->id, data->new_id);
    memcpy(conn->id, data->new_id, sizeof conn->id);

    MessageContent content;
    sprintf(content, "set id from %s to %s", old_id, conn->id);
    send_message(fd, "server", conn->id, content);

    break;

  case MESSAGE:;
    fprintf(stderr, "MESSAGE forwarding not implemented");
    break;
  }

  return 0;
}

int server_loop(int socket_fd) {
  struct vec connections_list;
  vec_new(&connections_list, sizeof(struct Connection));

  struct vec pollfds;
  vec_new(&pollfds, sizeof(struct pollfd));
  struct pollfd listener;
  listener.fd = socket_fd;
  listener.events = POLLIN;
  listener.revents = 0;
  vec_push(&pollfds, &listener);

  while (1) {
    int num_events = poll(pollfds.buf, pollfds.len, -1);
    if (num_events == -1) {
      perror("poll");
      return 1;
    }

    if ((((struct pollfd *)pollfds.buf)[0].revents & POLLIN) != 0) {
      struct Connection c;
      accept_and_create_connection(&connections_list, &c, socket_fd);
      vec_push(&connections_list, &c);

      struct pollfd p;
      p.fd = c.socket_fd;
      p.events = POLLIN | POLLRDHUP;
      p.revents = 0;
      vec_push(&pollfds, &p);

      num_events -= 1;
    }

    struct vec indices_to_close;
    vec_new(&indices_to_close, sizeof(int[2]));
    for (size_t i = 0; i < pollfds.len && num_events > 0; i++) {
      struct pollfd p = ((struct pollfd *)pollfds.buf)[i];
      if ((p.revents & POLLIN) != 0) {
        handle_message(&connections_list, p.fd);
      }

      if ((p.revents & POLLRDHUP) != 0) {
        int conn_list_index = find_connection_list_fd(&connections_list, p.fd);
        int ele[2] = {i, conn_list_index};
        vec_push(&indices_to_close, ele);
      }

      if (p.revents != 0) {
        num_events -= 1;
      }
    }

    for (int i = 0; i < indices_to_close.len; i++) {
      int *indices = (int *)vec_get(&indices_to_close, i);
      int fd = ((struct pollfd *)vec_get(&pollfds, indices[0]))->fd;
      ConnectionID id;
      memcpy(id,
             ((struct Connection *)vec_get(&connections_list, indices[1]))->id,
             sizeof id);
      close(fd);
      vec_remove(&pollfds, indices[0]);
      vec_remove(&connections_list, indices[1]);
      printf("closed connection for %s\n", id);
    }

    vec_free(&indices_to_close);
  }

  close(socket_fd);

  return 0;
}

int main() {
  int socket_fd = bind_server_socket(ADDRESS, PORT);
  if (socket_fd == -1) {
    perror("bind_server_socket");
    return EXIT_FAILURE;
  }

  if (listen(socket_fd, BACKLOG) != 0) {
    perror("listen");
    close(socket_fd);
    return EXIT_FAILURE;
  }

  struct sigaction sa;
  sa.sa_handler = child_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL)) {
    perror("sigaction");
    return EXIT_FAILURE;
  }

  return server_loop(socket_fd);
}