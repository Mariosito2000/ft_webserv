#include "../../includes/webserv.hpp"

bool	tokenizeFile(const char *file, std::vector<t_token> &tokens, std::string &del) //comprobar también aquí que lo anterior a un delimitador es un str
{
	std::fstream	readTokens(file);
	if (readTokens.fail())
	{
		std::cout << "Invalid file!" << std::endl;
		return (false);
	}
	std::string	tokenStr;
	t_token	token;
	int	start;
	int	end;
	int	flag;
	int	brackets = 0;
	while (readTokens >> tokenStr) //!readTokens.eof()
	{	
		start = 0;
		end = 0;
		flag = 0;
		for (size_t i = 0; i < tokenStr.length(); i++)
		{
			for (size_t j = 0; j < del.length(); j++) //quiza hacer una funcion aparte que busque un char en un str, como un "alfabeto"
			{
				if (tokenStr[i] == del[j])
				{
					if (del[j] == '{')
						brackets++;
					else if (del[j] == '}')
						brackets--;
					if (brackets < 0)
						return (false);
					end = i;
					//si lo anterior no es un símbolo especial, que ya está añadido, sino una palabra "normal"
					if (start != end)
					{
						token.value = tokenStr.substr(start, end - start);
						token.type = 0;
						tokens.push_back(token);
					}
					token.value = del[j];
					token.type = j + 1;
					tokens.push_back(token);
					end++;
					start = end;
					flag = 1;
					break ;
				}
			}
		}
		if (flag == 0)
		{
			token.value = tokenStr;
			tokens.push_back(token);
		}
	}
	if (brackets != 0)
		return (false);
	return (true);
}

size_t	matrixLenChar(char const **matrix)
{
	size_t i;

	for (i = 0; matrix[i] != NULL; i++);
	return (i);
}

bool	validSubContextsCmp(std::string &context, std::string &subcontext)
{
	const char	*main[] = {"events", "http", NULL};
	const char	*http[] = {"types", "server", NULL};
	const char	*server[] = {"location", NULL};
	
	const char	**find = NULL;
	if (context == "main")
		find = main;
	else if (context == "http")
		find = http;
	else if (context == "server")
		find = server;
	if (!find)
		return (false);
	for (int i = 0; find[i]; i++)
	{
		if (find[i] == subcontext)
			return (true);
	}
	return (false);
}

bool	validDirectivesCmp(std::string	&context, std::string &directive)
{
	const char	*main[] = {"workers", NULL};
	const char	*events[] = {"prueba", NULL};
	const char	*http[] = {"hola", NULL};
	const char	*types[] = {"text/html", "text/css", "text/xml"};
	const char	*server[] = {"listen", "limit_body_size", "server_name", "error_page", "root", "index", NULL};
	const char	*location[] = {"autoindex", "root", "methods", "alias", "error_files", "index", "redirect", 
								"try_files", "postdir", "cgi_pass", NULL};

	const char	**find = NULL;
	if (context == "main")
		find = main;
	else if (context == "events")
		find = events;
	else if (context == "http")
		find = http;
	else if (context == "types")
		find = types;
	else if (context == "server")
		find = server;
	else if (context == "location")
		find = location;
	if (!find)
		return (false);
	for (int i = 0; find[i]; i++)
	{
		if (find[i] == directive)
			return (true);
	}
	return (false);
}
bool	strInVector(std::string &str, std::vector<std::string> &vector) //usar para comparar tanto directivas como contextos
//hacer que en vez de devolver un bool devuelva un puntero, si no está es NULL, si está puedo acceder a él directamente
{
	std::cout << "Buscar si str: " << str << " está en vector: " << std::endl; 
	for (size_t i = 0; i < vector.size(); i++)
	{
		std::cout << "A comparar: " << vector[i] << std::endl;
		if (str == vector[i])
			return (true);
	}
	return (false);
}

bTreeNode	*copyBTreeNode(bTreeNode *node)
{
	bTreeNode	*copy = new bTreeNode();
	copy->contextName = node->contextName;
	return (copy);
}

void	binaryInsert(std::vector<bTreeNode *> &vec, bTreeNode *insert) //hacer mejor un template de esto
{
	std::cout << "Entro en binary insert" << std::endl;
	if (vec.empty())
	{
		vec.push_back(insert);
		return ;
	}
	int	res = binarySearch(vec, insert);
	std::cout << "Resultado de binarySearch: " << res << std::endl;
	//hacer un switch
	if (res == -1)
	{
		std::cout << "Lo añade al principio" << std::endl;
		vec.insert(vec.begin(), insert);
	}
	else if (res == -2)
	{
		std::cout << "Lo añade al final o vector está vacío" << std::endl;
		vec.push_back(insert);
	}
	else
	{
		std::cout << "Guardar elemento en pos: " << res << std::endl;
		vec.insert(vec.begin() + res, insert);
	}
}

bool	parseContextTokens(bTreeNode *root, std::vector<t_token> &tokens)
{
	std::list<bTreeNode *>	nodes; //stack de nodos - se añade cuando entra a un contexto, se borra cuando termina, vuelve al anterior, evitar recursividad
	bTreeNode	*child; //hijo del nodo del árbol a crear
	//context		*subcontext; // subcontexto a crear y añadir al arbol

	nodes.push_front(root);
	int	initDirective = 0;
	int	endDirective = 0;
	for (size_t i = 0;  i < tokens.size(); i++)
	{
		if (tokens[i].value == "{") //tiene que abrir contexto y saltar al siguiente
		{
			if (!validSubContextsCmp(root->contextName, tokens[initDirective].value))
				return (false);
			child = new bTreeNode();
			child->contextName = tokens[initDirective].value;
			for (size_t start = initDirective + 1; start < i; start++) //añade argumentos del contexto si los hay
				child->contextArgs.push_back(tokens[start].value);
			if (child->contextName == "location")
				binaryInsert(root->childs, child);
			else
				root->childs.push_back(child);
			root->childsNames.push_back(child->contextName);
			nodes.push_front(child);
			root = child;
			initDirective = i + 1;
		}
		else if (tokens[i].value == "}") //tiene que cerrar contexto y volver al anterior
		{
			nodes.pop_front();
			root = nodes.front();
			initDirective = i + 1;
		}
		else if (tokens[i].value == ";") // final de directiva, añadir a lista
		{
			endDirective = i;
			if (!validDirectivesCmp(root->contextName, tokens[initDirective].value))
			{
				std::cout << "Directiva no está entre las posbles" << std::endl;
				return (false);
			}
			std::pair<std::string, std::vector<std::string> >	keyVal;
			keyVal.first = tokens[initDirective].value;
			for (int j = initDirective + 1; j < endDirective; j++) //añade values
				keyVal.second.push_back(tokens[j].value);
			root->directives.push_back(keyVal); //añade key-values
			for (int j = initDirective + 1; j < endDirective; j++)
				root->directivesMap.insert(std::pair<std::string, std::string>(tokens[initDirective].value, tokens[j].value));
			initDirective = i + 1;
		}
	}
	return (true);
}
void	printBTree(bTreeNode *root)
{
	std::cout << "Context: " << root->contextName << std::endl;
	if (root->contextArgs.size() > 0)
	{
		std::cout << "Argumentos del contexto: {";
		for (size_t i = 0; i < root->contextArgs.size(); i++)
		{
			std::cout << root->contextArgs[i];
			if (i < root->contextArgs.size() - 1)
				std::cout << ", ";
		}
		std::cout << "}" << std::endl;
	}
	/*std::cout << "Directivas: " << std::endl;
	for (int i = 0; i < root->directives.size(); i++)
	{
		std::cout << "Key: " << root->directives[i].first << ": ";
		std::cout << "Values: {";
		for (int j = 0; j < root->directives[i].second.size(); j++)
		{
			std::cout << root->directives[i].second[j];
			if (j < root->directives[i].second.size() - 1)
				std::cout << ", ";
		}
		std::cout << "}" << std::endl;
	}*/
	typedef std::multimap<std::string, std::string>::iterator	itm;
	for (itm ib = root->directivesMap.begin(); ib != root->directivesMap.end(); ib++)
	{
		std::cout << "Key: " << ib->first << ": ";
		std::cout << "Value: " << ib->second << std::endl;
	}
	for (size_t i = 0; i < root->childs.size(); i++)
	{
		std::cout << "Hijo " << i << " de: " << root->contextName << std::endl;
		printBTree(root->childs[i]);
	}
}

bTreeNode	*parseFile(char	*file)
{
	std::string	del = "{};=";
	std::vector<t_token>	tokens;
	if (!tokenizeFile(file, tokens, del))
	{
		std::cout << "Error al tokenizar" << std::endl;
		return (NULL);
	}
	for (size_t i = 0; i < tokens.size(); i++)
		std::cout << "Token: " << tokens[i].value << std::endl;
	bTreeNode	*root = new bTreeNode();
	root->contextName = "main";
	if (!parseContextTokens(root, tokens))
	{
		//funcion de liberar el arbol
		return (NULL);
	}
	std::cout << "---------------Imprimir árbol de fichero de configuración---------------" << std::endl;
	printBTree(root);
	return (root);
}

/*int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Error: bad number of arguments" << std::endl;
		return (1);
	}
	if (!parseFile(argv[1]))
	{
		std::cout << "Hubo un error al parsear el fichero" << std::endl;
		return (1);
	}
	return (0);
}*/