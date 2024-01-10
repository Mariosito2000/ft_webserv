#include "webserv.hpp"

void	findNode(bTreeNode *root, bTreeNode **find_node, std::string	find)
//quiza pasar un puntero a una funcion de comparar, que ya compare los valores para determinar si coincide o no
{
	std::cout << "Estoy en contexto: " << root->contextName << std::endl;

	if (root->contextName == find)
	{
		*find_node = root;
		return ;
	}
	if (!root->childs.empty())
	{
		for (int i = 0; i < root->childs.size(); i++)
			findNode(root->childs[i], find_node, find);
	}
}
/*bool	strcmp_range(char *s1, char *s2, int n1, int n2)
{
	std::cout << "Rango a comparar s1: " << n1 << std::endl;
	std::cout << "Longitud de s1: " << strlen(s1) << std::endl;
	std::cout << "Rango a comparar s2: " << n2 << std::endl;
	std::cout << "Longitud de s2: " << strlen(s2) << std::endl;
	int	range = std::max(n1, n2);
	if (n1 > strlen(s1) || n2 > strlen(s2))
		return (false);
	for (int i = 0; i < range; i++)
	{
		if (!s1 || !s2)
		if (s1[i] != s2[i])
			return (false);
	}
	return (true);
}
int		str_skip_char_index(char const *str, char c)
{
	int	len = strlen(str);
	int i = 0;

	for (; i < len && str[i] == c; i++);
	if (str[i] != '\0')
		return (-1);
	return (i);
}

int		str_search_char_index(char const *str, char c)
{
	int	len = strlen(str);
	for (int i = 0; i < len; i++)
	{
		if (str[i] == c)
			return (i);
	}
	return (-1);
}

int	match_loc_url(char const *loc, char const *url)
{
	int	loc_i = 0;
	int	loc_j;
	int	url_i = 0;
	int	url_j;

	int	loc_len = strlen(loc);
	int	url_len = strlen(url);
	std::cout << "loc: " << loc << std::endl;
	std::cout << "url: " << url << std::endl;
	while (loc_i < loc_len && url_i < url_len)
	{
		loc_i += str_skip_char_index(&loc[loc_i], '/');
		if (loc_i == -1)
			break ;
		std::cout << "loc_i después de skip: " << loc_i << std::endl;
		loc_j = str_search_char_index(&loc[loc_i], '/');
		if (loc_j == -1)
			loc_j = loc_len;
		std::cout << "loc_j después de search: " << loc_j << std::endl;
		url_i += str_skip_char_index(&url[url_i], '/');
		if (url_i == -1)
			break ;
		std::cout << "url_i después de skip: " << url_i << std::endl;
		url_j = str_search_char_index(&url[url_i], '/');
		if (url_j == -1)
			url_j = url_len;
		std::cout << "url_j después de search: " << url_j << std::endl;
		std::cout << "Dir de Location: ";
		for (int i = loc_i; i <= loc_j; i++)
			std::cout << loc[i];
		std::cout << std::endl;
		std::cout << "Dir de URL: ";
		for (int i = url_i; i <= url_j; i++)
			std::cout << url[i];
			std::cout << std::endl;
		int	diff = (loc_j - loc_i) - (url_j - url_i);
		if (diff != 0)
		{
			std::cout << "Rango no es el mismo, no pueden ser iguales" << std::endl;
			return (diff);
		}
		std::cout << "Entro a comparar" << std::endl;
		diff = strncmp(loc, url, loc_j - loc_i);
		if (diff != 0)
		{
			std::cout << "No coinciden" << std::endl;
			return (diff);
		}
		else
			std::cout << "Coinciden" << std::endl;
		loc_i = loc_j + 1;
		std::cout << "loc_i después de avanzar loc_j + 1: " << loc_i << std::endl;
		url_i = url_j + 1;
		std::cout << "url_i después de avanzar url_j + 1: " << url_i << std::endl;
		std::cout << "---------------------------------------------------" << std::endl;
		sleep(5);
	}
	return (0);
}
*/

int	cmpDirectories(std::string &dir1, std::string &dir2)
{
	std::vector<std::string>	subDirs1;
	std::vector<std::string>	subDirs2;
	
	//tokenize dirs
	{
		//dir1
		{
			std::stringstream	dir1Stream(dir1);
			std::string			subDir1;
			std::cout << "SUBDIRS1" << std::endl;
			while (getline(dir1Stream, subDir1, '/'))
			{
				std::cout << subDir1 << std::endl;
				subDirs1.push_back(subDir1);
			}
		}
		//dir2
		{
			std::stringstream	dir2Stream(dir2);
			std::string			subDir2;
			std::cout << "SUBDIRS2" << std::endl;
	 		while (getline(dir2Stream, subDir2, '/'))
			{
				std::cout << subDir2 << std::endl;
				subDirs2.push_back(subDir2);
			}
		}
	}
	//cmp dirs
	int	len = std::min(subDirs1.size(), subDirs2.size());
	for (int i = 0; i < len; i++)
	{
		int dif = strcmp(subDirs1[i].c_str(), subDirs2[i].c_str());
		if (dif != 0)
			return (dif);
	}
	return (0);
}

bTreeNode	*findLocation(bTreeNode *server, std::string &URL)
{
	bTreeNode	*loc = NULL;

	std::cout << "URL a comparar: " << URL << std::endl;
	for (int i = 0; i < server->childs.size(); i++)
	{
		loc = server->childs[i];
		std::cout << "Estoy en contexto location: " << loc->contextArgs[0] << std::endl;
		std::cout << "Len de location: " <<  loc->contextArgs[0].length() << std::endl;
		//if (!match_loc_url(loc->contextArgs[0].c_str(), URL.c_str()))
		//if (URL.compare(0, loc->contextArgs[0].length(), loc->contextArgs[0]) == 0)
		if (!cmpDirectories(loc->contextArgs[0], URL))
		{
			std::cout << "Hace match de la URL: " << loc->contextArgs[0] << " y " << URL << std::endl;
			return (loc);
		}
	}
	std::cout << "No hizo ningún match" << std::endl;
	return (NULL);
}

/*bTreeNode	*findLocation(bTreeNode *server, std::string &URL)
{
	bTreeNode	*loc = NULL;

	std::cout << "URL a comparar: " << URL << std::endl;
	for (int i = 0; i < server->childs.size(); i++)
	{
		loc = server->childs[i];
		std::cout << "Estoy en contexto location: " << loc->contextArgs[0] << std::endl;
		std::cout << "Len de location: " <<  loc->contextArgs[0].length() << std::endl;
		if (URL.compare(0, loc->contextArgs[0].length(), loc->contextArgs[0]) == 0)
		{
			std::cout << "Hace match de la URL: " << loc->contextArgs[0] << " y " << URL << std::endl;
			return (loc);
		}
	}
	std::cout << "No hizo ningún match" << std::endl;
	return (NULL);
}*/

bool	findFile(std::string &dirFind, std::string &file)
{
	DIR *dir = opendir(dirFind.c_str());
	dirent	*dir_elem = readdir(dir);
	bool	found = false;

	while (dir_elem)
	{
		printf("Elemento de tipo: %d, y nombre: %s\n", dir_elem->d_type, dir_elem->d_name);
		//std::cout << "Elemento de tipo: " << dir_elem->d_type << " y nombre: " << dir_elem->d_name << std::endl;
		if (dir_elem->d_type == 8 && dir_elem->d_name == file)
		{
			std::cout << "Coincide el fichero: " << dir_elem->d_name << std::endl;
			found = true;
			break ;
		}
		dir_elem = readdir(dir);
	}
	closedir(dir);
	return (found);
}

/*bool	SuccesiveChar(const char *str, char c)
{
	for (int i = 1; i < strlen(str); i++)
	{
		if (str[i] == str[i - 1])
			return (true);
	}
	return (false);
}

size_t	countCharinStr(const char *str, char c)
{
	int	count = 0;
	
	for (int i = 0; i < strlen(str); i++)
		if	(str[i] == c)
			count++;
	return (count);


bool	validLocationModifiers(std::string &modifier)
{
	const char	*modifiers[] = {"=", "~", "~*", "^~"}; //= | ~ | ~* | ^~ 
	for (int i = 0; i < 4; i++)
		if (modifiers[i] == modifier)
			return (true);
	return (false);
}

typedef	struct	Location{
	size_t						uriType;
	int							modifier;
	std::vector<std::string>	uriTokens;
	std::vector<std::pair<std::string, std::vector<std::string> > >	directives; 	
}	Location;


int	cmpLocations(void *loc, void *cmp) //loc es la location a comparar, cmp es el url
{
	context	*loc_cast = (context *)loc;
	std::string	*cmp_cast = (std::string *)cmp;
	std::string	s1;
	std::string	s2;
	std::cout << "CMP1: " << loc_cast->_args[0] << ", CMP2: " << *cmp_cast << std::endl;

	if (loc_cast->_args[0] == "/")
	{
		std::cout << "Location es directorio raíz" << std::endl;
		return (1);
	}		
	for (int start1 = 1, start2 = 1, i = 1, j = 1; start1 < loc_cast->_args[0].length() && start2 < (*cmp_cast).length(); start1++, start2++)
	{
		//std::cout << "Loc: " << loc_cast->_args[0][i] << std::endl;
		//std::cout << "Cmp: " << (*cmp_cast)[i] << std::endl;
		for ( ; loc_cast->_args[0][start1] && loc_cast->_args[0][start1] != '/'; start1++);
		for ( ; (*cmp_cast)[start2] && (*cmp_cast)[start2] != '/'; start2++);
		s1 = loc_cast->_args[0].substr(i, start1 - i);
		s2 = (*cmp_cast).substr(j, start2 - j);
		std::cout << "S1: " << s1 << ", S2: " << s2 << std::endl;
		if (s1 != s2)
		{
			std::cout << "No coinciden" << std::endl;
			return (0);
		}
		i = start1 + 1;
		j = start2 + 1;
	}
	//if (!loc_cast->_name.compare(loc_cast->_name.length(), 0, *cmp_cast))
	//	return (true);
	return (1);
}

void	parseURI(Location &loc, std::string &URI)
{
	std::stringstream			str(URI);
	std::string					token;
	int							slash;

	if (SuccesiveChar(URI.c_str(), '/'))
	{
		std::cout << "Error: Slashes sucesivos" << std::endl;
		return ;
	}
	slash  = countCharinStr(URI.c_str(), '/');
	std::cout << "Número de slashes: " << slash << std::endl;
	if (URI[URI.length() - 1] == '/')
	{
		loc.uriType = 0;
		std::cout << "Es un directorio" << std::endl;
	}
	else
	{
		loc.uriType = 1;
		std::cout << "Es un fichero" << std::endl;
	}
	while (getline(str, token, '/'))
	{
		if (!token.empty())
		{
			std::cout << "Token: " << token << std::endl;
			loc.uriTokens.push_back(token);
		}		
	}
}

int	main(void)
{
	std::string	dir = "/Users/pfuentes/ejercicios/CPP/webserv_git";
	std::string	file = "webserv";
	if (findFile(dir, file))
		std::cout << "Encontró el fichero!" << std::endl;
	else
		std::cout << "No lo encontró..." << std::endl;
	std::string	URI = "/kapouet/pouic/toto/pouet";
	std::string	URI4 = "/kapouet/pouic/manuel/pouet";
	//std::string	URI = "/";
	std::string	URI2 = "/pepe/manuel/";
	std::string	URI3 = "/";
	context	c;
	c._args.push_back(URI);
	cmpLocations(&c, &URI4);
	return (0);
}*/

/*int	main(void)
{
	std::string	loc = "/post/";
	std::string	find = "/post.html";
	
	if (!cmpDirectories(loc, find))
		std::cout << "Son iguales" << std::endl;
	else
		std::cout << "Son diferentes" << std::endl;
}*/