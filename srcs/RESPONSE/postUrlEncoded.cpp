#include "../../includes/webserv.hpp"

void	postUrlEncoded(std::string &route, const char *body, size_t size)
{
	std::map<std::string, std::string > map;
	std::string	filename;
	std::string str;

	for (size_t i = 0; i < size; i++)
	{
		if (body[i] == '&')
		{
			if (!str.empty())
			{
				size_t equal = str.find('=');
				if (equal == std::string::npos || equal == 0 || equal == str.size() -1)
					throw (400);
				map.insert(std::pair<std::string, std::string> (str.substr(0, equal), str.substr(equal + 1)));
				str.clear();
			}
			i++;
		}
		if (i < size)
			str += body[i];
	}
	if (!str.empty())
	{
		size_t equal = str.find('=');
		if (equal == std::string::npos || equal == 0 || equal == str.size() -1)
			throw (400);
		map.insert(std::pair<std::string, std::string> (str.substr(0, equal), str.substr(equal + 1)));
		str.clear();
	}
	for (std::map<std::string, std::string>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		if (iter->first == "file_name")
		{
			filename = iter->second;
			break;
		}
	}
	if (!access((route + filename).c_str(), F_OK))
		throw (400);
	std::ofstream	newfile(route + filename, std::ios::binary | std::ios::trunc);
	for (std::map<std::string, std::string>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		if (iter->first != "file_name")
			newfile << iter->first << "=" << iter->second << "\n";
	}
}