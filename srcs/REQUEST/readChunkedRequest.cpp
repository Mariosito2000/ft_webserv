#include "../../includes/webserv.hpp"

void	readBodyChunked(struct client *client)
{
	size_t	limit;
	for (size_t j = client->request.chunk.index; j < client->request.bufLen;)
	{
		if (client->request.chunk.readingSize)
		{
			limit = locate(client->request.buf.c_str(), "\r\n", client->request.chunk.index, client->request.bufLen, 2);
			if (limit == (size_t)-1)
				break;
			while (j < limit)
			{
				client->request.chunk.stringHex += client->request.buf[j];
				j++;
			}
			client->request.chunk.index += (2 + client->request.chunk.stringHex.size());
			client->request.chunk.readingSize = false;
			client->request.chunk.size = strtol(client->request.chunk.stringHex.c_str(), NULL, 16);
			client->request.chunk.stringHex.clear();
			j += 2;
			if (client->request.chunk.size == 0)
				client->state = 2;
		}
		else
		{
			size_t pos = client->request.chunk.index + client->request.chunk.size;
			if (pos + 1 > client->request.bufLen)
				return ;
			if (client->request.buf[pos] != '\r' || client->request.buf[pos + 1] != '\n')
				throw (BAD_REQUEST);
			size_t read = 0;
			while (read < client->request.chunk.size)
			{
				client->request.chunk.buf += client->request.buf[j];
				read++;
				client->request.chunk.index++;
				j++;
			}
			client->request.chunk.index += 2;
			j += 2;
			client->request.chunk.readingSize = true;
			if (j == client->request.bufLen && client->request.chunk.size == 0)
				client->state = 2;
		}
	}
}