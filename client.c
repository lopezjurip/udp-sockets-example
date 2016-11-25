#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER "127.0.0.1"
#define BUFLEN 1024  // Max length of buffer
#define PORT 1029    // The port on which to send data

struct Registry {
  char name[256];
  char value[256];
  char type[256];
  char ttl[256];
};

char *trim(char *str) {
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0) return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

int valid_ip(char *ip) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return result != 0;
}

int main(int argc, char const *argv[]) {
  // Get server address
  const char *server = (argc < 2) ? SERVER : argv[1];
  printf("Server: %s\n\n", server);

  // Prepare sockets
  struct sockaddr_in si_other;
  socklen_t slen = sizeof(si_other);
  char buf[BUFLEN];
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

    char *message = trim(buffer);
    // int reverse_query = valid_ip(message);
    // int16_t id = identifier++;
    // int16_t flags = 0;
    // flags &= ~0x01; // 0 = query
    // flags &= (0b0100 << 4);

    // Send the message
    if (sendto(sock, message, strlen(message), 0, (struct sockaddr *) &si_other, slen)==-1) {
      perror("sendto()");
    }

    // Clear the buffer by filling null, it might have previously received data
    memset(buf, '\0' , BUFLEN);

    // Try to receive some data, this is a blocking call
    if (recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1) {
      perror("recvfrom()");
    }

    // Parse response
    struct Registry *registry = malloc(sizeof(struct Registry));
    sscanf(buf, "%s %s %s %s", registry->name, registry->value, registry->type, registry->ttl);

    // Show output or do action
    if (strcmp(registry->type, "A") == 0) {
      printf("%s has address %s\n\n", registry->name, registry->value);
    } else if (strcmp(registry->type, "CNAME") == 0) {
      printf("%s is an alias for %s\n\n", registry->name, registry->value);
      // TODO: requery
    } else {
      printf("Host %s not found\n\n", registry->name);
    }
  }

  // Shutdown socket
  close(sock);

  return 0;
}
