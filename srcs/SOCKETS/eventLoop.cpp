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

std::string *getMultiMapValue(std::multimap<std::string, std::string> &map, std::string key)
{
	std::multimap<std::string, std::string>::iterator	it;
	it = map.find(key);
	if (it != map.end())
		return (&it->second);
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

int	pollEvents(std::vector<bTreeNode *> &servers, t_ports *ports)
{
	std::cout << "EN POLLEVENTS" << std::endl;
	pollfd	events[SOMAXCONN + 1];
	int		events_n = ports->n;
	std::vector<client>	clients;
	bzero(events, sizeof(pollfd) * (SOMAXCONN + 1));
	for (size_t i = 0; i < ports->n; i++)
	{
		events[i].fd = ports->fd[i];
		events[i].events = POLLIN;
		//events[i].revents = -1;
	}
	int	sign_events;
	int	accept_socket;
	//struct sockaddr_in	addr;
	struct sockaddr_in	client_addr;
	int					client_len;

	for (;;) {
		//std::cout << "----------LLAMAR A POLL PARA BUSCAR NUEVOS EVENTOS---------" << std::endl;
		sign_events = poll(events, events_n, 0);
        if (sign_events == -1) {
            perror("poll failed: ");
            exit(1);
        }
		if (sign_events > 0)
		{
			//std::cout << "Número de eventos a monitorear: " << events_n << std::endl;
			//std::cout << "Número de eventos señalizados: " << sign_events  << std::endl;
			int	events_it = events_n;
			for (int i = 0, j = 0; i < events_it && j < sign_events; i++)
			{
				//std::cout << "========EVENTO DETECTADO======" << std::endl;
				//std::cout << "EVENTO " << i << ": FD: " << events[i].fd << " | EVENTS: "
				//	<< events[i].events << " | REVENTS: " << events[i].revents << std::endl;
				if (events[i].revents & POLLIN) // socket de escucha, crear nuevo cliente
				{
					if (events[i].fd < (int)(ports->n + 3)) //ES SOCKET DE PUERTO, CREAR NUEVO CLIENTE
					{
						std::cerr << "Crea cliente nuevo" << std::endl;
						std::cout << "SOCKET DE ESCUCHA: " << events[i].fd << std::endl;
						accept_socket = accept(events[i].fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
						std::cout << "Socket de cliente aceptado: " << accept_socket << std::endl;
						if (accept_socket == -1) {
							std::cout << "ACCEPT ERROR" << std::endl;
							exit(0);
						}
						setNonBlocking(accept_socket);
						//CLIENTE
						client c;
						//bzero(&c, sizeof(client));
						c.fd = accept_socket;
						c.portID = findPortbySocket(ports, events[i].fd);
						std::cout << "PUERTO ASOCIADO AL CLIENTE: " << c.portID << std::endl;
						//c.serverID = events[i].fd - 3; //esto es mas bien portID
						c.state = 0;
						c.request.cgi = 0;
						c.request.bufLen = 0;
						c.response.bytesSent = 0;
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
								event->fd = accept_socket;
								event->events = poll_events[e];
								event->revents = 0;
								std::cout << "NEW EVENT | FD: " << event->fd << " | EVENTS: " << event->events << std::endl;
							}
							else
							{
								if (events_n < SOMAXCONN)
								{
									std::cout << "No hay evento a -1 para sobreescribir, escribo en el máximo actual" << std::endl;
									events[events_n].fd = accept_socket;
									events[events_n].events = poll_events[e];
									events[events_n].revents = 0;
									std::cout << "NEW EVENT | FD: " << events[events_n].fd << " | EVENTS: " << events[events_n]
										.events << std::endl;
									events_n++;
								}
							}
						}
						events[i].revents = 0;
						j++;
						continue ;
					}
					else // SOCKET DE CLIENTE, LEER REQUEST
					{
						client *curr_client = findClientFd(clients, events[i].fd);
						if (curr_client && curr_client->state < 2)
						{
							std::cout << "EVENTO DE LECTURA" << std::endl;
							if (readEvent(curr_client)) //meterlo todo en una funcion
								events_n = deleteClient(clients, *curr_client, events[i], events_n);
							std::cout << "Estado de cliente tras leer: " << curr_client->state << std::endl;
							if (curr_client->state == 2)
							{
								events[i].fd = -1;
								events[i].events = 0;
								curr_client->server = findServerByClient(servers, curr_client);
								if (!curr_client->server)
									events_n = deleteClient(clients, *curr_client, events[i], events_n);
								//events[i].revents = 0;
								else
									curr_client->response.response = ResponseToMethod(curr_client);
							}
						}
						events[i].revents = 0;
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
		//sleep(5);
	}
}	