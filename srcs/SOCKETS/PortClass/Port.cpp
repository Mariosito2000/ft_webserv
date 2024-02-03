#include "Port.hpp"

Port::Port(int id, int fd)
{
	_fd = fd;
	_id = id;
	_clients = 0;
}

int		Port::getFd() { return (_fd); }
int		Port::getId() { return (_id); }
size_t	Port::getClients() { return (_clients); }
void	Port::setFd(int fd) { _fd = fd; }
void	Port::setId(int id) { _id = id; }
void	Port::addClient() { _clients++; };
void	Port::eraseClient() { _clients--; };