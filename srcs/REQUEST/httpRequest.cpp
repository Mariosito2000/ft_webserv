#include "../../includes/webserv.hpp"

bool	strIsDigit(std::string &s)
{
	for (size_t i = 0; i < s.size(); i++)
		if ((s[i] < '0' || s[i] > '9') && s[i] != '\r')
			return (false);
	return (true);
}

void	loadRequest(HttpRequest *request) 
{
	std::istringstream bufferFile(request->header.c_str());
	std::string line;
	std::getline(bufferFile, line);
	size_t methodLength = line.find(' ');
    size_t urlLength = line.find(' ', methodLength + 1);
    if (methodLength != std::string::npos && urlLength != std::string::npos) {
		request->method = line.substr(0, methodLength);
		std::string methods[] = {"GET", "POST", "PUT", "DELETE"};
		for (int i = 0; i < 4; i++)
		{
			if (methods[i] == request->method)
				request->method_int = i;
		}
        request->url = line.substr(methodLength + 1, urlLength - methodLength - 1);
		if (request->url.find(".py") != std::string::npos ||
			request->url.find(".pl") != std::string::npos)
		{
			request->cgi = true;
			if (request->url.find('?') != std::string::npos) {
				request->query = request->url.substr(request->url.find('?') + 1);
				request->url = request->url.substr(0, request->url.find('?'));
			}
			else if (request->url.find('.') + 3 != request->url.size()
				&& request->url.at(request->url.find('.') + 3) == '/') {
				request->pathInfo = request->url.substr(request->url.find('.') + 3);
				request->url = request->url.substr(0, request->url.find('.') + 3);
			}
		}
		else
			request->cgi = false;
    }
	else
		throw (BAD_REQUEST);

	std::cout << "METHOD: " << request->method << std::endl;
	std::cout << "URL: " << request->url << std::endl;

	std::string	tokenKey;
	std::string	tokenValue;
	while (std::getline(bufferFile, line)) {
		std::stringstream	streamLine(line);
        getline(streamLine, tokenKey, ':');
		while (getline(streamLine, tokenValue, ';'))
		{
			size_t	space = tokenValue.find(' ', 0);
			if (space != std::string::npos)
				tokenValue.erase(space, 1);
			request->headers.insert(std::pair<std::string, std::string>(tokenKey, tokenValue));
		}
    }
	if (!multiMapCheckValidValue(request->headers, "Content-Length", strIsDigit))
		throw (BAD_REQUEST);
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

void	setChunked(chunk &chunk, bool state, bool complete, bool size, int index)
{
	chunk.isChunked = state;
	chunk.complete = complete;
	chunk.readingSize = size;
	chunk.index = index;
}

void	readHeader(struct client *client)
{
	int		lim = find_str(client->request.buf.c_str(), (const char*)"\r\n\r\n", 0, client->request.buf.size(), 4);
	
	if (lim < 0)
		return ;
	
	if ((size_t)(lim + 4) == client->request.buf.size()) // NO BODY AFTER HEADER
	{
		
		client->request.header = client->request.buf;
		client->request.bufLen = 0;
		loadRequest(&client->request);
		if(isInMultiMapValue(client->request.headers, "Transfer-Encoding", "chunked\r")
				|| isInMultiMapValue(client->request.headers, "Transfer-Encoding", "chunked"))
			setChunked(client->request.chunk, true, false, true, 0);
		else
		{
			if (!isInMultiMapKey(client->request.headers, "Content-Length"))
				client->state = 2;
			client->request.chunk.isChunked = false;
		}
			
	}
	else // BODY AFTER HEADER
	{
		client->request.header = client->request.buf.substr(0, lim);
		client->request.bufLen -= lim + 4;
		loadRequest(&client->request);
		std::string	body;
		body.reserve(client->request.bufLen);
		for (size_t i = 0, j = lim + 4; i < client->request.bufLen; i++, j++)
			body[i] = client->request.buf[j];
		client->request.buf.reserve(client->request.bufLen);
		for (size_t i = 0; i < client->request.bufLen; i++)
			client->request.buf[i] = body[i];
		if (isInMultiMapValue(client->request.headers, "Transfer-Encoding", "chunked\r")
				|| isInMultiMapValue(client->request.headers, "Transfer-Encoding", "chunked"))
		{
			setChunked(client->request.chunk, true, false, true, 0);
			readBodyChunked(client);
		}
		else
		{
			client->request.chunk.isChunked = false;
			if (!isInMultiMapKey(client->request.headers, "Content-Length"))
				throw (LENGTH_REQUIRED);
			if (!multiMapCheckValidValue(client->request.headers, "Content-Length", strIsDigit))
				throw (BAD_REQUEST);
		}
	}
	if (client->state == 2)
		return ;
	client->state = 1;
}

void	readBody(struct client *client)
{
	if (client->request.chunk.isChunked)
		readBodyChunked(client);
	else
	{
		if (!isInMultiMapKey(client->request.headers, "Content-Length"))
			throw (LENGTH_REQUIRED);
		if (!multiMapCheckValidValue(client->request.headers, "Content-Length", strIsDigit))
			throw (BAD_REQUEST);
		std::string *contentLenStr = getMultiMapValue(client->request.headers, "Content-Length");
		if (contentLenStr)
		{
			size_t	contentLen = atoi(contentLenStr->c_str());
			if (client->request.bufLen > contentLen)
				throw (BAD_REQUEST);
			if (client->request.bufLen == contentLen)
				client->state = 2;
		}
		else
			client->state = 2;
	}
}

int	readEvent(struct client *client)
{
    char buf[BUF_SIZE + 1];
	memset(buf, 0, sizeof(BUF_SIZE));
	int	bytes_read = recv(client->fd, buf, BUF_SIZE, MSG_DONTWAIT);
	if (bytes_read == -1)
		return (1);
	if (bytes_read == 0)
		return (2);
	buf[bytes_read] = '\0';

	size_t	size = client->request.bufLen;
	client->request.bufLen += bytes_read;
	client->request.buf.resize(client->request.bufLen);
	for (size_t j = 0; size < client->request.bufLen; size++, j++)
		client->request.buf[size] = buf[j];
	if (client->state == 0)
		readHeader(client);
	if (client->state == 1)
		readBody(client);
	if (client->state == 2 && client->request.chunk.isChunked)
	{
		client->request.bufLen = client->request.chunk.buf.size();
		client->request.buf = client->request.chunk.buf;
	}
	return (0);
}
