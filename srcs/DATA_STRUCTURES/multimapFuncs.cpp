#include "../../includes/webserv.hpp"

std::string *getMultiMapValue(std::multimap<std::string, std::string> &map, std::string key)
{
	std::multimap<std::string, std::string>::iterator	it;
	it = map.find(key);
	if (it != map.end())
		return (&it->second);
	return (NULL);
}

bool	isInMultiMapKey(std::multimap<std::string, std::string> &map, std::string key)
{
	typedef std::map<std::string, std::string>::iterator	itmap;
	itmap it = map.find(key);
	if (it == map.end())
		return (false);
	return (true);
}

bool	isInMultiMapValue(std::multimap<std::string, std::string> &map, std::string key, std::string value)
{
	typedef std::map<std::string, std::string>::iterator	itmap;
	itmap it = map.find(key);
	if (it == map.end())
		return (false);
	typedef std::pair<itmap, itmap> itr;
	itr iti = map.equal_range(key);
	for (itmap itb = iti.first, ite = iti.second; itb != ite; itb++)
	{
		if (itb->second == value)
			return (true);
	}
	return (false);
}

bool isInMultiMapValues(std::multimap<std::string, std::string> &map, std::string key, std::string values[], size_t len)
{
	typedef std::map<std::string, std::string>::iterator	itmap;
	itmap it = map.find(key);
	if (it == map.end())
		return (false);
	typedef std::pair<itmap, itmap> itr;
	itr iti = map.equal_range(key);
	for (itmap itb = iti.first, ite = iti.second; itb != ite; itb++)
	{
		for (size_t i = 0; i < len; i++)
		{
			if (itb->second == values[i])
				return (true);
		}
	}
	return (false);
}

std::string	getMultiMapValueKeys(std::multimap<std::string, std::string> &map, std::string keys[], size_t len)
{
	std::string *value;
	for (size_t i = 0; i < len; i++)
	{
		value = getMultiMapValue(map, keys[i]);
		if (value)
			return (*value);
	}
	throw (1);
}

bool	multiMapCheckValidValue(std::multimap<std::string, std::string>	&map, std::string key, bool (*f)(std::string &value))
{
	itmap	itm = map.find(key);
	if (itm != map.end())
	{
		itr	it = map.equal_range(key);
		for (itmap itb = it.first, ite = it.second; itb != ite; itb++)
		{
			if (!f(itb->second))
				return (false);
		}
	}
	return (true);
}

