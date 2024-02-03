#include "../../includes/webserv.hpp"

bool	tokenizeFile(const char *file, std::vector<t_token> &tokens, std::string &del)
{
	std::fstream	readTokens(file);
	if (readTokens.fail())
		return (false);

	std::string	tokenStr;
	t_token	token;
	int	start;
	int	end;
	int	flag;
	int	brackets = 0;
	while (readTokens >> tokenStr)
	{	
		start = 0;
		end = 0;
		flag = 0;
		for (size_t i = 0; i < tokenStr.length(); i++)
		{
			for (size_t j = 0; j < del.length(); j++)
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
			if (tokens.size() > 1)
			{
				if (tokens[tokens.size() - 2].value == "{" && tokens[tokens.size() - 1].value == "}")
					return (false);
				if ((tokens[tokens.size() - 2].value == "{" || tokens[tokens.size() - 2].value == ";")
						&& (tokens[tokens.size() - 1].value == ";"))
					return (false);
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
	const char	*server[] = {"listen", "limit_body_size", "server_name", "timeout", "error_page", "root", "index", NULL};
	const char	*location[] = {"autoindex", "limit_body_size", "root", "methods", "alias", "error_files", "index", "redirect", 
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
bool	parseContextTokens(parseTree *root, std::vector<t_token> &tokens)
{
	std::list<parseTree *>	nodes;
	parseTree				*child;

	nodes.push_front(root);
	int	initDirective = 0;
	int	endDirective = 0;
	for (size_t i = 0;  i < tokens.size(); i++)
	{
		if (tokens[i].value == "{")
		{
			if (!validSubContextsCmp(root->context._name, tokens[initDirective].value))
				return (false);
			if (tokens[initDirective].value != "location" && i - initDirective != 1)
				return (false);
			if (tokens[initDirective].value == "location" && i - initDirective != 2)
				return (false);				
			child = new parseTree();
			child->context._name = tokens[initDirective].value;
			for (size_t start = initDirective + 1; start < i; start++)
				child->context._args.push_back(tokens[start].value);
			if (root->context._name == "location")
				linearInsertLoc(root->childs, child);
			else
				root->childs.push_back(child);
			nodes.push_front(child);
			root = child;
			initDirective = i + 1;
		}
		else if (tokens[i].value == "}")
		{
			if (tokens[i - 1].value != ";" && tokens[i - 1].value != "}")
				return (false);
			nodes.pop_front();
			root = nodes.front();
			initDirective = i + 1;
		}
		else if (tokens[i].value == ";")
		{
			endDirective = i;
			if (i - initDirective == 1)
				return (false);
			if (!validDirectivesCmp(root->context._name, tokens[initDirective].value))
				return (false);
			for (int j = initDirective + 1; j < endDirective; j++)
				root->context._dirs.insert(std::pair<std::string, std::string>(tokens[initDirective].value, tokens[j].value));
			initDirective = i + 1;
		}
	}
	return (true);
}
void	freeParseTree(parseTree *root)
{
	for (size_t i = 0; i < root->childs.size(); i++)
		freeParseTree(root->childs[i]);
	delete(root);
	root = NULL;
}

parseTree	*parseFile(char	*file)
{
	std::string	del = "{};=";
	std::vector<t_token>	tokens;
	if (!tokenizeFile(file, tokens, del))
		return (NULL);
	parseTree	*root = new parseTree();
	root->context._name = "main";
	if (!parseContextTokens(root, tokens))
	{
		freeParseTree(root);
		return (NULL);
	}
	return (root);
}