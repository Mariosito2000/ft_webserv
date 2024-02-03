#include "../../includes/webserv.hpp"

size_t locate(const char *haystack, const char *needle, size_t i, size_t size, size_t nsize)
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

std::string	get_filename(std::vector<char> &header)
{
	std::string	filename;
	char cheader[header.size()];
	for (size_t i = 0; i < header.size(); i++)
		cheader[i] = header[i];
	size_t start = locate(cheader, "filename=\"", 0, header.size(), 10) + 10;
	if (start == (size_t)-1)
		throw (BAD_REQUEST);
	while (start < header.size() && header[start] != '\"')
	{
		filename += cheader[start];
		start++;
	}
	if (header[start] != '\"')
		throw (BAD_REQUEST);
	return (filename);
}

void	create_files(std::map<std::string, std::vector<char> >files, std::string route)
{
	for (std::map<std::string, std::vector<char> >::iterator iter = files.begin(); iter != files.end(); iter++)
	{
		std::ofstream	newfile(route + iter->first, std::ios::binary | std::ios::trunc);
		for (size_t i = 0; i < iter->second.size(); i++)
			newfile << iter->second[i];
		newfile.close();
	}
}

void	postMultiPartForm(std::string &route, const char *body, std::string &boundary, size_t size)
{
	std::map<std::string, std::vector<char> >	files;
	std::vector<char>	fheader;
	std::vector<char>	fcont;
	std::string			filename;
	size_t		limit;
	size_t		i = 0;
	size_t		c = 0;

	while (i < size)
	{
		fheader.push_back(body[i]);
		i++;
		c++;
		if (c >= 4 && fheader[c - 1] == '\n' && fheader[c - 2] == '\r' && fheader[c - 3] == '\n' && fheader[c - 4] == '\r')
		{
			limit = locate(body, ("\r\n--" + boundary).c_str(), i, size, ("\r\n--" + boundary).size() - 1);	
			if (limit == (size_t)-1)
				throw (BAD_REQUEST);
			while (i < limit)
			{
				fcont.push_back(body[i]);
				i++;
			}
			filename = get_filename(fheader);
			if (!access((route + filename).c_str(), F_OK))
				throw(CONFLICT);
			for (std::map<std::string, std::vector<char> >::iterator iter = files.begin(); iter != files.end(); iter++)
				if (filename == iter->first)
					throw(BAD_REQUEST);
			files.insert(std::pair<std::string, std::vector<char> >(filename, fcont));
			fcont.clear();
			fheader.clear();
			c = 0;
		}
	}
	create_files(files, route);
}

void	callMultiPart(struct client *client, std::string &path)
{
	itr itm;
	itm = client->request.headers.equal_range("Content-Type");
	char	*boundary = NULL;
	for (std::multimap<std::string, std::string>::iterator itb = itm.first; itb != itm.second; itb++)
	{
		if (!itb->second.compare(0, 9, "boundary="))
		{
			boundary = (char *)(itb->second.c_str() + 9);
			break ;
		}
	}
	if (!boundary)
		throw (BAD_REQUEST);
	std::string	boundaryStr(boundary);
	postMultiPartForm(path, client->request.buf.c_str(), boundaryStr, client->request.bufLen);
}