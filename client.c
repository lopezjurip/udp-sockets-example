#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "common.h"

#define SERVER "127.0.0.1"
#define BUFLEN 1024  // Max length of buffer
#define PORT 1029    // The port on which to send data

struct Registry *request_dns(int sock, struct sockaddr_in si_other, char *message) {
  socklen_t slen = sizeof(si_other);

  // Clear the buffer by filling null, it might have previously received data
  char buf[BUFLEN];
  memset(buf, '\0' , BUFLEN);

  // Send the message
  if (sendto(sock, message, strlen(message), 0, (struct sockaddr *) &si_other, slen)==-1) {
    perror("sendto()");
  }

  // Try to receive some data, this is a blocking call
  if (recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1) {
    perror("recvfrom()");
  }

  // Parse response
  struct Registry *registry = malloc(sizeof(struct Registry));
  sscanf(buf, "%s %s %s %s", registry->name, registry->value, registry->type, registry->ttl);
  return registry;
}

int main(int argc, char const *argv[]) {
  printf("Please see README.md, you may have problems the first try.\n");

  // Get server address
  const char *server = (argc < 2) ? SERVER : argv[1];
  printf("Server: %s\n\n", server);

  // Prepare sockets
  struct sockaddr_in si_other;
  char buffer[BUFLEN];

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) {
    perror("Cannot create socket");
  }

  // Zero out the structure
  memset((char *) &si_other, 0, sizeof(si_other));

  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);

  if (inet_aton(server, &si_other.sin_addr) == 0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  // int16_t identifier = 0;

  while(1) {
    printf("Enter query: ");
    fgets(buffer, BUFLEN, stdin);

    char *query = trim(buffer);
    // int reverse_query = valid_ip(query);
    // int16_t id = identifier++;
    // int16_t flags = 0;
    // flags &= ~0x01; // 0 = query
    // flags &= (0b0100 << 4);

    struct Registry *registry;

    do {
      // Request DNS
      registry = request_dns(sock, si_other, query);

      // It is an A record
      if (strcmp(registry->type, "A") == 0) {
        printf("%s has address %s\n\n", registry->name, registry->value);

      // It is a CNAME record, try requesting again
      } else if (strcmp(registry->type, "CNAME") == 0) {
        printf("%s is an alias for %s\n\n", registry->name, registry->value);
        query = registry->value; // update query value

      // ¿?
      } else {
        printf("Host %s not found\n\n", registry->name);
      }

    // Do till no more CNAMES
    } while(strcmp(registry->type, "CNAME") == 0);
  }

  // Shutdown socket
  close(sock);

  return 0;
}
