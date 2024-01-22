#include "../../includes/webserv.hpp"

char **getCgiEnv(std::string &path, client* client) {

	std::vector<std::string> env;
	char **arrayEnv;
	
	std::string translatedScript;
	std::string scriptName;
	if (path.find('?') != std::string::npos)
		scriptName = path.substr(0, path.find('?'));
	else
		scriptName = path;
	env.push_back("SCRIPT_NAME=" + scriptName);

	std::string requestedMethod = "REQUESTED_METHOD=" + client->request.method;
	env.push_back(requestedMethod);

	std::string contentLength = "CONTENT_LENGTH=0";
	size_t contentLengthVal = 0;

	if (client->request.method == "POST")
	{
		std::cout << "----CGI POST-----" << std::endl;
		std::string *contentLengthStr = getMultiMapValue(client->request.headers, "Content-Length");
		if (contentLengthStr)
		{
			contentLengthVal = atoi((*getMultiMapValue(client->request.headers, "Content-Length")).c_str());
			contentLength = "CONTENT_LENGTH=" + *contentLengthStr;
		}

		client->request.query = client->request.buf.substr(0, contentLengthVal);
		std::cout << "BUF IS: " << client->request.buf << std::endl;
		std::cout << "POST QUERY IS: " << client->request.query << std::endl;
	}
	env.push_back("QUERY_STRING=" + client->request.query);
		
	env.push_back(contentLength);
	

	env.push_back("PATH_INFO=" + path);
	
	for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); ++it)
		std::cout << "Vector env var is: " << *it << std::endl;

	std::cout << "----------" << std::endl;

	arrayEnv = (char **)malloc(sizeof(char *) * (env.size()));
	if (!arrayEnv)
		exit(1);
	size_t i = 0;
	for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); ++it)
	{
		arrayEnv[i] = (char *)malloc(sizeof(char) * (*it).size());
		if (!arrayEnv[i])
			exit(1);
		arrayEnv[i] = (char *)(*it).c_str();
		i++;
	}
	arrayEnv[i] = NULL;

	for (int i = 0; arrayEnv[i]; i++) {
		std::cout << "Array env var is: " << arrayEnv[i] << std::endl;
	}
	return (arrayEnv);

}

std::string CGIForward(std::string &path, client *client)
{
    std::cout << "Path del ejecutable: " << path << std::endl;
	//tiene que partirse por la query y luego hacer el chequeo, no se puede hacer sobre path
	if (access(path.c_str(), X_OK) != 0)
		throw (404);
    int pipes[2];
    if (pipe(pipes) == -1)
        exit (1);

    int pid1 = fork();
    if (pid1 == 0)
    {
        std::cout << "Estoy en proceso hijo" << std::endl;
        std::cout << "path is: " << path << std::endl;
    	char **cgiEnv = getCgiEnv(path, client);
		std::string newPath = path.substr(0, path.find('?'));
        dup2(pipes[1], STDOUT_FILENO);
        close(pipes[0]);
        if (execve(newPath.c_str(), NULL, cgiEnv) < 0) {
            exit(1);
        }
    }
   // int status;
	//if the process that first exits is timeout, then error 408
    //waitpid(pid, &status, 0); //flag for timeout
    std::cout << "Estoy en proceso padre" << std::endl;
    close(pipes[1]);
    char    *readCGI = readFileSeLst(pipes[0]);
	close(pipes[0]);
	std::string CGIstring = readCGI;
	free(readCGI);
    std::cout << "Fichero leído: " << CGIstring << std::endl;
    return (CGIstring);
}