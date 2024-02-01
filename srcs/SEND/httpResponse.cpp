#include "../../includes/webserv.hpp"

std::string getResponseHeader(HttpRequest &currentRequest, std::string &body) {

	std::string line = "HTTP/1.1 ";
	line.append(getStatus(currentRequest.status));
	line.append("\r\n");
	if (!body.empty()) {
		line.append("Content-Length: ");
		line.append(std::to_string((body).size()));
	}
	line.append("\r\n\r\n");
	std::cout << std::endl << "RESPONSE HEADER IS: " << line << std::endl;
	return (line);
}

std::string getResponseBody(std::string fileToReturn)
{
	std::string fileLine;
	if (access(fileToReturn.c_str(), F_OK | R_OK))
		throw (NOT_FOUND);
	std::ifstream file (fileToReturn, std::ios::binary);
    if (!file.is_open())
	{
        std::cerr << "File error" << std::endl;
        throw (INTERNAL_SERVER_ERROR);
	}
	char c;
	while (file.get(c))
		fileLine.push_back(c);
	file.close();
	return (fileLine);
}

int	writeEvent(struct client *client)
{
	std::cout << "WRITE EVENT | CLIENT: " << client->fd << std::endl;
	size_t responseLen = client->response.response.size();
	std::cout << "LEN DE RESPONSE: " << responseLen << std::endl;
	size_t	bytesSent = send(client->fd, 
				&client->response.response.c_str()[client->response.bytesSent], responseLen - client->response.bytesSent, MSG_DONTWAIT);
	if (bytesSent <= 0)
		return (-1);
	client->response.bytesSent += bytesSent;
	std::cout << "\033[1;31mENVIADO: " << client->response.bytesSent << "\033[0m" << std::endl;
	std::cout << "LONGITUD DE RESPONSE: " << responseLen << std::endl;
	if (client->response.bytesSent == responseLen)
	{
		client->state = -1;
		return (0);
	}
	return (1);
}