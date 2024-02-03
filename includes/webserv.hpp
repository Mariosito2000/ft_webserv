#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>   
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <dirent.h>
#include <cstdio>
#include <array>
#include <vector>
#include <list>
#include <algorithm>
#include <libc.h>

#define MAX_SE_ELEM 500000
#define MAX_READ 200000
#define BUF_SIZE 200000
#define PORT 8080
#define CGITIMEOUT 3

enum statusCodes {CREATED = 201,
				MOVED_PERMANENTLY = 301,
				BAD_REQUEST = 400,
				FORBIDDEN = 403,
				NOT_FOUND = 404,
				METHOD_NOT_ALLOWED = 405,
				REQUEST_TIMEOUT = 408,
				CONFLICT = 409,
				LENGTH_REQUIRED = 411,
				PAYLOAD_TOO_LARGE = 413,
				INTERNAL_SERVER_ERROR = 500,
				NOT_IMPLEMENTED = 501,
				BAD_GATEWAY = 502,
				GATEWAY_TIMEOUT = 504
};

typedef struct seNode{
	size_t			elem_n;
	char 			elem[MAX_SE_ELEM];
	int				content_bytes;
	struct seNode *next;
} seNode;

typedef struct	seLst{
	seNode *head;
	seNode *tail;
	size_t	size;
	size_t	bytes;
} seLst;

typedef struct	int_tuple{
	int	i;
	int	e;
} int_tuple;

typedef struct	s_context{
	std::string							_name;
	typedef enum types{MAIN, MIME_TYPES, HTTP, SERVER, LOCATION} types;
	short								_type;
	short								_op;
	std::vector<std::string>			_args;
	std::multimap<std::string, std::string>	_dirs;
} t_context;

typedef struct parseTree 
{
	t_context					context;
	std::vector<parseTree *>	childs;
}	parseTree ;

struct HttpResponse {
	std::string firstLine;
    std::string body;
	std::string	response;
	size_t	bytesSent;
};

typedef struct	chunk{
	bool		isChunked; // If true the request body is chunk encoded
	bool		complete; // If true DeChunking complete
	bool		readingSize; // If true we are currently reading the HEX value of the chunk size
	size_t		size; // Size of the current chunk
	size_t		index; // Extra bytes from the body that are not content (necesarios para mirar max body length)
	std::string	stringHex; // String of the HEX value of the chunk size
	std::string	buf; // Content
}	chunk;

typedef struct HttpRequest {
	
	chunk		chunk;
	std::string	buf;
	size_t		bufLen;
	std::string	header;
    std::string method;
	int			method_int;
	typedef enum methods{GET, POST, PUT, DELETE} methods;
    std::string url; //version
	std::string query;
	std::string pathInfo;
    std::multimap<std::string, std::string> headers;
	bool	cgi;
	std::map<std::string, std::string>	cookies;
	int status;
	size_t	contentLen;
} 	HttpRequest;

typedef struct s_ports{
	std::vector<int>	id;
	std::vector<int>	fd;
	size_t				n;
} t_ports ;

typedef struct client {
    int 			fd;
	int				portID;
	int				state; // 0 -> READ HEADER, 1 -> READ BODY, 2 -> GENERATE RESPONSE, 3 -> WRITE RESPONSE
	parseTree 		*server;
	HttpRequest 	request;
	HttpResponse	response;
	parseTree		*loc;
	pollfd 			event;
	size_t			timer;
	bool 			operator==(struct client const &cmp) const
	{
		if (this->fd == cmp.fd)
			return (1);
		return (0);
	}
} client;

typedef struct events
{
	pollfd *events;
	std::list<pollfd>	addEvents;
	std::list<pollfd>	delEvents;
	size_t				n;
}	events;

typedef struct clients
{
	std::vector<client>	clients;
	std::list<client>	addClients;
	std::list<client>	delClients;
}	clients;

typedef struct	servers{
	
	int			*serversFd;
	int			servers_n;
	std::vector<parseTree*>	serversPtr;
}	servers ;

enum	token_type{word, openBracket, closeBracket, endDeclare};

typedef struct	s_token
{
	size_t		line;
	size_t		type;
	std::string	value;
}	t_token;

//seLst functions
seNode	*newseNode();
void	seLstPushBack(seLst &lst, seNode *newNode);
void	seLstFree(seLst &lst);

//MULTIMAP
typedef std::multimap<std::string, std::string>::iterator itmap;
typedef std::pair<itmap, itmap> itr;
std::string *getMultiMapValue(std::multimap<std::string, std::string> &map, std::string key);
bool		isInMultiMapKey(std::multimap<std::string, std::string> &map, std::string key);
bool		isInMultiMapValue(std::multimap<std::string, std::string> &map, std::string key, std::string value);
bool 		isInMultiMapValues(std::multimap<std::string, std::string> &map, std::string key, std::string values[], size_t len);
std::string	getMultiMapValueKeys(std::multimap<std::string, std::string> &map, std::string keys[], size_t len);
bool		multiMapCheckValidValue(std::multimap<std::string, std::string>	&map, std::string key, bool (*f)(std::string &value));

//data structures transforms
char	*seLstToStr(seLst &lst);

//str functions
size_t	countCharinStr(const char *str, char c);

//parse files
bool		tokenizeFile(const char *file, std::vector<t_token> &tokens, std::string &del);
char		*readFileSeLst(int fd);
parseTree	*parseFile(char	*file);

//tree funcs

void		findNode(parseTree *root, parseTree **find_node, std::string find);
bool		findFile(std::string &dirFind, std::string &file);

//sort-insert funcs
void	linearInsertLoc(std::vector<parseTree *> locs, parseTree *add);

//directories-locations
int	cmpLocations(parseTree *loc, parseTree *cmp);
int	cmpDirectives(void *loc, void *cmp);

//--HTTP REQUEST---
int			readEvent(struct client *client);
int			writeEvent(struct client *client);
void		loadRequest(HttpRequest *request);
std::string	getPathFileRequest(client *client, std::vector<std::string>	&redirs);
std::string getResponseBody(std::string fileToReturn);
std::string	getStatus(int status);
std::string getResponseHeader(HttpRequest &currentRequest, std::string &body);
std::string GetResponse(parseTree	*server, std::string &url);
void 		ResponseToMethod(client *client);

//CHUNKED REQUEST
void 	postHeaderChunk(struct client *client, size_t lim);
void	readBodyChunked(struct client *client);

//HTTP METHODS
parseTree	*matchLocation(struct client *client);
void		getMethod(client *client);
void		callMultiPart(struct client *client, std::string &path);
void		postMultiPartForm(std::string &route, const char *body, std::string &boundary, size_t size);
void		postText(std::string &route, const char *body, size_t size);
void		postUrlEncoded(std::string &route, const char *body, size_t size);

//LOCATE
size_t locate(const char *haystack, const char *needle, size_t i, size_t size, size_t nsize);

//---SOCKET---
void 	setNonBlocking(int fd);
int		getServerSocket(sockaddr_in *addr, int port);
void	bindAndListen(int sock, sockaddr_in *addr);
int		pollEvents(std::vector<parseTree *> &servers, t_ports *ports);

//---CGI---
std::string getCgi(std::string script);
void		CGIForward(client *client);

//ERRORS
std::string	getStatus(int status);
std::string	getErrorPath(struct client *client, int error);
void		getErrorResponse(struct client *client, int error);