#include "../../includes/webserv.hpp"

std::string	getStatus(int status)
{
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

std::string	defaultErrorPath(int error)
{
	std::cout << "DEFAULT ERROR PATH" << std::endl;
	char		buf[1000];
	std::string	absPath = getcwd(buf, 1000);
	std::string	errorPath;
	
	switch (error)
	{
		//client errors
		case 400: {errorPath = absPath + "/errors" + "/error400.html" ; break ; }
		case 403: {errorPath = absPath + "/errors" + "/error403.html" ; break ; }
		case 404: {errorPath = absPath + "/errors" + "/error404.html" ; break ; }
		//server errors
		case 500: {errorPath = absPath + "/errors" + "/error500.html" ; break ; }
	}
	return (errorPath);
}

std::string	getErrorPath(struct client *client, int error)
{
	char		buf[1000];
	std::string	absPath = getcwd(buf, 1000);
	std::string	errorPath;

	std::cout << "---------Hubo error: " << error << " , mandar error path--------" << std::endl;
	//mirar si en config hay ficheros de error redirigidos
	bTreeNode	&loc = *(client->loc);
	typedef std::multimap<std::string, std::string>::iterator itm;
	
	itm it = loc.directivesMap.find("error_files");
	if (it != loc.directivesMap.end())
	{
		std::pair<itm, itm> itr = loc.directivesMap.equal_range("error_files");
		std::cout << "Encuentra error_files en location" << std::endl;
		for (itm ib = itr.first, ie = itr.second; ib != ie; ib++)
		{
			std::cout << "Key: " << ib->first << " | Value: " << ib->second << std::endl;
			int	n = atoi(ib->second.c_str());
			std::cout << "error de error_files: " << n << std::endl;
			if (n == error)
			{
				ie--;
				std::cout << "Fichero a redirigir: " << "Key: " << ie->first << " | Value: " << ie->second << std::endl;
				errorPath = absPath + ie->second;
				return (errorPath);
			}
		}
	}
	errorPath = defaultErrorPath(error);
	return (errorPath);
}

std::string	getErrorResponse(struct client *client, int error)
{
	std::string	resp;
	std::string	path;

	path = getErrorPath(client, error);
	std::string body = getResponseBody(path);
	resp = getResponseHeader(client->request, body) + body;
	return (resp);
}
