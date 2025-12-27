#ifndef PACKET_H
#define PACKET_H

#define MAX_DATA_SIZE sizeof(struct MessageData)

typedef char ConnectionID[32];
typedef char MessageContent[1024];

enum PacketType { SET_ID, MESSAGE };

struct SetIDData {
  ConnectionID new_id;
};

struct MessageData {
  ConnectionID from;
  ConnectionID to;
  MessageContent content;
};

struct Packet {
  enum PacketType type;
  char data[MAX_DATA_SIZE];
};

// Receives a packet over the socket `fd` and puts it in `packet`.
int receive_packet(int fd, struct Packet *packet);
// Sends `packet` over the socket `fd`.
int send_packet(int fd, struct Packet *packet);

int send_message(int fd, ConnectionID from, ConnectionID to,
                 MessageContent content);

#endif
