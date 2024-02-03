#include "../../includes/webserv.hpp"

void	findNode(parseTree *root, parseTree **find_node, std::string find)
{
	if (root->context._name == find)
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

int	cmp_str_ranges(const char *s1, size_t n1, const char *s2, size_t n2)
{
	if (!s1 || !s2)
		return (-1);
	if (n1 != n2)
		return (-2);
	int	dif;
	for (size_t i = 0 ; i < n1; i++)
	{
		dif = s1[i] - s2[i];
		if (dif != 0)
			return (dif);
	}
	return (0);
}

std::vector<int_tuple>	setRangesDelRev(std::string &s, char del, int pos)
{
	int_tuple				ranges;
	size_t					find;
	std::vector<int_tuple>	rangesVec;

	if (s.size() <= 1)
		return (rangesVec);
	while (pos > 0)
	{
		ranges.e = pos;
		find = s.rfind(del, pos);
		if (find == std::string::npos)
			break ;
		ranges.i = find + 1;
		rangesVec.insert(rangesVec.begin(), ranges);
		pos = find - 1;
		for ( ; pos > 0 && s[pos] == del; pos--);
	}
	return (rangesVec);
}

int	cmpLocUri(std::string &loc, std::string &url)//, std::string &url
{
	//LOC RANGES
	int	i = loc.size() - 1;
	if (loc[i] == '/')
		for (; loc[i] != '/' && i >= 0; i--);
	std::vector<int_tuple>	rangesLoc = setRangesDelRev(loc, '/', i);
	if (!rangesLoc.size())
		return (0);
	//URL RANGES
	i = url.size() - 1;
	for (; url[i] != '/' && i >= 0; i--);
	std::vector<int_tuple>	rangesURI = setRangesDelRev(url, '/', i);
	if (!rangesURI.size())
		return (1);

	for (size_t i = 0; i < rangesLoc.size(); i++)
	{
		int	dif = (rangesLoc[i].e - rangesLoc[i].i) - (rangesURI[i].e - rangesURI[i].i);
		if (dif != 0)
			return (1);
		dif = cmp_str_ranges(&(loc.c_str()[rangesLoc[i].i]), rangesLoc[i].e - rangesLoc[i].i + 1,
				&(url.c_str()[rangesURI[i].i]), rangesURI[i].e - rangesURI[i].i + 1);
		if (dif != 0)
			return (1);
	}
	return (0);
}

parseTree	*matchLocation(struct client *client)
{
	parseTree	*loc = NULL;
	parseTree	*server = client->server;

	for (size_t i = 0; i < server->childs.size(); i++)
	{
		loc = server->childs[i];
		if (!cmpLocUri(loc->context._args[0], client->request.url))
			return (loc);
	}
	return (NULL);
}