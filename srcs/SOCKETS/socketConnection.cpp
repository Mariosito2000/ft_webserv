#include "../../includes/webserv.hpp"

void setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1)
  {
    perror("fcntl()"); //throw
    return;
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    perror("fcntl()"); //throw
}

int	getServerSocket(sockaddr_in *addr, int port) {

	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
  	if (sock == -1)
	{
    	perror("socket()");
    	exit (1);
 	}
	//Setear el socket a non-blocking
	int enable = 1;
  	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
	{
    	perror("setsockopt()");
    	exit (1);
  	}
	//Montar la address y bindear al socket
  	memset(addr, 0, sizeof(*addr));
  	addr->sin_family = PF_INET;
 	addr->sin_addr.s_addr = htonl(INADDR_ANY);
  	addr->sin_port = htons(port);
	setNonBlocking(sock);
	return (sock);
}

void	bindAndListen(int sock, sockaddr_in *addr) {

	if (bind(sock, (struct sockaddr *)addr, sizeof(*addr)) < 0)
	{
   		perror("bind()");
    	exit (1);
  	}
	//Socket en modo escucha
  	if (listen(sock, SOMAXCONN) < 0)
	{
    	perror("listen()");
    	exit (1);
  	}
}