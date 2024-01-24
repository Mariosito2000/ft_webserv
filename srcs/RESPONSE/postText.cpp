#include "../../includes/webserv.hpp"

/* STRUCT
RN
K=VRN
K=VRN
K=VRN
*/


void	postText(std::string &route, const char *body, size_t size)
{
	std::map<std::string, std::string > map;
	std::string	filename;
	std::string str;

	/* std::cerr << "BODY:\n";
	for (size_t i = 0; i < size; i++)
	{
		std::cerr << body[i];
	}
	std::cerr << "\n--BODY--\n"; */
	for (size_t i = 0; i < size; i++)
	{
		if (i + 1 < size && body[i] == '\r' && body[i + 1] == '\n')
		{
			if (!str.empty())
			{
			/* 	std::cerr << "\n\nSTR: -";
				std::cerr << str << "-\n"; */
				size_t equal = str.find('=');
				if (equal == std::string::npos || equal == 0 || equal == str.size() -1)
					throw (400);
				/* std::cerr << "\nKEY: -" << str.substr(0, equal);
				std::cerr << "-\nVALUE: -" << str.substr(equal + 1);
				std::cerr << "-\n\n"; */
				map.insert(std::pair<std::string, std::string> (str.substr(0, equal), str.substr(equal + 1)));
				str.clear();
			}
			i += 2;
		}
		if (i < size)
		{
			//std::cerr << body[i];
			str += body[i];
		}
	}
	for (std::map<std::string, std::string>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		if (iter->first == "file_name")
		{
			filename = iter->second;
			break;
		}
	}
	//std::cerr << "\nRUTA: -" << (route + filename).c_str() << "-\n";
	if (!access((route + filename).c_str(), F_OK))
		throw (400);
	std::ofstream	newfile(route + filename, std::ios::binary | std::ios::trunc);
	for (std::map<std::string, std::string>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		if (iter->first != "file_name")
		{
			newfile << iter->first << "=" << iter->second << "\n";
		}
	}
}