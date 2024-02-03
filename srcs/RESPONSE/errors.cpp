#include "../../includes/webserv.hpp"

std::string	getStatus(int status)
{
	switch (status) {
        case CREATED:
            return "201 Created";
		case MOVED_PERMANENTLY:
			return "301 Moved Permanently";
        case BAD_REQUEST:
            return "400 Bad Request"; //El formato de la request está mal
        case FORBIDDEN:
            return "403 Forbidden"; //Acceso no permitido
        case NOT_FOUND:
            return "404 Not Found"; //Archivo no encontrado
        case METHOD_NOT_ALLOWED:
            return "405 Method Not Allowed"; //Método que no gestionamos (DELETE de CGI o cualquier otro)
        case REQUEST_TIMEOUT:
            return "408 Request Timeout"; //Timeout
		case CONFLICT:
            return "409 Conflict";
		case LENGTH_REQUIRED:
            return "411 Length Required";
        case PAYLOAD_TOO_LARGE:
            return "413 Payload Too Large"; //Request muy grande
        case INTERNAL_SERVER_ERROR:
            return "500 Internal Server Error";
        case NOT_IMPLEMENTED:
            return "501 Not Implemented";
        case BAD_GATEWAY:
            return "502 Bad Gateway";
        case GATEWAY_TIMEOUT:
            return "504 Gateway Timeout";
        default:
            return "200 OK";
    }
}

std::string	getErrorPath(struct client *client, int error)
{
	if (client->loc)
	{
		typedef std::multimap<std::string, std::string>::iterator itm;
		
		itm it = client->loc->context._dirs.find("error_files");
		if (it != client->loc->context._dirs.end())
		{
			std::pair<itm, itm> itr = client->loc->context._dirs.equal_range("error_files");
			for (itm ib = itr.first, ie = itr.second; ib != ie; ib++)
			{
				int	n = atoi(ib->second.c_str());
				if (n == error)
				{
					ie--;
					return (ie->second);
				}
			}
		}
	}
	return ("errors/error" + std::to_string(error) + ".html");
}

void	getErrorResponse(struct client *client, int error)
{
	std::string	resp;
	std::string	path;
	std::string body;
	path = getErrorPath(client, error);
	try
	{
		body = getResponseBody(path);
	}
	catch(enum statusCodes e)
	{
		body = "<html><body><h1>ERROR FILE NOT FOUND</h1></body></html>";
	}
	client->response.response = getResponseHeader(client->request, body) + body;
}
