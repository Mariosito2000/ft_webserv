#include "webserv.hpp"

std::string	getStatus(int status) {

	switch (status) {
		case 201:
			return "201 Created";
		case 400:
			return "400 Bad Request"; //El formato de la request está mal
		case 403:
			return "403 Forbidden"; //Acceso no permitido
		case 404:
			return "404 Not Found"; //Archivo no encontrado
		case 405:
			return "405 Method Not Allowed"; //Método que no gestionamos (DELETE de CGI o cualquier otro)
		case 408:
			return "408 Request Timeout"; //Timeout
		case 413:
			return "413 Payload Too Large"; //Request muy grande
		case 429:
			return "429 Too Many Request"; //Muchas
		case 500:
			return "500 Internal Server Error";
		case 501:
			return "501 Not Implemented";
		case 502:
			return "502 Bad Gateway";
		case 504:
			return "504 Gateway Timeout";
		default:
			return "200 OK";
	}
}

std::string getResponseHeader(HttpRequest &currentRequest, std::string &body) {

	std::string line = "HTTP/1.1 ";
	line.append(getStatus(currentRequest.status));
	line.append("\r\n");
	if (!body.empty()) {
		line.append("Content-Length: ");
		line.append(std::to_string((body).size()));
		line.append("\r\n");
	}
	line.append("Connection: close");
	line.append("\r\n\r\n");
	std::cout << std::endl << "RESPONSE HEADER IS: " << line << std::endl;
	return (line);
}

std::string getResponseBody(std::string fileToReturn) {

	std::ifstream file (fileToReturn, std::ios::binary);
	std::string fileLine;
    if (!file.is_open()) {
        std::cerr << "File error" << std::endl;
        exit (1); }
	std::cout << "Lee bien el fichero para enviar la respuesta" << std::endl;
	char c;
	while (file.get(c))
		fileLine.push_back(c);
	file.close();
	//std::cout << std::endl << "RESPONSE BODY IS: " << fileLine << std::endl;
	return (fileLine);
}

void	writeEvent(bTreeNode *server, struct client *client)
{
	std::cout << "---WRITE EVENT---" << std::endl;
	std::string finalRequest  = ResponseToMethod(server, client);
	
	size_t requestLength = strlen(finalRequest.c_str());
	size_t bytes_sent = 0;
	//std::cout << "RESPONSE IS: " << finalRequest << std::endl;
	while (bytes_sent < requestLength)
		bytes_sent += send(client->fd, finalRequest.c_str(), requestLength, MSG_DONTWAIT);
	client->state = -1;
}

/*void	writeEvent(bTreeNode *server, clientQueue &Queue, int ident, struct kevent *client_event, int kq)
{
	
	std::cout << "---WRITE EVENT---" << std::endl;
	std::string finalRequest  = ResponseToMethod(server, &(Queue.clientArray[Queue.getPos(ident)]));
	
	size_t requestLength = strlen(finalRequest.c_str());
	size_t bytes_sent = 0;
	//std::cout << "RESPONSE IS: " << finalRequest << std::endl;
	while (bytes_sent < requestLength)
		bytes_sent += send(ident, finalRequest.c_str(), requestLength, MSG_DONTWAIT);
	Queue.clearRequest(ident);
	EV_SET(&client_event[0], ident, EVFILT_WRITE, EV_DISABLE | EV_CLEAR, 0, 0, NULL);
	if (kevent(kq, &client_event[0], 1, NULL, 0, NULL) == -1)
		std::cerr << "kevent error\n";
}*/