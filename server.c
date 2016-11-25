#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include<arpa/inet.h>

#define BUFLEN 512  // Max length of buffer
#define PORT 1029   // The port on which to listen for incoming data

struct Registry {
  char name[256];
  char value[256];
  char type[256];
  char ttl[256];
};

long read_dns_file(FILE *file, struct Registry *registries[]) {
  long count = 0;
  char line[1024];
  while (fgets(line, sizeof line, file) != NULL) {
    struct Registry *registry = malloc(sizeof(struct Registry));
    sscanf(line, "%s %s %s %s", registry->name, registry->value, registry->type, registry->ttl);
    registries[count++] = registry;
  }
  return count;
}

long line_count(FILE *file) {
  long c;
  long lines = 0;
  while ((c = fgetc(file)) != EOF) {
    if (c == '\n' || c == '\r') ++lines;
  }
  fseek(file, 0, SEEK_SET); // seek back to beginning of file
  return lines;
}

int main(int argc, char const *argv[]) {

  /* DNS file setup */

  FILE *file = fopen("./server.dns", "r");

  long size = line_count(file);
  struct Registry **registries = calloc(size, sizeof(struct Registry));
  size = read_dns_file(file, registries);

  fclose(file);


  /* UDP Server setup */

  struct sockaddr_in si_me, si_other;

  socklen_t slen;
  int recv_len;
  char buf[BUFLEN];

  // Create UDP socket
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) {
    perror("Cannot create socket");
  }

  // Zero out the structure
  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind socket to port
  if(bind(sock, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
    perror("Cannot bind socket");
  }

  while(1) {
    printf("Waiting for data...");
    fflush(stdout);

    // Try to receive some data, this is a blocking call
    if ((recv_len = recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == -1) {
      perror("recvfrom()");
    }

    // Print details of the client/peer and the data received
    printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("Data: %s\n" , buf);

    // Now reply the client with the same data
    if (sendto(sock, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1) {
      perror("sendto()");
    }

    // Clear the buffer by filling null
    memset(buf, '\0' , BUFLEN);
  }
  close(sock);

  return 0;
}
