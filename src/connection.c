#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int BACKLOG_LENGTH = 1; // Not sure if I need more.

int usairb_connect(int port) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (errno) {
    fprintf(stderr, "socket produced errno %i\n", errno);
    exit(EXIT_FAILURE);
  }
  if (socket_fd < 0) {
    fprintf(stderr, "socket returned error code %i\n", socket_fd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in socket_address_in = {
      .sin_family = AF_INET,
      .sin_port = htons(port),
      .sin_addr = {.s_addr = INADDR_ANY},
  };
  int socket_len = sizeof(socket_address_in);
  struct sockaddr *socket_address = (struct sockaddr *)&socket_address_in;

  int bind_result = bind(socket_fd, socket_address, socket_len);
  if (errno) {
    fprintf(stderr, "bind produced errno %i\n", errno);
    exit(EXIT_FAILURE);
  }
  if (bind_result != 0) {
    fprintf(stderr, "bind returned error code %i\n", bind_result);
    exit(EXIT_FAILURE);
  }

  int listen_result = listen(socket_fd, BACKLOG_LENGTH);
  if (errno) {
    fprintf(stderr, "listen produced errno %i\n", errno);
    exit(EXIT_FAILURE);
  }
  if (listen_result != 0) {
    fprintf(stderr, "listen returned error code %i\n", listen_result);
    exit(EXIT_FAILURE);
  }

  return socket_fd;
}

int usairb_accept(int socket_fd) {
  // Make the call to accept non-blocking, since we'll rely
  // on our udev monitor busy loop for now.
  int socket_fd_flags = fcntl(socket_fd, F_GETFL);
  fcntl(socket_fd, F_SETFL, socket_fd_flags | O_NONBLOCK);

  int client_socket_fd = accept(socket_fd, NULL, NULL);
  if (errno) {
    fprintf(stderr, "accept produced errno %i\n", errno);
    exit(EXIT_FAILURE);
  }
  if (client_socket_fd < 0) {
    fprintf(stderr, "accept returned error code %i\n", client_socket_fd);
    exit(EXIT_FAILURE);
  }
  return client_socket_fd;
}
