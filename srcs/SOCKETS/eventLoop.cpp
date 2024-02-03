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

parseTree *findServerByClient(std::vector<parseTree *> servers, struct client *client)
{
	typedef std::multimap<std::string, std::string>::iterator	it;

	//Busca por hostname
	it ith = client->request.headers.find("Hostname");
	if (ith != client->request.headers.end())
	{
		for (size_t i = 0; i < servers.size(); i++)
		{
			it its = servers[i]->context._dirs.find("server_name");
			it itp = servers[i]->context._dirs.find("listen");
			if (its != servers[i]->context._dirs.end()
				&& itp != servers[i]->context._dirs.end())
			{
				if (!strncmp(ith->second.c_str(), its->second.c_str(), its->second.length())
						&& atoi(itp->second.c_str()) == client->portID)
					return (servers[i]);
			}
		}
	}
	//Busca por puerto
	for (size_t i = 0; i < servers.size(); i++)
	{
		ith = servers[i]->context._dirs.find("listen");
		if (ith != servers[i]->context._dirs.end())
		{
			int	port = atoi(ith->second.c_str());
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

void	setEvent(pollfd *event, int _fd, short _event, short _revent)
{
	event->fd = _fd;
	event->events = _event;
	event->revents = _revent;
}

void	deleteClient(struct clients &clients, struct events &events, client &c) //int i, int events_n
{
	std::cout << "DELETE CLIENT: " << c.fd << std::endl;
	//if (close(c.fd) == -1)
	//	std::cerr << "CLOSE FAIL\n";
	//setEvent(c.events[0], -1, 0, 0);
	//setEvent(c.events[1], -1, 0, 0);
	events.delEvents.push_back(c.event);
	//std::vector<client>::iterator	it = std::find(clients.clients.begin(), clients.clients.end(), c);
	clients.delClients.push_back(c);
	//clients.clients.erase(it);
}

size_t	getTimeSeconds()
{
	time_t	seconds;

	seconds = time(NULL);
	return (seconds);
}

void	setClient(struct client &client, int fd, int id, std::vector<parseTree*> servers)
{
	client.fd = fd;
	client.portID = id;
	client.state = 0;
	client.request.cgi = 0;
	client.request.bufLen = 0;
	client.response.bytesSent = 0;
	client.server = findServerByClient(servers, &client);
	client.loc = NULL;
	client.timer = getTimeSeconds();
}

int	createClient(struct clients &clients, struct events &events, std::vector<parseTree *> servers, int socket,
					t_ports *ports)
{
	struct sockaddr_in	client_addr;
	int					client_len;

	int accept_socket = accept(socket, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
	if (accept_socket == -1)
	{
		std::cerr << "ACCEPT ERROR" << std::endl;
		return (0);
	}
	setNonBlocking(accept_socket);

	client c;
	setClient(c, accept_socket, findPortbySocket(ports, socket), servers);
	pollfd event;
	setEvent(&event, c.fd, POLLIN | POLLOUT, 0);
	c.event = event;
	events.addEvents.push_back(event);
	std::cout << "NEW CLIENT | FD: " << event.fd << std::endl;
	clients.addClients.push_back(c);
	return (1);
}

int	readClient(struct clients &clients, struct events &events, std::vector<parseTree *> servers, pollfd &event)
{
	client *curr_client = findClientFd(clients.clients, event.fd);
	try
	{
		if (curr_client && curr_client->state < 2)
		{
			if (readEvent(curr_client))
				deleteClient(clients, events, *curr_client);
			else 
			{
				if (curr_client->state == 2)
				{
					//setEvent(&event, -1, 0, 0);
					curr_client->server = findServerByClient(servers, curr_client);
					if (!curr_client->server)
						deleteClient(clients, events, *curr_client);
					else
						ResponseToMethod(curr_client);
				}
			}
		}
	}
	catch(enum statusCodes error)
	{
		curr_client->request.status = error;
		getErrorResponse(curr_client, error);
		curr_client->state = 3;
	}
	return (1);
}

int	checkTimerExpired(clients &clients, struct events &events)
{
	for (size_t i = 0; i < clients.clients.size(); i++)
	{
		std::string *timeoutStr = getMultiMapValue(clients.clients[i].server->context._dirs, "timeout");
		if (timeoutStr)
		{
			size_t	timeoutInt = atoi(timeoutStr->c_str());
			size_t	time = getTimeSeconds();
			if ((time - clients.clients[i].timer) >= timeoutInt && clients.clients[i].state < 2)
			{
				clients.clients[i].request.status = REQUEST_TIMEOUT;
				getErrorResponse(&clients.clients[i], REQUEST_TIMEOUT);
				deleteClient(clients, events, clients.clients[i]);
				//setEvent(clients.clients[i].events[0], -1, 0, 0);
				clients.clients[i].state = 3;
			}
		}
	}
	return (0);
}

void	resetEvents(struct events &events)
{
	if (events.addEvents.empty() && events.delEvents.empty())
		return ;
	//std::cout << "Resetear array de eventos" << std::endl;
	size_t	new_events_n = events.n + events.addEvents.size() - events.delEvents.size();
	//std::cout << "Número nuevo de eventos: " << new_events_n << std::endl;
	pollfd	*new_events = (pollfd *)malloc(sizeof(pollfd) * new_events_n);
	size_t	i = 0;
	size_t	j = 0;
	/*if (events.delEvents.empty())
		std::cout << "No hay eventos que borrar" << std::endl;
	else
	{
		std::cout << "Hay eventos que borrar" << std::endl;
		for (std::list<pollfd>::iterator itb = events.delEvents.begin(), ite = events.delEvents.end(); itb != ite; itb++)
			std::cout << "EVENTO: FD: " << itb->fd << std::endl;
	}
	std::cout << "COPIAR EVENTOS YA EXISTENTES" << std::endl;*/
	for (; i < events.n; i++)
	{
		if (!events.delEvents.empty() && events.delEvents.front().fd == events.events[i].fd)
		{
			//std::cout << "Borrar de array de eventos: FD: " << events.events[i].fd << std::endl;
			events.delEvents.pop_front();
		}
		else
		{
			//std::cout << "Copiar eventos ya existentes: " << events.events[i].fd << std::endl;
			new_events[j] = events.events[i];
			j++;
		}
	}
	events.delEvents.clear();
	/*if (events.addEvents.empty())
		std::cout << "No hay eventos nuevos que añadir" << std::endl;
	else
	{
		std::cout << "Hay eventos nuevos que añadir" << std::endl;
		for (std::list<pollfd>::iterator itb = events.addEvents.begin(), ite = events.addEvents.end(); itb != ite; itb++)
			std::cout << "EVENTO: FD: " << itb->fd << std::endl;
	}*/
	free(events.events);
	events.events = new_events;
	events.n = new_events_n;
	for (; j < events.n && !events.addEvents.empty(); events.addEvents.pop_front(), j++)
	{
		//std::cout << "Añadir nuevo evento al array: FD: " << events.addEvents.front().fd << std::endl;
		events.events[j] = events.addEvents.front();
		//std::cout << "Añadido: " << events.events[j].fd << std::endl;
	}
	events.addEvents.clear();
	std::cout << "EVENTOS A MONITOREAR: " << events.n << std::endl;
	std::cout << "NUEVO ARRAY DE EVENTOS: " << std::endl;
	for (size_t i = 0; i < events.n; i++)
		std::cout << "EVENT: FD: " << events.events[i].fd << std::endl;
}

void	resetClients(struct clients &clients)
{
	if (clients.addClients.empty() && clients.delClients.empty())
		return ;
	//std::cout << "Resetear vector de clientes" << std::endl;
	for (; !clients.delClients.empty(); clients.delClients.pop_front())
	{	
		std::vector<client>::iterator it = std::find(clients.clients.begin(), clients.clients.end(), clients.delClients.front());
		if (close(it->fd) == -1)
			std::cerr << "CLOSE FAIL\n";
		clients.clients.erase(it);
	}
	clients.delClients.clear();
	for (; !clients.addClients.empty(); clients.addClients.pop_front())
		clients.clients.push_back(clients.addClients.front());
	clients.addClients.clear();
	std::cout << "NUEVO VECTOR DE CLIENTES: " << std::endl;
	for (size_t i = 0; i < clients.clients.size(); i++)
		std::cout << "CLIENT: FD: " << clients.clients[i].fd << std::endl;
}

int	pollEvents(std::vector<parseTree *> &servers, t_ports *ports)
{
	//EVENTS
	struct events events;
	events.n = ports->n;
	events.events = (pollfd *)malloc(sizeof(pollfd) * events.n);
	bzero(events.events, sizeof(pollfd) * events.n);

	struct clients clients;
	
	for (size_t i = 0; i < ports->n; i++)
		setEvent(&events.events[i], ports->fd[i], POLLIN, 0);
	int	sign_events;
	for (;;)
	{
		sign_events = poll(events.events, events.n, 0);
        if (sign_events == -1)
			perror("POLL FAILED: ");
		//std::cout << "SIGNALED EVENTS: " << sign_events << std::endl;
		if (sign_events > 0)
		{
			//int	events_it = events_n;
			for (size_t i = 0; i < events.n; i++)
			{
				if (events.events[i].revents & POLLIN) // READ EVENT
				{
					if (events.events[i].fd < (int)(ports->n + 3)) // PORT SOCKET, CREATE NEW CLIENT
						createClient(clients, events, servers, events.events[i].fd, ports);
					else // CLIENT SOCKET, READ REQUEST
						readClient(clients, events, servers, events.events[i]);
					//j++;
				}
				else if (events.events[i].revents & POLLOUT) // CLIENT SOCKET, WRITE RESPONSE
				{
					client *curr_client = findClientFd(clients.clients, events.events[i].fd);
					if (curr_client && curr_client->state == 3)
					{
						if (writeEvent(curr_client) <= 0)
							deleteClient(clients, events, *curr_client);		
					}
					//j++;
				}		
			}
		}
		//checkTimerExpired(clients, events);
		resetClients(clients);
		resetEvents(events);
	}
	return (1);
}	