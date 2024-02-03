#include "../../includes/webserv.hpp"

void	postText(std::string &route, const char *body, size_t size)
{
	std::map<std::string, std::string > map;
	std::string	filename;
	std::string str;

	for (size_t i = 0; i < size; i++)
	{
		if (i + 1 < size && body[i] == '\r' && body[i + 1] == '\n')
		{
			if (!str.empty())
			{
				size_t equal = str.find('=');
				if (equal == std::string::npos || equal == 0 || equal == str.size() -1)
					throw (BAD_REQUEST);
				map.insert(std::pair<std::string, std::string> (str.substr(0, equal), str.substr(equal + 1)));
				str.clear();
			}
			i += 2;
		}
		if (i < size)
			str += body[i];
	}
	for (std::map<std::string, std::string>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		if (iter->first == "filename")
		{
			filename = iter->second;
			break;
		}
	}
	if (!access((route + filename).c_str(), F_OK))
		throw (CONFLICT);
	std::ofstream	newfile(route + filename, std::ios::binary | std::ios::trunc);
	for (std::map<std::string, std::string>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		if (iter->first != "filename")
			newfile << iter->first << "=" << iter->second << "\n";
	}
}