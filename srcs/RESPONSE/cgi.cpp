#include "../../includes/webserv.hpp"

char **getCgiEnv(std::string path, client* client) {

    std::vector<std::string> env;
    char **arrayEnv;
    
    std::string scriptName;;
    scriptName = "SCRIPT_NAME=" + path.substr(0, path.find('.') + 3);
    env.push_back(scriptName);

    std::string requestedMethod = "REQUEST_METHOD=" + client->request.method;
    env.push_back(requestedMethod);

    std::string contentLength = "CONTENT_LENGTH=0";
    size_t contentLengthVal = 0;

    if (client->request.method == "POST")
    {
        std::string *contentLengthStr = getMultiMapValue(client->request.headers, "Content-Length");
        if (contentLengthStr)
        {
            contentLengthVal = atoi((*getMultiMapValue(client->request.headers, "Content-Length")).c_str());
            contentLength = "CONTENT_LENGTH=" + *contentLengthStr;
        }
 
        client->request.query = client->request.buf.substr(0, contentLengthVal);
    }
    std::string queryString = "QUERY_STRING=" + client->request.query;
    env.push_back(queryString);
    env.push_back(contentLength);
    
    std::string pathInfo = "PATH_INFO=" + client->request.pathInfo;
    env.push_back(pathInfo);
    
    arrayEnv = (char **)malloc(sizeof(char *) * (env.size() + 1));
    if (!arrayEnv)
        exit(1);
    size_t i = 0;
    for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); it++)
    {
        arrayEnv[i] = strdup((char *)(*it).c_str());
        if (!arrayEnv[i])
            exit(1);
        i++;
    }
    arrayEnv[i] = NULL;
    return (arrayEnv);

}

void CGIForward(client *client)
{
    std::string                 path;
    std::vector<std::string>    redirs;

    redirs.push_back("cgi_pass");
    path = getPathFileRequest(client, redirs);
    if (access(path.c_str(), F_OK) != 0)
        throw (NOT_FOUND);
    if (access(path.c_str(), X_OK) != 0)
        throw (INTERNAL_SERVER_ERROR);
    int pipes[2];
    int status;
    if (pipe(pipes) == -1)
       throw (INTERNAL_SERVER_ERROR);

    pid_t exec_pid = fork();
    if (exec_pid == -1) 
        throw (INTERNAL_SERVER_ERROR);
    
    if (exec_pid == 0)
    {
        char **cgiEnv = getCgiEnv(path, client);
        if (close(pipes[0]) == -1) 
            throw (INTERNAL_SERVER_ERROR);
        if (dup2(pipes[1], STDOUT_FILENO) == -1)
            throw (INTERNAL_SERVER_ERROR);
        if (execve(path.c_str(), NULL, cgiEnv) != 0)
			throw (INTERNAL_SERVER_ERROR);
    }
    else if (exec_pid > 0) {

        time_t ref = std::time(NULL);
        time_t now;
        while (waitpid(exec_pid, &status, WNOHANG) == 0)
        {
            now = std::time(NULL);
            if (now > ref + CGITIMEOUT)
            {
                kill(exec_pid, SIGKILL);
                throw (GATEWAY_TIMEOUT); 
            }
        }
    }
    if (WEXITSTATUS(status) != 0)
        throw(BAD_GATEWAY);
    if (close(pipes[1]) == -1)
        throw (INTERNAL_SERVER_ERROR);
    char    *readCGI = readFileSeLst(pipes[0]);
    if (close(pipes[0]) == -1)
        throw (INTERNAL_SERVER_ERROR);
    std::string CGIstring = readCGI;
    free(readCGI);

    client->request.status = 200;
    HttpResponse Response;
    Response.body = CGIstring;
    Response.firstLine = getResponseHeader(client->request, Response.body);
    client->response.response = Response.firstLine + Response.body;
    return ;
}
