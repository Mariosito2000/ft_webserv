#include "../includes/webserv.hpp"

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

std::vector<int_tuple>	del_ranges_path(std::string &s, char del)
{
	std::vector<int_tuple>	ranges;
	int_tuple	range;
	size_t	i = 0;
	//size_t	len = s.size();
	size_t	pos = s.size();
	while (pos > 0)
	{
		range.e = pos;
		pos = s.rfind(del, pos - 1);
		if (pos < 0)
			break ;
		//range.i = skip_char(s, i, del);
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
		//return (0);
	}
	std::cout << "Ranges de loc: " << std::endl;
	for (size_t i = 0; i < rangesLoc.size(); i++)
	{
		std::cout << "Rango " << i << std::endl;
		std::cout << "Inicio: " << rangesLoc[i].i << " | Final: " << rangesLoc[i].e << std::endl;
		for (int j = rangesLoc[i].i; j <= rangesLoc[i].e; j++)
			std::cout << loc[j];
		std::cout << std::endl;
	}
	std::vector<int_tuple>	rangesURL = del_ranges(url, '/');
	if (rangesURL.empty())
	{
		std::cout << "rangesURL esta vacío, no hay rangos separables" << std::endl;
		//return (0);
	}
	std::cout << "rangesURL: " << std::endl;
	for (size_t i = 0; i < rangesURL.size(); i++)
	{
		std::cout << "Rango " << i << std::endl;
		std::cout << "Inicio: " << rangesURL[i].i << " | Final: " << rangesURL[i].e << std::endl;
		for (int j = rangesURL[i].i; j <= rangesURL[i].e; j++)
			std::cout << url[j];
		std::cout << std::endl;
	}
	int dif;
	//size_t	range = std::max(rangesLoc.size(), rangesURL.size());
	std::cout << "---------------ENTRO A COMPARAR RANGOS------------" << std::endl;
	for (size_t i = 0; i < rangesLoc.size(); i++)
	{
		std::cout << "Comparación de rango: " << i << std::endl;
		std::cout << "Comparar loc: ";
		for (int j = rangesLoc[i].i; j <= rangesLoc[i].e; j++)
			std::cout << loc[j];
		std::cout << " y url: ";
		for (int j = rangesURL[i].i; j <= rangesURL[i].e; j++)
			std::cout << url[j];
		std::cout << std::endl;
		dif = cmp_str_ranges(&loc[rangesLoc[i].i], rangesLoc[i].e - rangesLoc[i].i + 1,
				&url[rangesURL[i].i], rangesURL[i].e - rangesURL[i].i + 1);
		if (dif != 0)
		{
			std::cout << "No coinciden: " << dif << std::endl;
			return (dif);
		}	
	}
	std::cout << "Terminó, coinciden" << std::endl;
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

int	main(void)
{
	//std::string	loc("/pepe/juanito/manuel//e");
	std::string	loc("/buenas/eee/holas");
	//std::string	url("/buenas/eee/holas/adios/aaa");
	//std::string	url("/pepe/");
	std::string url("/");
	if (!cmpLocUri(loc, url))
		std::cout << "URL está contenida en location" << std::endl;
	else
		std::cout << "URL no está contenida en location, mal" << std::endl;
	return (1);
}
