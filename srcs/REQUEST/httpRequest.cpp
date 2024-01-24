#include "../../includes/webserv.hpp"

void	loadRequest(HttpRequest *request) 
{
	std::cout << "~~~~LOAD REQUEST STAGE~~~~" << std::endl;
	
	std::istringstream bufferFile(request->header.c_str());
	std::string line;
	std::cout << "Creó el istringstream" << std::endl;
	//Coger la primera línea
	std::getline(bufferFile, line);
	//Sacar: método, url (y versión)
	size_t methodLength = line.find(' ');
    size_t urlLength = line.find(' ', methodLength + 1);
    if (methodLength != std::string::npos && urlLength != std::string::npos) {
        //request->method = line.substr(0, methodLength);
		request->method = line.substr(0, methodLength);
		std::cout << "Method a evaluar: " << request->method << std::endl;
		std::string methods[] = {"GET", "POST", "PUT", "DELETE"};
		for (int i = 0; i < 4; i++)
		{
			if (methods[i] == request->method)
				request->method_int = i;
		}
		std::cout << "Id de method: " << request->method_int << std::endl;
        request->url = line.substr(methodLength + 1, urlLength - methodLength - 1);
		if (request->url.find('?') != std::string::npos) {
			request->query = request->url.substr(request->url.find('?') + 1);
			request->url = request->url.substr(0, request->url.find('?'));
		}
    }
	else
	{
    	perror("invalid HTTP request");
		//return (1);
		throw (400);
    }
	std::cout << std::endl << "METHOD: " << request->method << std::endl;
	std::cout << std::endl << "URL: " << request->url << std::endl;
	//checkear que tipo de accion deberia hacer en base a url - directorio, fichero normal, cgi
	std::string	tokenKey;
	std::string	tokenValue;
	//Montar los headers
	while (std::getline(bufferFile, line)) {
        //Montar el mapa
		std::stringstream	streamLine(line);
		//std::cout << "Línea: " << line << std::endl;
        getline(streamLine, tokenKey, ':');
		//std::cout << "TokenKey: " << tokenKey << std::endl;
		while (getline(streamLine, tokenValue, ';'))
		{
			//std::cout << "TokenValue: " << tokenValue << std::endl;
			size_t	space = tokenValue.find(' ', 0);
			if (space != std::string::npos)
				tokenValue.erase(space, 1);
			request->headers.insert(std::pair<std::string, std::string>(tokenKey, tokenValue));
		}
    }
	std::cout << "-------------------IMPRIMO MAPA-------------------\n\n\n";
	typedef std::multimap<std::string, std::string>::iterator	itm;
	for (itm b = request->headers.begin(), e = request->headers.end(); b != e; b++)
	{
		std::cout << "Key: " << b->first << " | Value: " << b->second << std::endl;
	}
	std::cout << "\n\n-------------------TERMINA DE IMPRIMIR MAPA-------------------\n\n\n";
}

int find_str(const char *haystack, const char *needle, size_t i, size_t size, size_t nsize)
{
	size_t	n = i;
	size_t	e;

	while (n < size)
	{
		e = 0;
		while (haystack[n] == needle[e])
		{
			if (n == size)
				return (-1);
			e++;
			n++;
			if (e == nsize)
				return (n - e);
		}
		n = n - e;
		n++;
	}
	return (-1);
}

int	readHeader(struct client *client)
{
	int		lim = find_str(client->request.buf.c_str(), (const char*)"\r\n\r\n", 0, client->request.buf.size(), 4);
	if (lim < 0)
		return (0);
	if ((size_t)(lim + 4) == client->request.buf.size()) //NO HAY NADA MÁS DESPUÉS DEL HEADER
	{
		client->request.header = client->request.buf;
		client->request.bufLen = 0;
		loadRequest(&client->request);
	}
	else
	{
		client->request.header = client->request.buf.substr(0, lim);
		client->request.bufLen -= lim + 4;
		loadRequest(&client->request);
		std::string	body;
		if (isInMultiMapValue(client->request.headers, "Transfer-Encoding", "chunked\r"))
		{
			client->request.chunk.isChunked = true;
			client->request.buf.clear();
			client->request.chunk.readingSize = true;
			size_t	junk = 0;
			size_t limit; //BARRAERREBARRAENE
			for (size_t j = (lim + 4); j < client->request.bufLen;)
			{
				if (client->request.chunk.readingSize)
				{
					limit = locate(client->request.buf.c_str(), "\r\n", j, client->request.bufLen, 2);
					while (std::strchr("0123456789ABCDEF", client->request.buf[j]) && j < client->request.bufLen)
					{
						client->request.chunk.stringHex += client->request.buf[j];
						j++;
					}
					if (j >= client->request.bufLen)
						break;
					client->request.chunk.readingSize = false;
					client->request.chunk.size = strtol(client->request.chunk.stringHex.c_str(), NULL, 16);
				}
				else
				{
					limit = locate(client->request.buf.c_str(), "\r\n", j, client->request.bufLen, 2);
					if (limit == 0)
					{
						j += 2;
						junk += 2;
					}
					else
					{
						while (j < limit && j < client->request.bufLen)
						{
							body += client->request.buf[j];
							j++;
						}
					}
				}
			}
			client->request.bufLen -= junk;
			client->request.buf.reserve(client->request.bufLen);
			for (size_t i = 0; i < client->request.bufLen; i++)
				client->request.buf[i] = body[i];
		}
		else
		{
			client->request.chunk.isChunked = false;
			body.reserve(client->request.bufLen);
			for (size_t i = 0, j = lim + 4; i < client->request.bufLen; i++, j++)
				body[i] = client->request.buf[j];
			client->request.buf.reserve(client->request.bufLen);
			for (size_t i = 0; i < client->request.bufLen; i++)
				client->request.buf[i] = body[i];
		}
	}
	client->state = 1;
	return (1);
}

int	readBodyChunked(struct client *client)
{
	typedef std::multimap<std::string, std::string>::iterator itm;
	itm	it = client->request.headers.find("Transfer-Encoding");
	if (it != client->request.headers.end())
	{
		if (it->second == "chunked")
		{
			std::cout << "Chunked" << std::endl;
		}
		return (0);
	}
	return (1);
}

int	readBody(struct client *client)
{
	typedef std::multimap<std::string, std::string>::iterator itm;
	if (client->request.chunk.isChunked)
	{
		
	}
	else
	{
		size_t	contentLen;
		itm	it = client->request.headers.find("Content-Length");
		if (it != client->request.headers.end())
		{
			std::pair<itm, itm>	keyVal = client->request.headers.equal_range("Content-Length");
			contentLen = atoi(keyVal.first->second.c_str());
			if (client->request.bufLen > contentLen)
				return (-1); // ESTO ES UN THROW
			if (client->request.bufLen == contentLen)
			{
				client->state = 2;
				return (1);
			}
		}
		else
		{
			client->state = 2;
			return (1);
		}
	}
	return (0);
}

int	readEvent(struct client *client)
{
    char buf[BUF_SIZE + 1];
	memset(buf, 0, sizeof(BUF_SIZE));
	int	bytes_read = recv(client->fd, buf, BUF_SIZE, MSG_DONTWAIT);
	if (bytes_read == -1) {
		perror("recv");
		return (1); //HAY QUE HACER ALGO ¿?
	}
	if (bytes_read == 0) {
		/*//std::cout << "[[CLOSE]]" << std::endl;
		close(cli->ident);*/
		return (2);
	}
	buf[bytes_read] = '\0';
	size_t	size = client->request.bufLen;
	client->request.bufLen += bytes_read;
	client->request.buf.resize(client->request.bufLen);
	for (size_t j = 0; size < client->request.bufLen; size++, j++)
		client->request.buf[size] = buf[j];
	for (size_t i = 0; i < client->request.bufLen; i++)
	if (client->state == 0) //ESTOY EN MODO DE LEER EL HEADER
		readHeader(client);
	if (client->state == 1)
		readBody(client);
	system("leaks -q webserv");
	return (0);
}