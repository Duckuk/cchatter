#include "packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int receive_packet(int fd, struct Packet *packet) {
  struct Packet intermediate_packet;
  memset(&intermediate_packet, 0, sizeof intermediate_packet);
  unsigned long total_received = 0;
  while (1) {
    long received = recv(fd, &intermediate_packet + total_received,
                         sizeof intermediate_packet - total_received, 0);
    if (received == -1) {
      perror("recv");
      exit(1);
    }

    if (total_received == 0 && received == 0) {
      return -1;
    }

    total_received += (unsigned long)received;

    if (total_received == sizeof intermediate_packet) {
      break;
    }
  }

  memcpy(packet, &intermediate_packet, sizeof *packet);

  return 0;
}

int send_packet(int fd, struct Packet *packet) {
  unsigned long total_sent = 0;
  while (1) {
    long sent = send(fd, packet + total_sent, sizeof *packet - total_sent, 0);
    if (sent == -1) {
      perror("recv");
      exit(1);
    }

    total_sent += (unsigned long)sent;

    if (total_sent == 0 && sent == 0) {
      fprintf(stderr, "Error: send_packet couldn't send any data!");
      return -1;
    }

    if (total_sent > sizeof *packet) {
      fprintf(stderr, "Error: send_packet sent more data than necessary!");
      return -1;
    }

    if (total_sent == sizeof *packet) {
      break;
    }
  }

  return 0;
}

int send_message(int fd, ConnectionID from, ConnectionID to,
                 MessageContent content) {
  struct MessageData message;
  memcpy(message.from, from, sizeof(message.from));
  memcpy(message.to, to, sizeof(message.to));
  memcpy(message.content, content, sizeof(message.content));

  struct Packet packet;
  packet.type = MESSAGE;
  memcpy(packet.data, &message, sizeof packet.data);

  return send_packet(fd, &packet);
}
