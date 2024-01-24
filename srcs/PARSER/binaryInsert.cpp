#include "../../includes/webserv.hpp"

size_t	countCharinStr(const char *str, char c)
{
	int	count = 0;
	
	for (size_t i = 0; i < strlen(str); i++)
		if	(str[i] == c)
			count++;
	return (count);
}

/*int	cmpDirectories(std::string &s1, std::string &s2)
{
	int	slash1 = countCharinStr(s1.c_str(), '/');
	int	slash2 = countCharinStr(s2.c_str(), '/');
	std::cout << "Número de slashes en dir1: " << slash1 << std::endl;
	std::cout << "Número de slashes en dir2: " << slash2 << std::endl;
	if (slash1 > slash2)
	{
		std::cout << "Slashes no coinciden: slash1 es mayor que slash2" << std::endl;
		return (slash1 - slash2);
	}
	if (slash1 < slash2)
	{
		std::cout << "Slashes no coinciden: slash1 es menor que slash2" << std::endl;
		return (slash1 - slash2);
	}
	if (s2 == "/") //caso especial hacerlo bien solo para probar que este es el problema
		return (1);
	std::stringstream			streamDir1(s1);
	std::stringstream			streamDir2(s2);
	std::string					tokenDir1;
	std::string					tokenDir2;
	while (getline(streamDir1, tokenDir1, '/') && getline(streamDir2, tokenDir2, '/'))
	{
		std::cout << "Comparar: " << tokenDir1 << " y " << tokenDir2 << std::endl;
		if (tokenDir1 > tokenDir2)
		{
			std::cout << "No coinciden, token1 es mayor que token2" << std::endl;
			return (1);
		}
		else if (tokenDir1 < tokenDir2)
		{
			std::cout << "No coinciden, token1 es menor que token2" << std::endl;
			return (-1);
		}
	}
	std::cout << "Coinciden" << std::endl;
	return (0);
}*/

/*void    binaryInsertionDeque(std::deque<int> &deq, itdeque const &begin, itdeque const &end, int num)
{
    size_t  len = distance(begin, end);
    if (len == 0)
    {
		deq.insert(begin, num);
        return ;
    }
    size_t	find = len / 2;
    itdeque	itf = begin + find;
    if (num < *itf)
        binaryInsertionDeque(deq, begin, itf, num);
    else
        binaryInsertionDeque(deq, itf + 1, end, num);
}*/

int	cmpLocations(bTreeNode *loc, bTreeNode *cmp) //loc es la location a comparar, cmp es el url
{
	std::string dir = loc->contextArgs[0]; //cambiar esto a struct context
	std::string	cmp_cast = cmp->contextArgs[0];
	
	int	slash1 = countCharinStr((dir).c_str(), '/');
	int	slash2 = countCharinStr((cmp_cast).c_str(), '/');
	//std::cout << "Número de slashes en dir1: " << slash1 << std::endl;
	//std::cout << "Número de slashes en dir2: " << slash2 << std::endl;
	if (slash1 > slash2)
	{
		//std::cout << "Slashes no coinciden: slash1 es mayor que slash2" << std::endl;
		return (slash1 - slash2);
	}
	if (slash1 < slash2)
	{
		//std::cout << "Slashes no coinciden: slash1 es menor que slash2" << std::endl;
		return (slash1 - slash2);
	}

	std::string	s1;
	std::string	s2;
	//std::cout << "CMP1: " << dir << ", CMP2: " << cmp_cast << std::endl;

	if (dir == "/")
	{
		//std::cout << "Location es directorio raíz" << std::endl;
		return (1);
	}		
	for (size_t start1 = 1, start2 = 1, i = 1, j = 1; start1 < dir.length() && start2 < cmp_cast.length(); start1++, start2++)
	{
		//std::cout << "Loc: " << loc_cast->_args[0][i] << std::endl;
		//std::cout << "Cmp: " << (*cmp_cast)[i] << std::endl;
		for ( ; dir[start1] && dir[start1] != '/'; start1++);
		for ( ; cmp_cast[start2] && cmp_cast[start2] != '/'; start2++);
		s1 = dir.substr(i, start1 - i);
		s2 = cmp_cast.substr(j, start2 - j);
		//std::cout << "S1: " << s1 << ", S2: " << s2 << std::endl;
		if (s1 != s2)
		{
			//std::cout << "No coinciden" << std::endl;
			return (0);
		}
		i = start1 + 1;
		j = start2 + 1;
	}
	//if (!loc_cast->_name.compare(loc_cast->_name.length(), 0, *cmp_cast))
	//	return (true);
	return (1);
}

/*int	cmpDirectives(void *loc, void *cmp) //loc es la location a comparar, cmp es el url
{
	std::string *dir = (bTreeNode *)(std::string *); //cambiar esto a struct context
	std::string	*cmp_cast = (std::string *)cmp;
	
	int	slash1 = countCharinStr((*dir).c_str(), '/');
	int	slash2 = countCharinStr((*cmp_cast).c_str(), '/');
	std::cout << "Número de slashes en dir1: " << slash1 << std::endl;
	std::cout << "Número de slashes en dir2: " << slash2 << std::endl;
	if (slash1 > slash2)
	{
		std::cout << "Slashes no coinciden: slash1 es mayor que slash2" << std::endl;
		return (slash1 - slash2);
	}
	if (slash1 < slash2)
	{
		std::cout << "Slashes no coinciden: slash1 es menor que slash2" << std::endl;
		return (slash1 - slash2);
	}

	std::string	s1;
	std::string	s2;
	std::cout << "CMP1: " << *dir << ", CMP2: " << *cmp_cast << std::endl;

	if (*dir == "/")
	{
		std::cout << "Location es directorio raíz" << std::endl;
		return (1);
	}		
	for (int start1 = 1, start2 = 1, i = 1, j = 1; start1 < dir->length() && start2 < (*cmp_cast).length(); start1++, start2++)
	{
		//std::cout << "Loc: " << loc_cast->_args[0][i] << std::endl;
		//std::cout << "Cmp: " << (*cmp_cast)[i] << std::endl;
		for ( ; (*dir)[start1] && (*dir)[start1] != '/'; start1++);
		for ( ; (*cmp_cast)[start2] && (*cmp_cast)[start2] != '/'; start2++);
		s1 = (*dir).substr(i, start1 - i);
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
}*/

int	binarySearch(std::vector<bTreeNode*> &vec, bTreeNode *insert) //hacer un template de esto
//pasar funcion de comparar como parametro
{
	int	start = 0;
	int	end = vec.size() - 1;
	int	low = start;
	int	high = end;
	int	mid;

	//std::cout << "Entro en binarySearch" << std::endl;
	//std::cout << "Cmp: " << insert->contextArgs[0] << std::endl;
	while (low <= high)
	{
		mid = (high + low) / 2;
		//std::cout << "Low: " << vec[low]->contextArgs[0] << std::endl;
		//std::cout << "High: " << vec[high]->contextArgs[0] << std::endl;
		//std::cout << "Valor a comparar: " << vec[mid]->contextArgs[0] << " en pos: " << mid << std::endl;
		int	res = cmpLocations(vec[mid], insert);
		//std::cout << "Res es: " << res << std::endl;
		if (res == 0)
		{
			//std::cout << "Coinciden" << std::endl;
			return (mid);
		}
   		else if (res > 0)
		{
			//std::cout << "Cmp es mayor que mid, me muevo a lado derecho" << std::endl;
			low = mid + 1;
		}	
    	else
		{
			//std::cout << "Cmp es menor que mid, me muevo a lado izquierdo" << std::endl;
			high = mid - 1;
		}
	}
	if (low <= start) //iterador ha llegado a principio del array
	{
		//std::cout << "No encuentra elemento, pero debería estar al principio" << std::endl;
		return (-1);
	}
	else if (high >= end) //iterador ha llegado a final del array
	{
		//std::cout << "No encuentra elemento, pero debería estar al final" << std::endl;
		return (-2);
	}
	return (low);
}

/*int	main(void)
{
	std::string	dir = "/Users/pfuentes/ejercicios/CPP/webserver_git";
	std::string	file = "webserv";
	std::string	location1 = "/kapouet/bueno";
	std::string	location2 = "/kapouet";
	std::string	location3 = "/kapouet/pepe/pedro";
	std::string	location4 = "/kapouet/pepe";
	std::string	location5 = "/pepe/manuel/manolo/jose";
	std::string	location6 = "/";
	std::string	location7 = "/bueno";
	std::string	location8 = "/pepe/manuel";
	//std::string	URI = "/kapouet/pouic/toto/pouet";
	//std::string	URI = "/";
	//std::string	URI = "/pepe/manuel/";
	std::vector<std::string>	locations;
	locations.push_back(location1);
	locations.push_back(location2);
	locations.push_back(location3);
	locations.push_back(location4);
	locations.push_back(location5);
	locations.push_back(location6);
	locations.push_back(location7);
	locations.push_back(location8);
	std::vector<std::string>	vec;
	vec.push_back(location1);
	for (int i = 1; i < locations.size(); i++)
	{
		std::cout << "----------------------Debe guardar: " << locations[i] << "---------------------" << std::endl;
		int	res = binarySearch(vec, locations[i], &cmpDirectives);
		if (res == -1)
		{
			std::cout << "Lo añade al principio" << std::endl;
			vec.insert(vec.begin(), locations[i]);
		}
		else if (res == -2)
		{
			std::cout << "Lo añade al final" << std::endl;
			vec.push_back(locations[i]);
		}
		else
		{
			std::cout << "Guardar elemento en pos: " << res << std::endl;
			vec.insert(vec.begin() + res, locations[i]);
		}
		std::cout << "Vector que debería estar ordenado" << std::endl;
		for (int i = 0; i < vec.size(); i++)
			std::cout << vec[i] << std::endl;
	}
	//cmpDirectories(URI, location2);
	//cmpDirectories(URI, location1);
	std::cout << "----------------AL FINAL----------------" << std::endl;
	std::cout << "Vector que debería estar ordenado" << std::endl;
	for (int i = 0; i < vec.size(); i++)
		std::cout << vec[i] << std::endl;
	return (0);

}*/
