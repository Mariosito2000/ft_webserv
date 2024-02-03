#pragma once
#ifndef PORT_HPP
#define PORT_HPP

#include "../../../includes/webserv.hpp"

class Port
{
private:

	int		_fd;
	int		_id;
	size_t	_clients;

public:

	Port();
	Port(int id, int fd);
	~Port();

	int		getFd();
	int		getId();
	size_t	getClients();

	void	setFd(int);
	void	setId(int);
	void	addClient();
	void	eraseClient();

};

#endif