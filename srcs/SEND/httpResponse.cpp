#include "../../includes/webserv.hpp"

std::string getResponseHeader(HttpRequest &currentRequest, std::string &body) {

	std::string line = "HTTP/1.1 ";
	line.append(getStatus(currentRequest.status));
	line.append("\r\n");
	line.append("Set-Cookie: hello=cookies\r\n");
	if (!body.empty()) {
		line.append("Content-Length: ");
		line.append(std::to_string((body).size()));
	}
	line.append("\r\n\r\n");
	std::cout << std::endl << "URL: " << currentRequest.url << "\nRESPONSE HEADER IS: " << line << std::endl;
	return (line);
}

std::string getResponseBody(std::string fileToReturn)
{
	std::string fileLine;
	if (access(fileToReturn.c_str(), F_OK | R_OK))
		throw (NOT_FOUND);
	std::ifstream file (fileToReturn, std::ios::binary);
    if (!file.is_open())
        throw (INTERNAL_SERVER_ERROR);
	char c;
	while (file.get(c))
		fileLine.push_back(c);
	file.close();
	return (fileLine);
}

int	writeEvent(struct client *client)
{
	size_t responseLen = client->response.response.size();
	size_t	bytesSent = send(client->fd, 
				&client->response.response.c_str()[client->response.bytesSent], responseLen - client->response.bytesSent, MSG_DONTWAIT);
	if (bytesSent <= 0)
		return (-1);
	client->response.bytesSent += bytesSent;
	if (client->response.bytesSent == responseLen)
	{
		std::cout << "MANDÓ TODO" << std::endl;
		//client->state = -1;
		return (0);
	}
	return (1);
}