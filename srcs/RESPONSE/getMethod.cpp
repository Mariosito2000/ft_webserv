#include "../../includes/webserv.hpp"

static void	getIndex(client *client)
{
	if (!isInMultiMapKey(client->loc->context._dirs, "index"))
		return ;
	itr itk = client->loc->context._dirs.equal_range("index");

	std::string	path;
	std::vector<std::string>	redirs;
	redirs.push_back("alias");
	redirs.push_back("root");
	for (itmap itb = itk.first, ite = itk.second; itb != ite; itb++)
	{
		path = getPathFileRequest(client, redirs) + itb->second;
		if (!access(path.c_str(), F_OK | R_OK))
		{
			client->request.status = 200;
			std::string body = getResponseBody(path);
			client->response.response = getResponseHeader(client->request, body) + body;
			return ;
		}
	}
	throw (NOT_FOUND);
}

static void	autoIndexListing(client *client)
{
	if (!isInMultiMapValue(client->loc->context._dirs, "autoindex", "on"))
		return ;
	std::vector<std::string>	redirsVec;
	redirsVec.push_back("alias");
	redirsVec.push_back("root");
	std::string	path = getPathFileRequest(client, redirsVec);
	DIR	*dir = opendir(path.c_str());
	if (!dir)
		throw (BAD_REQUEST);
	dirent *elem = readdir(dir);
	std::string body;
	std::string	redirs[] = {"alias", "root"};
	body += "<html><body>";
	while (elem)
	{
		if (elem->d_name[0] != '.')
		{
			body += "<a href=\"http://";
			body += *(getMultiMapValue(client->request.headers, "Host"));
			//body += "/" + getMultiMapValueKeys(client->loc->context._dirs, redirs, 2);
			body += client->loc->context._args[0] + '/';
			body += elem->d_name;
			if (elem->d_type == DT_DIR)
				body += '/';
			body += "\">";
			body += elem->d_name;
			body += "\n</a>";
		}
		elem = readdir(dir);
	}
	body += "</body></html>";
	std::cout << "BODY AUTOINDEX: " << body << std::endl;
	closedir(dir);
	client->request.status = 200;
	client->response.response = getResponseHeader(client->request, body) + body;
}

static void pathIsDirectory(client *client)
{
	std::string dirs[] = {"autoindex", "index"};
	void (*f[])(struct client *client) = {autoIndexListing, getIndex};

	for (size_t i = 0; i < 2; i++)
	{
		if (getMultiMapValue(client->loc->context._dirs, dirs[i]))
		{
			f[i](client);
			if (!client->response.response.empty())
			{
				client->request.status = 200;
				return ;
			}	
		}
	}
	throw (BAD_REQUEST);
}

static void	pathIsFile(client *client, std::string &path)
{
	if (access(path.c_str(), F_OK) != 0)
		throw (NOT_FOUND);
	if (access(path.c_str(), R_OK) != 0)
		throw (FORBIDDEN);

	HttpResponse Response;
	client->request.status = 200;
	Response.body = getResponseBody(path);
	Response.firstLine = getResponseHeader(client->request, Response.body);
	client->response.response = Response.firstLine + Response.body;
}

void	getMethod(client *client)
{
	std::vector<std::string>	redirs;
	redirs.push_back("alias");
	redirs.push_back("root");
	std::string	path = getPathFileRequest(client, redirs);
	struct stat	st;
	if (stat(path.c_str(), &st) == -1)
		throw (NOT_FOUND);
	if (st.st_mode & S_IFDIR)
		pathIsDirectory(client);
	else
		pathIsFile(client, path);
}