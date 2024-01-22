#include "../../includes/webserv.hpp"

std::string getResponseHeader(HttpRequest &currentRequest, std::string &body) {

	std::string line = "HTTP/1.1 ";
	line.append(getStatus(currentRequest.status));
	line.append("\r\n");
	if (!body.empty()) {
		line.append("Content-Length: ");
		line.append(std::to_string((body).size()));
		//line.append("\r\n");
	}
	//line.append("Connection: close");
	line.append("\r\n\r\n");
	std::cout << std::endl << "RESPONSE HEADER IS: " << line << std::endl;
	return (line);
}

std::string getResponseBody(std::string fileToReturn) {

	std::string fileLine;
	std::cout << "File a responder: " << fileToReturn << std::endl;
	std::ifstream file (fileToReturn, std::ios::binary);
    if (!file.is_open())
	{
        std::cerr << "File error" << std::endl;
        exit (1);
	}
	std::cout << "Lee bien el fichero para enviar la respuesta" << std::endl;
	char c;
	while (file.get(c))
		fileLine.push_back(c);
	file.close();
	//std::cout << std::endl << "RESPONSE BODY IS: " << fileLine << std::endl;
	return (fileLine);
}

int	writeEvent(struct client *client)
{
	std::cout << "---WRITE EVENT---" << std::endl;
	//std::string finalRequest  = ResponseToMethod(server, client);
	
	//size_t requestLength = strlen(finalRequest.c_str());
	size_t responseLen = client->response.response.size();
	std::cout << "LEN DE RESPONSE: " << responseLen << std::endl;
	//std::cout << "\033[1;31mRESPONSE IS: " << client->response.response << "\033[0m" << std::endl;
	//while (bytes_sent < requestLength)
	client->response.bytesSent += send(client->fd, 
				&client->response.response.c_str()[client->response.bytesSent], responseLen - client->response.bytesSent, MSG_DONTWAIT);
	std::cout << "\033[1;31mENVIADO: " << client->response.bytesSent << "\033[0m" << std::endl;
	if (client->response.bytesSent == responseLen)
	{
		client->state = -1;
		return (0);
	}
	return (1);
}