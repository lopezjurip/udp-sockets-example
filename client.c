#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512  // Max length of buffer
#define PORT 1029   // The port on which to send data

int main(int argc, char const *argv[]) {
  struct sockaddr_in si_other;
  socklen_t slen = sizeof(si_other);
  char buf[BUFLEN];
  char message[BUFLEN];

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) {
    perror("Cannot create socket");
  }

  // Zero out the structure
  memset((char *) &si_other, 0, sizeof(si_other));

  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);

  if (inet_aton(SERVER, &si_other.sin_addr) == 0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  while(1) {
    printf("Enter message: ");
    fgets(message, BUFLEN, stdin);

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

    // Show response
    puts(buf);
  }
  close(sock);

  return 0;
}
