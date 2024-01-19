#include "../includes/webserv.hpp"

/*int	kqueue_events(std::vector<bTreeNode *>servers, std::vector<int> &sockets)
{
	struct kevent event[SOMAXCONN + 1];

	int kq = kqueue();
	if (kq < 0) {
    	perror("kqueue()");
    	exit (1);
  	}
	
	//Declarar la clientQueue;
	clientQueue Queue;
	std::vector<client>	clients;
	//Seter el evento para la kqueue
	struct kevent server_event;
	for (int i = 0; i < servers.size(); i++) {
		EV_SET(&server_event, sockets[i], EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
		//Registrar el evento
		if (kevent(kq, &server_event, 1, NULL, 0, NULL) == -1) {
    	    perror("kevent");
    	    exit(1);
   		}
	}
	//Montar el cliente en el vector
	//Chequear para evento una vez cada ciclo
	int new_events;
	int	new_sock;
	struct sockaddr_in	addr;
	struct sockaddr_in	client_addr;
	int					client_len;
	for (;;) {
		std::cout << "----------LLAMAR A KEVENT PARA BUSCAR NUEVOS EVENTOS---------" << std::endl;
		new_events = kevent(kq, NULL, 0, event, SOMAXCONN + 1, NULL);
		std::cout << "Número de eventos señalizados: " << new_events << std::endl;
        if (new_events == -1) {
            perror("kevent");
            exit(1);
        }
		for (int i = 0; i < new_events; i++) {
			struct kevent client_event[2];
			std::cout << "========EVENTO DETECTADO======" << std::endl;
			if (event[i].udata != NULL)
			{
				std::cout << "EVENTO ES SOCKET DE CLIENTE" << std::endl;
				client *clientPtr = (client *)event[i].udata;
				std::cout << "CLIENT FD: " << clientPtr->fd << " | SERVER ID: " << clientPtr->serverID
						<< " | STATE: " << clientPtr->state << std::endl;
			}
			//Socket de escucha
			bool isServerSocket = false;
			if (event[i].ident <= sockets.size() + 3)
			{
				std::cout << "EVENTO ES SOCKET DE ESCUCHA: " << event[i].ident << std::endl;
				isServerSocket = true;
			}
			if (isServerSocket == true && event[i].filter == EVFILT_READ) {

				std::cout << "---ACCEPT EVENT---" << std::endl;
                //Montar cliente
                new_sock = accept(event[i].ident, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
				if (new_sock == -1) {
					std::cout << "ACCEPT ERROR" << std::endl;
					exit(0);
				}
				setNonBlocking(new_sock);
				client c = {new_sock, event[i].ident - 3, 0};
				clients.push_back(c);
				//Queue.addClient(new_sock, event[i].ident);
				//std::cout << "ADDED CLIENT. FD: " << event[i].ident << "SERVER ID: " << Queue.getServerId(new_sock) << std::endl;
				std::cout << "ADDED CLIENT. FD: " << clients.back().fd << " | SERVER ID: " << clients.back().serverID << std::endl;
                // Nuevo evento al kqueue
				EV_SET(&client_event[0], new_sock, EVFILT_READ, EV_ADD | EV_CLEAR | EV_EOF, 0, 0, &clients.back());
				EV_SET(&client_event[1], new_sock, EVFILT_WRITE, EV_ADD | EV_DISABLE | EV_CLEAR, 0, 0, &clients.back());
				if (kevent(kq, client_event, 2, NULL, 0, NULL) < 0)
                    perror("kevent error");
				continue ;
			}
			client *clientPtr = (client *)event[i].udata;
			if (isServerSocket == false && event[i].filter == EVFILT_WRITE && clientPtr->state == 2) // && clientPtr->state == 2
			{
				std::cout << "EVENTO DE ESCRITURA" << std::endl;
				//writeEvent(servers[event[i].ident - 3], &event[i]);
				//writeEvent(servers[Queue.getServerId(event[i].ident)], Queue, event[i].ident, client_event, kq);
				close(event[i].ident); //cerrar socket de conexion - se terminó
				//std::vector<client>::iterator	it = std::find(clients.begin(), clients.end(), *clientPtr);
				//clients.erase(it);
			}
			else if (isServerSocket == false && event[i].filter == EVFILT_READ && clientPtr->state < 2) // && clientPtr->state < 2
			{
				std::cout << "EVENTO DE LECTURA" << std::endl;
				//readEvent(&event[i]);
				//readEvent(Queue, &event[i], client_event, kq); //event es el evento que ha ocurrido; client_event es el tipo de evento a monitorear
				std::cout << "Estado de cliente tras leer: " << clientPtr->state << std::endl;
				if (clientPtr->state == 2)
				{
					EV_SET(&client_event[1], event[i].ident, EVFILT_WRITE, EV_ENABLE | EV_CLEAR, 0, 0, NULL);
					if (kevent(kq, &client_event[1], 1, NULL, 0, NULL) == -1)
						std::cerr << "kevent error" << std::endl;
				}
			}
			
		}
	}
	return (0);
}*/

int	main(int argc, char **argv) {

	if (argc != 2) {
        std::cerr << "Format: ./webserv [configuration file]" << std::endl;
        return (1); }
    if (access(argv[1], R_OK) != 0) {
        std::cerr << "Inaccessible file" << std::endl;
        return 1; }
	
	
	struct sockaddr_in	addr;
	//struct sockaddr_in	client_addr;
	//int					client_len;

	//guardar arbol con config
	bTreeNode	*root = parseFile(argv[1]);
	if (!root)
		return (2);
	bTreeNode	*http = NULL;
	findNode(root, &http, "http");
	if (!http)
		return (2);
	std::vector<bTreeNode*>	servers;
	for (size_t i = 0; i < http->childs.size(); i++)
	{
		if (http->childs[i]->contextName == "server")
			servers.push_back(http->childs[i]);
	}
	//std::vector<int>	ports;
	//Socket del servidor
	t_ports	ports;
	int	id;
	ports.n = 0;
	for (size_t i = 0; i < servers.size(); i++)
	{
		
		std::string	*value = getMultiMapValue(servers[i]->directivesMap, "listen");
		if (!value)
			return (3);
		//if (!getValue(servers[i]->directives, "listen"))
		//	return (3);
		id = atoi(value->c_str());
		ports.id.push_back(id);
		std::cout << "Port de server " << i << " es: " << id << std::endl;
		ports.fd.push_back(getServerSocket(&addr, id));
		std::cout << "Socket de servidor " << i << " es: " << ports.fd.back() << std::endl;
		bindAndListen(ports.fd[i], &addr);
		ports.n++;
	}
	pollEvents(servers, &ports);	
	return (0);
}

