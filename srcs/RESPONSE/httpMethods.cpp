#include "../../includes/webserv.hpp"

/*std::string	getPathFileRequest(bTreeNode *location, client *client)
//pasar como argumento un array de palabras claves donde buscar el directorio relativo
{
	std::cout << "Location es: " << location->contextArgs[0] << std::endl;
	std::cout << "URL es: " << client->request.url << std::endl;
	std::string	pathFile;
	
	int	locLen = location->contextArgs[0].length();
	//coger path absoluto
	char	buf[1000];
	std::string absPath = getcwd(buf, 1000);
	std::string	relPath;
	std::string filePath; //cambiar a file solo
	
	//BUSCAR SI TIENE CGI
	typedef std::multimap<std::string, std::string>::iterator itmap;
	itmap itm = location->directivesMap.find("cgi_pass");
	if (itm != location->directivesMap.end())
	{
		std::cout << "Encontró la key: " << itm->first << " | Valor: " << itm->second << std::endl;
		std::string	fileDir = client->request.url.substr(locLen, locLen - itm->second.length());
		std::cout << "fileDir: " << fileDir << std::endl;
		std::string	filePath = absPath + itm->second + fileDir;
		client->request.cgi = 1;
		std::cout << "filePath: " << filePath << std::endl;
		return (filePath);
	}
	switch(client->request.method_int)
	{
		case HttpRequest::POST : { 
			itm = location->directivesMap.find("postdir");
			if (itm == location->directivesMap.end())
			{
				std::cout << "No encontró la key de postdir" << std::endl;
				throw(404);
			}
			filePath = absPath + itm->second; break ;} 
		default : {
			std::string	keys[] = {"alias", "root"};
			int	i;
			for (i = 0; i < 2; i++) //esto rehacerlo para que llame a otra funcion - rehacer getValue
			{
				std::cout << "Key tipo: " << keys[i] << std::endl;
				itm = location->directivesMap.find(keys[i]);
				if (itm != location->directivesMap.end())
				{
					std::cout << "Encontró la key" << std::endl;
					break ;
				}
			}
			std::cout << "Key tipo encontrada: " << keys[i] << std::endl;
			std::string	fileDir = client->request.url.substr(locLen, locLen - itm->second.length() + 1);
			//std::string	fileDir;
			//std::cout << "FileDir: " << fileDir << std::endl;
			if (client->request.url[client->request.url.size() - 1] == '/')// es un directorio
			{
				std::cout << "FileDir vacío, usar index" << std::endl;
				typedef std::pair<itmap, itmap> itr;
				itr itk = location->directivesMap.equal_range("try_files");
				if (itk.first != location->directivesMap.end())
				{
					std::cout << "Encontró la key" << std::endl;
					break ;
				}
				for (itmap itb = itk.first, ite = itk.second; itb != ite; itb++)
				{
					fileDir = itb->second;
					if (!access(itb->second.c_str(), F_OK))
					{
						break ;
					}
				}
				if (fileDir.empty())
					throw(404);
			}
			std::cout << "fileDir: " << fileDir << std::endl;
			switch (i) //esto sacarlo fuera y hacerlo antes
			{
				case 0: { std::cout << "Usar Alias: " << std::endl;
							filePath = absPath + itm->second + fileDir; break ;} //alias
				case 1: { std::cout << "Usar Root: " << std::endl;
							filePath = absPath + itm->second + location->contextArgs[0] + fileDir ; break ;} //root
			}
		}
	}
	std::cout << "filePath: " << filePath << std::endl;
	return (filePath);
}*/

typedef std::multimap<std::string, std::string>::iterator itmap;
typedef std::pair<itmap, itmap> itr;

std::string	getRedir(struct client *client, std::string &absPath, itmap &redir, std::string &file)
{
	std::cout << "En getRedir" << std::endl;
	std::string redirs[] = {"postdir", "alias", "root"};
	std::string	&loc = client->loc->contextArgs[0];
	std::string path;
	size_t		i;

	for (i = 0; i < 3; i++)
	{
		if (redir->first == redirs[i])
		{
			std::cout << "Encontró: " << redirs[i] << std::endl;
			switch (i)
			{
				case 0 : { path = absPath + redir->second; break ;} //postdir
				case 1 : { path = absPath + redir->second + file; break ;} //alias
				case 2 : { path = absPath + redir->second + loc + file; break ;} //root
			}
			break ;
		}
	}
	if (i == 3)
		throw (404);
	return (path);
}


std::string	findLocIndex(client *client, std::string &absPath)
{
	std::cout << "Es un directorio, usar index" << std::endl;
	bTreeNode &loc = *(client->loc);
	itr itk = loc.directivesMap.equal_range("index");
	if (itk.first != loc.directivesMap.end())
		std::cout << "Encontró la key de index: " << std::endl;
	std::string	path;
	for (itmap itb = itk.first, ite = itk.second; itb != ite; itb++)
	{
		path = absPath + itb->second;
		std::cout << "path de file index: " << path << std::endl;
		if (!access(path.c_str(), F_OK | R_OK))
		{
			std::cout << "Existe fichero " << std::endl;
			return (path);
		}	
	}
	throw (404);
}

std::string	getPathFileRequest(client *client, std::vector<std::string>	&redirs)
//pasar como argumento un array de palabras claves donde buscar el directorio relativo
{
	std::cout << "En getPathFileRequest" << std::endl;
	bTreeNode	&loc = *(client->loc);
	std::cout << "Location es: " << loc.contextArgs[0] << std::endl;
	std::cout << "URL es: " << client->request.url << std::endl;
	std::string	pathFile;
	
	int	locLen = loc.contextArgs[0].length();
	//coger path absoluto
	char	buf[1000];
	std::string absPath = getcwd(buf, 1000);
	std::string file; //cambiar a file solo, el resto de la url
	
	std::multimap<std::string, std::string>::iterator	itm;
	size_t i;
	for (i = 0; i < redirs.size(); i++)
	{
		std::cout << "Key tipo: " << redirs[i] << std::endl;
		itm = loc.directivesMap.find(redirs[i]);
		if (itm != loc.directivesMap.end())
		{
			std::cout << "Encontró la key" << std::endl;
			break ;
		}
	}
	if (i == redirs.size())
		throw (400);
	if (client->request.url[client->request.url.size() - 1] == '/') //es un directorio
		pathFile = findLocIndex(client, absPath);
	else //es un fichero
	{
		file = client->request.url.substr(locLen, locLen - itm->second.length() + 1);
		pathFile = getRedir(client, absPath, itm, file);
	}
	return (pathFile);
}

std::string	getRequestedFile(struct client *client, std::vector<std::string> &redirs) //sacar location y pasarlo como argumento
{
	std::cout << "URL: " << client->request.url << std::endl;
	//bTreeNode	*loc = findLocation(server, client->request.url);
	std::string	filePath;
	std::string path;
	if (!client->loc)
	{
		std::cout << "No encontró loc" << std::endl;
		client->request.status = 404;
		throw (404);
	}
	filePath = getPathFileRequest(client, redirs);
	std::cout << std::endl << "FILEPATH IS:" << filePath << std::endl; 
	return (filePath); 
}

std::string	getMethod(client *client) {
	
	std::cout << "GET METHOD" << std::endl;
	std::string filePath;
	std::vector<std::string>	redirs;
	redirs.push_back("alias");
	redirs.push_back("root");
	filePath = getRequestedFile(client, redirs);
	std::cout << "filePath: " << filePath << std::endl;
	if (access(filePath.c_str(), F_OK) != 0)
		throw (404);
	else if (access(filePath.c_str(), R_OK) != 0)
		throw (403);
	HttpResponse Response;
	client->request.status = 200;
	Response.body = getResponseBody(filePath);
	Response.firstLine = getResponseHeader(client->request, Response.body);
	//if (client->request.cgi == 1)
	//	Response.body = CGIForward(filePath);
	std::string finalRequest = Response.firstLine + Response.body;
	return (finalRequest);
}

std::string	postMethod(client *client)
{
	std::cout << "ESTOY EN POST" << std::endl;
	std::cout << "BODY REQUEST: " << client->request.buf << std::endl;
	std::string filePath;
	std::vector<std::string>	redirs;
	redirs.push_back("postdir");
	filePath = getRequestedFile(client, redirs);
	std::multimap<std::string, std::string>::iterator itm;
	itm = client->request.headers.find("Content-Type");
	if (itm != client->request.headers.end())
	{
		std::cout << "Encontró la key: " << itm->first << " , Value: " << itm->second << std::endl;
	}
	//DIFERENTES TIPOS DE POST: APPLICATION, MULTIPART-FORM, TEXT-PLAIN
	std::cout << "TIPO DE POST: " << itm->second << std::endl;
	if (itm->second == "application/x-www-form-urlencoded\r")
		postUrlEncoded(filePath, client->request.buf.c_str(), client->request.bufLen);
	else if (itm->second == "multipart/form-data")
		callMultiPart(client, filePath);
	else if (itm->second == "text/plain\r")
		postText(filePath, client->request.buf.c_str(), client->request.bufLen);
	else
		throw (400);
	//REHACER
	client->request.status = 201;
	return ("HTTP/1.1 201 Created\r\n\r\n");
}

std::string	deleteMethod(client *client)
{
	std::cout << "Delete method" << std::endl;
	
	struct stat	st;
	std::string filePath;
	std::vector<std::string>	redirs;
	redirs.push_back("alias");
	redirs.push_back("root");
	filePath = getRequestedFile(client, redirs);
	std::cout << "filePath: " << filePath << std::endl;
	if (stat(filePath.c_str(), &st) == 0 && st.st_mode & S_IFDIR)
    {
		std::cout << "Es un directorio"  <<std::endl;
		if (rmdir(filePath.c_str()) < 0)
		{
			//client->request.status = 500;
			throw(500);
		}	
	}
	else
	{
		std::cout << "Es un fichero" << std::endl;
		if (remove(filePath.c_str()) < 0)
		{
			//client->request.status = 500;
			throw(500);
		}
	}
	client->request.status = 200;
	return ("HTTP/1.1 200 OK\r\n\r\n<html>	<body><h1>File deleted.</h1>\n  </body>\n</html>\n");
}

bool	isInMultiMap(std::multimap<std::string, std::string> &map, std::string key, std::string value)
{
	typedef std::map<std::string, std::string>::iterator	itmap;
	itmap it = map.find(key);
	if (it == map.end())
		return (false);
	typedef std::pair<itmap, itmap> itr;
	itr iti = map.equal_range(key);
	for (itmap itb = iti.first, ite = iti.second; itb != ite; itb++)
	{
		if (itb->second == value)
			return (true);
	}
	return (false);
}

std::string	autoIndexListing(client *client)
{
	std::cout << "Entro en autoindex" << std::endl;
	char	buf[1000];
	std::string absPath = getcwd(buf, 1000) + client->loc->contextArgs[0];
	std::cout << "Abspath: " << absPath << std::endl;
	DIR	*dir = opendir(absPath.c_str());
	if (!dir)
		throw (400);
	dirent *elem = readdir(dir);
	std::string body;
	body += "<html><body>";
	while (elem)
	{
		std::cout << elem->d_name << std::endl;
		body += "<h1>";
		body += elem->d_name;
		body += "</h1>";
		elem = readdir(dir);
	}
	body += "</body></html>";

	std::string	response;
	
	client->request.status = 200;
	response += getResponseHeader(client->request, body);
	response += body;
	std::cout << "RESPONSE DE AUTOINDEX: " << response << std::endl;
	return (response);
}

std::string ResponseToMethod(client *client)
{
	std::cout << "EN RESPONSE TO METHOD" << std::endl;
	std::string response;
	client->loc = findLocation(client);
	if (!client->loc)
		throw (0);
	//if (!isInMultiMap(client->loc->directivesMap, "methods", client->request.method))
	//	throw (1);
	//probar aqui casos de que url sea un directorio, no fichero
	if (client->request.url[client->request.url.size() - 1] == '/') //es un directorio
	{
		if (isInMultiMap(client->loc->directivesMap, "autoindex", "on"))
		{
			response = autoIndexListing(client);
			client->state = 3;
			return (response);
		}
	}
	std::cout << "MÉTODO PERMITIDO: " << client->request.method << std::endl;
	std::cout << "MÉTODO A EVALUAR: " << client->request.method_int << std::endl;
	try {
		///esto hay que modificarle la carpeta en algún momento
		std::vector<std::string>	redirs;
		redirs.push_back("alias");
		redirs.push_back("root");
		std::string filePath = getRequestedFile(client, redirs);

		if (filePath.find(".py") != std::string::npos ||
				filePath.find(".pl") != std::string::npos ||
				filePath.find(".php") != std::string::npos) {
			std::cout << "Entra en CGI" << std::endl;
			HttpResponse Response;
			client->request.status = 200;
			Response.firstLine = getResponseHeader(client->request, Response.body);
			Response.body = CGIForward(filePath, client);
			response = Response.firstLine + Response.body;
		}
		else {
			//pasar filepath como parámetro o montarlo sobre el cliente en vez d en cada método
			switch(client->request.method_int)
			{
				case HttpRequest::GET : {response = getMethod(client); break ;}
				case HttpRequest::POST : {response = postMethod(client); break ;}
				//case HttpRequest::PUT : {response = getMethod(server, client); break ;}
				case HttpRequest::DELETE : {response = deleteMethod(client); break ;}
			}
		}
	}
	catch(int error) {
		client->request.status = error;
		response = getErrorResponse(client, error);
	}
	client->state = 3;
	return (response);
}