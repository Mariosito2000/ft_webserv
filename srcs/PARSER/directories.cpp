#include "../../includes/webserv.hpp"



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
		for (size_t i = 0; i < root->childs.size(); i++)
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

size_t	skip_char(std::string &s, size_t pos, char c)
{
	for (; pos < s.size() && s[pos] == c; pos++);
	if (s[pos] == '\0')
		return (-1);
	return (pos);
}

size_t	move_to_char(std::string &s, size_t pos, char c)
{
	for (; pos < s.size() && s[pos] != c; pos++);
	if (s[pos] == '\0')
		return (-1);
	return (pos);
}

typedef struct	int_tuple{
	int	i;
	int	e;
} int_tuple;

std::vector<int_tuple>	del_ranges(std::string &s, char del)
{
	std::vector<int_tuple>	ranges;
	int_tuple	range;
	size_t	i = 0;
	size_t	len = s.size();
	while (i < len)
	{
		range.i = skip_char(s, i, del);
		if (range.i < 0)
		{
			std::cout << "No hay siguiente valor de inicio para del" << std::endl;
			break ;
		}
		range.e = move_to_char(s, range.i, del) - 1;
		if (range.e < 0)
		{
			std::cout << "No hay siguiente valor de final para del" << std::endl;
			break ;
		}
		ranges.push_back(range);
		i = range.e + 1;
	}
	return (ranges);
}

int	cmp_str_ranges(std::string &s1, size_t pos1, size_t n1, std::string &s2, size_t pos2, size_t n2)
//probar a pasar un char* ya en la pos correcta, en vez del string entero desde el comienzo con la pos
{
	if (pos1 + n1 >= s1.size())
		return (-2);
	if (pos2 + n2 >= s2.size())
		return (-3);
	if ((n1 - pos1) != (n2 - pos2))
		return (-4);
	int	dif;
	for ( ; pos1 < n1; pos1++)
	{
		dif = s1[pos1] - s2[pos1];
		if (dif != 0)
			return (dif);
	}
	return (0);
}

int	cmp_str_ranges(const char *s1, size_t n1, const char *s2, size_t n2)
{
	if (!s1 || !s2)
		return (-1);
	if (n1 != n2)
		return (-2);
	int	dif;
	for (size_t i = 0 ; i < n1; i++)
	{
		//std::cout << "Comparar: " << s1[i] << " y " << s2[i] << std::endl;
		dif = s1[i] - s2[i];
		if (dif != 0)
			return (dif);
	}
	return (0);
}

int	cmpPaths(std::string &dir1, std::string &dir2)
{
	std::vector<int_tuple>	ranges = del_ranges(dir1, '/');
	if (ranges.empty())
	{
		std::cout << "Ranges1 esta vacío, no hay rangos separables" << std::endl;
	}
	std::cout << "Ranges1: " << std::endl;
	for (size_t i = 0; i < ranges.size(); i++)
	{
		std::cout << "Rango " << i << std::endl;
		std::cout << "Inicio: " << ranges[i].i << " | Final: " << ranges[i].e << std::endl;
	}
	std::vector<int_tuple>	ranges2 = del_ranges(dir2, '/');
	if (ranges2.empty())
	{
		std::cout << "Ranges2 esta vacío, no hay rangos separables" << std::endl;
	}
	std::cout << "Ranges2: " << std::endl;
	for (size_t i = 0; i < ranges.size(); i++)
	{
		std::cout << "Rango " << i << std::endl;
		std::cout << "Inicio: " << ranges[i].i << " | Final: " << ranges[i].e << std::endl;
	}
	int dif = ranges.size() != ranges2.size();
	if (dif != 0)
		return (dif);
	for (size_t i = 0; i < ranges.size(); i++)
	{
		dif = cmp_str_ranges(dir1, ranges[i].i, ranges[i].e - ranges[i].i + 1,
				dir2, ranges2[i].i, ranges2[i].e - ranges2[i].i + 1);
		if (dif != 0)
			return (dif);
	}
	return (0);
}

/*int	cmpLocUrl(std::string &loc, std::string &url)
{
	std::vector<int_tuple>	rangesLoc = del_ranges(loc, '/');
	if (rangesLoc.empty())
	{
		std::cout << "Ranges de loc esta vacío, no hay rangos separables" << std::endl;
	}
	std::cout << "Ranges de loc: " << std::endl;
	for (size_t i = 0; i < rangesLoc.size(); i++)
	{
		std::cout << "Rango " << i << std::endl;
		std::cout << "Inicio: " << rangesLoc[i].i << " | Final: " << rangesLoc[i].e << std::endl;
	}
	std::vector<int_tuple>	rangesURL = del_ranges(url, '/');
	if (rangesURL.empty())
	{
		std::cout << "rangesURL esta vacío, no hay rangos separables" << std::endl;
	}
	std::cout << "rangesURL: " << std::endl;
	for (size_t i = 0; i < rangesURL.size(); i++)
	{
		std::cout << "Rango " << i << std::endl;
		std::cout << "Inicio: " << rangesURL[i].i << " | Final: " << rangesURL[i].e << std::endl;
	}
	if (rangesURL.size() == 0)
	{
		if (rangesLoc.size() == 0)
			return (0);
		return (-1);
	}
	int dif;
	for (size_t i = 0; i < rangesLoc.size(); i++)
	{
		dif = cmp_str_ranges(loc, rangesLoc[i].i, rangesLoc[i].e - rangesLoc[i].i + 1,
				url, rangesURL[i].i, rangesURL[i].e - rangesURL[i].i + 1);
		if (dif != 0)
			return (dif);
	}
	return (0);
}*/

std::vector<int_tuple>	setRangesDelRev(std::string &s, char del, int pos)
{
	int_tuple				ranges;
	size_t					find;
	std::vector<int_tuple>	rangesVec;

	if (s.size() == 1)
		return (rangesVec);
	for (; pos >= 0; )
	{
		std::cout << "Pos de final: " << pos << std::endl;
		//sleep(2);
		ranges.e = pos;
		find = s.rfind(del, pos);
		std::cout << "Find: " << find << std::endl;
		if (find == std::string::npos)
		{
			std::cout << "Deja de encontrar del" << std::endl;
			break ;
		}
		ranges.i = find + 1;
		rangesVec.insert(rangesVec.begin(), ranges);
		pos = find - 1;
		std::cout << "Pos tras find - 1: " << pos << std::endl;
		if (pos < 0)
			break ;
		for (; s[pos] == del && pos >= 0; pos--);
	}
	for (size_t i = 0; i < rangesVec.size(); i++)
	{
		std::cout << "RANGE: START: " << rangesVec[i].i << " | END: " << rangesVec[i].e << std::endl;
		for (int j = rangesVec[i].i; j <= rangesVec[i].e; j++)
			std::cout << s[j];
		std::cout << std::endl;
		//sleep(3);
	}
	return (rangesVec);
}

int	cmpLocUri(std::string &loc, std::string &url)//, std::string &url
{
	int	i = loc.size() - 1;
	std::vector<int_tuple>	rangesLoc = setRangesDelRev(loc, '/', i);
	if (!rangesLoc.size())
		return (0);
	i = url.size() - 1;
	for (; url[i] != '/' && i >= 0; i--);
	std::vector<int_tuple>	rangesURI = setRangesDelRev(url, '/', i);
	std::cout << "Size de rangesLoc: " << rangesLoc.size() << std::endl;
	if (!rangesURI.size() && rangesLoc.size())
		return (1);
	if (!rangesURI.size() && !rangesLoc.size())
		return (0);
	for (size_t i = 0; i < rangesLoc.size(); i++)
	{
		std::cout << "Comparar LOC: ";
		for (int j = rangesLoc[i].i; j <= rangesLoc[i].e; j++)
			std::cout << loc[j];
		std::cout << " y URI: ";
		for (int j = rangesURI[i].i; j <= rangesURI[i].e; j++)
			std::cout << url[j];
		std::cout << std::endl;
		int	dif = (rangesLoc[i].e - rangesLoc[i].i) - (rangesURI[i].e - rangesURI[i].i);
		if (dif != 0)
		{
			std::cout << "Distinta longitud" << std::endl;
			return (1);
		}
		dif = cmp_str_ranges(&(loc.c_str()[rangesLoc[i].i]), rangesLoc[i].e - rangesLoc[i].i + 1,
				&(url.c_str()[rangesURI[i].i]), rangesURI[i].e - rangesURI[i].i + 1);
		std::cout << "Retorno de cmp: " << dif << std::endl;
		if (dif != 0)
		{
			std::cout << "No coinciden" << std::endl;
			return (1);
		}
	}
	return (0);
}

bTreeNode	*findLocation(struct client *client)
{
	bTreeNode	*loc = NULL;
	bTreeNode	*server = client->server;
	std::cout << "URL a comparar: " << client->request.url << std::endl;
	for (size_t i = 0; i < server->childs.size(); i++)
	{
		loc = server->childs[i];
		std::cout << "Estoy en contexto location: " << loc->contextArgs[0] << std::endl;
		std::cout << "Len de location: " <<  loc->contextArgs[0].length() << std::endl;
		//if (!match_loc_url(loc->contextArgs[0].c_str(), URL.c_str()))
		//if (URL.compare(0, loc->contextArgs[0].length(), loc->contextArgs[0]) == 0)
		//if (!cmpDirectories(loc->contextArgs[0], client->request.url))
		if (!cmpLocUri(loc->contextArgs[0], client->request.url))
		{
			std::cout << "Hace match de la URL: " << loc->contextArgs[0] << " y " << client->request.url << std::endl;
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