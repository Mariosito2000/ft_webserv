#include "../../includes/webserv.hpp"

client *findClientFd(std::vector<client> &clients, int fd)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].fd == fd)
			return (&clients[i]);
	}
	return (NULL);
}

int	findPortbySocket(t_ports *ports, int socket)
{
	for (size_t i = 0; i < ports->n; i++)
	{
		if (ports->fd[i] == socket)
			return (ports->id[i]);
	}
	return (-1);
}

bTreeNode *findServerByClient(std::vector<bTreeNode *> servers, struct client *client)
{
	std::multimap<std::string, std::string>::iterator	it;
	std::cout << "BUSCAR POR HOSTNAME" << std::endl;
	it = client->request.headers.find("hostname");
	if (it != client->request.headers.end())
	{
		std::string &serverName = it->second;
		//busca por servername
		std::cout << "SERVER_NAME: " << serverName << std::endl;
		for (size_t i = 0; i < servers.size(); i++)
		{
			it = servers[i]->directivesMap.find("server_name");
			if (it != servers[i]->directivesMap.end())
			{
				if (it->second == serverName)
					return (servers[i]);
			}
		}
	}
	//busca por puerto
	std::cout << "BUSCAR POR PUERTO" << std::endl;
	for (size_t i = 0; i < servers.size(); i++)
	{
		it = servers[i]->directivesMap.find("listen");
		if (it != servers[i]->directivesMap.end())
		{
			int	port = atoi(it->second.c_str());
			if (port == client->portID)
				return (servers[i]);
		}
	}
	return (NULL);
}

pollfd *findUnusedPoll(pollfd *polls, int polls_n)
{
	for (int i = 0; i < polls_n; i++)
	{
		if (polls[i].fd == -1)
			return (&polls[i]);
	}
	return (NULL);
}

size_t	deleteClient(std::vector<client> &clients, client &c, pollfd &event, size_t n)
{
	close(c.fd);
	event.fd = -1;
	event.events = 0;
	event.revents = 0;
	std::vector<client>::iterator	it = std::find(clients.begin(), clients.end(), c);
	clients.erase(it);
	std::cerr << "Elimina cliente" << std::endl;
	return (n - 1);
}

void	setClient(struct client &client, int fd, int id)
{
	client.fd = fd;
	client.portID = id;
	client.state = 0;
	client.request.cgi = 0;
	client.request.bufLen = 0;
	client.response.bytesSent = 0;
}

void	setEvent(pollfd *event, int _fd, short _event, short _revent)
{
	event->fd = _fd;
	event->events = _event;
	event->revents = _revent;
}

int	createClient(std::vector<client> &clients, int socket, t_ports *ports, pollfd *events, int &events_n)
{
	struct sockaddr_in	client_addr;
	int					client_len;

	std::cerr << "Crea cliente nuevo" << std::endl;
	std::cout << "SOCKET DE ESCUCHA: " << socket << std::endl;
	int accept_socket = accept(socket, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
	std::cout << "Socket de cliente aceptado: " << accept_socket << std::endl;
	if (accept_socket == -1)
	{
		std::cout << "ACCEPT ERROR" << std::endl;
		return (0); //throw
	}
	setNonBlocking(accept_socket);
	//CLIENTE
	client c;
	setClient(c, accept_socket, findPortbySocket(ports, socket));
	std::cout << "Añadir cliente al vector" << std::endl;
	clients.push_back(c);
	std::cout << "Añadió bien el cliente al vector" << std::endl;
	short	poll_events[2] = {POLLIN, POLLOUT};
	for (int e = 0; e < 2; e++)
	{
		pollfd *event = findUnusedPoll(events, events_n);
		if (event)
		{
			std::cout << "Hay evento a -1 para sobreescribir";
			setEvent(event, accept_socket, poll_events[e], 0);
			std::cout << "NEW EVENT | FD: " << event->fd << " | EVENTS: " << event->events << std::endl;
		}
		else
		{
			if (events_n < SOMAXCONN)
			{
				std::cout << "No hay evento a -1 para sobreescribir, escribo en el máximo actual" << std::endl;
				setEvent(&events[events_n], accept_socket, poll_events[e], 0);
				std::cout << "NEW EVENT | FD: " << events[events_n - 1].fd << " | EVENTS: " << events[events_n]
					.events << std::endl;
				events_n++;
			}
		}
	}
	return (1);
}

int	readClient(std::vector<client> &clients, std::vector<bTreeNode *> servers, pollfd &event, int &events_n)
{
	//TRY
	client *curr_client = findClientFd(clients, event.fd);
	if (curr_client && curr_client->state < 2)
	{
		std::cout << "EVENTO DE LECTURA" << std::endl;
		if (readEvent(curr_client))
			events_n = deleteClient(clients, *curr_client, event, events_n);
		else
		{
			std::cout << "Estado de cliente tras leer: " << curr_client->state << std::endl;
			if (curr_client->state == 2)
			{
				setEvent(&event, -1, 0, 0);
				curr_client->server = findServerByClient(servers, curr_client);
				if (!curr_client->server)
					events_n = deleteClient(clients, *curr_client, event, events_n);
				else
					curr_client->response.response = ResponseToMethod(curr_client);
			}
		}
	}
	//CATCH	CODIGO DE ERROR
	/*
		ESCRIBIR HEADER + HTML DE ERROR
	*/
	return (1);
}

int	pollEvents(std::vector<bTreeNode *> &servers, t_ports *ports)
{
	std::cout << "EN POLLEVENTS" << std::endl;
	pollfd	events[SOMAXCONN + 1];
	int		events_n = ports->n;
	std::vector<client>	clients;
	bzero(events, sizeof(pollfd) * (SOMAXCONN + 1));
	for (size_t i = 0; i < ports->n; i++)
		setEvent(&events[i], ports->fd[i], POLLIN, -1);

	int	sign_events;

	for (;;)
	{
		sign_events = poll(events, events_n, 0);
        if (sign_events == -1) {
            perror("poll failed: ");
            exit(1);
        }
		if (sign_events > 0)
		{
			int	events_it = events_n;
			for (int i = 0, j = 0; i < events_it && j < sign_events; i++)
			{
				if (events[i].revents & POLLIN) // EVENTO DE LECTURA
				{
					if (events[i].fd < (int)(ports->n + 3)) //ES SOCKET DE PUERTO, CREAR NUEVO CLIENTE
					{
						createClient(clients, events[i].fd, ports, events, events_n);
						j++;
						continue ;
					}
					else // SOCKET DE CLIENTE, LEER REQUEST
					{
						readClient(clients, servers, events[i], events_n);
						j++;	
					}
				}
				if (events[i].revents & POLLOUT) // SOCKET DE CLIENTE, ESCRIBIR REQUEST
				{
					client *curr_client = findClientFd(clients, events[i].fd);
					if (curr_client && curr_client->state == 3)
					{
						std::cout << "EVENTO DE ESCRITURA" << std::endl;
						if (!writeEvent(curr_client))
							events_n = deleteClient(clients, *curr_client, events[i], events_n);
					}
					j++;
				}		
			}
		}
	}
	return (1);
}	