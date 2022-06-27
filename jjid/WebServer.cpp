#include "WebServer.hpp"
#include <time.h>

std::vector<std::string > makeChunkedVec(std::string originStr);
std::string makeCgiHeader(std::string str)
{
    std::string tmpHeader;
    std::string statusCode = str.substr(str.find(':') + 2, 3);
    std::string tmpStr = str.substr(str.find('\n'));
    tmpStr = tmpStr.substr(str.find(':') + 1);
    tmpHeader = "HTTP/1.1 " + statusCode + " " + "OK" + "\r\n";
    tmpHeader += "Server: a\r\nLast-Modified: a\r\nETag: 'A'\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\n"+tmpStr+"\r\nTransfer-Encoding: chunked\r\n\r\n";
    return tmpHeader;
}
int checkArg(int ac, char **av, std::string& confPath)
{
    if (ac == 2)
		confPath = av[1];
	else if (ac == 1)
		confPath = DEFAULT_PATH;
	else
        return (-1);
    return (0);
}

WebServer::WebServer(std::string confPath) : confPath(confPath)
{
	std::vector<Server> servers;
	std::map < int, Server > serverMap;
}

int WebServer::parseConfig()
{
    Parser parser = Parser();
    
    parser.openConfigfile(this->confPath);
    if (parser.getConfig().empty() == 1)
    {
		printErr("Can not open config file.");
		return 1;
    }
    this->servers = parser.makeServers();
    return 0;
}

void WebServer::listenServers()
{
	for (unsigned long idx = 0; idx < this->servers.size() ; idx++)
	{
		struct sockaddr_in serverAddr;
		int serverSocketFD = socket(PF_INET, SOCK_STREAM, 0);
		int sockopt = 1;
        if (setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEADDR, & sockopt, sizeof(sockopt)) == -1)
            printErr("SO_REUSEADDR Set err: server socket");
		if (serverSocketFD == -1)	
			printErr("Socket error");
		
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_port = htons(this->servers[idx].getPort());
		if (bind(serverSocketFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
			printErr("bind error");
		if (listen(serverSocketFD, 1000) == -1)
			printErr("listen error");
		fcntl(serverSocketFD, F_SETFL, O_NONBLOCK);
		servers[idx].setServerFd(serverSocketFD);
		this->serverMap.insert(std::pair<int, Server>(serverSocketFD, servers[idx]));
	}
}

bool WebServer::checkLastChunked(std::string const &str)
{
    int strSize = str.size();
    if (strSize > 4)
    {
        if (str[strSize - 5] == '0' && str[strSize - 4] == '\r' && str[strSize - 3] == '\n' && str[strSize - 2] == '\r' && str[strSize - 1] == '\n')
            return (true);
    }
    return (false);
}

void WebServer::monitorKqueue()
{ 
	int kq;
    if ((kq = kqueue()) == -1)
        printErr("kqueue error");
        
    std::map<int, int> clientsServerMap;
    std::map<int, int> fdManager;
    std::vector <struct kevent> change_list;
    struct kevent event_list[serverMap.size()];
    
    for (std::map<int, Server>::iterator iter = this->serverMap.begin(); iter != this->serverMap.end(); iter++)
    {
        change_events(change_list, iter->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        iter->second.setServerFd(iter->first);
        iter->second.linkFdManager(fdManager); 
        iter->second.linkChangeList(change_list); 
	}
	
	int new_events;
    struct kevent* curr_event;
    struct timespec kTime;
    
    while (1)
    { 
        memset(&kTime, 0x00, sizeof(kTime));
        kTime.tv_sec = 5;

        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, serverMap.size(), &kTime);
        
        if (new_events == -1)
            usleep(10);
        
        change_list.clear();
        for (int i = 0; i < new_events; ++i)
        {
            curr_event = &event_list[i];
            if (curr_event->flags & EV_ERROR)
            {
                std::map<int, Server>::iterator serverIter = serverMap.find(curr_event->ident);
                if (serverIter != serverMap.end())
                    printErr("server socket error");
            }
            else if (curr_event->filter == EVFILT_READ)
            {
                if (fdManager.find(curr_event->ident) != fdManager.end())
                {
                    Client &currClient = serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]];
                    if (currClient.getCurrLocation().getLocationType() == LOCATIONTYPE_NORMAL || currClient.getCurrLocation().getLocationType() == LOCATIONTYPE_CGI_DONE)
                    {
                        change_events(change_list, fdManager[curr_event->ident], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                        currClient.readFile(curr_event->ident);
                    }
                    else
                    {
                        if (currClient.getCgiPid() > 0)
                        {
                            std::string body;
                            int n;
                            char buf[1024];
                            if ((n = read(curr_event->ident, buf, 1023)) > 0)
                            {
                                buf[n] = '\0';
                                body += buf;
                                memset(buf, 0, 1024);
                            }
                            else if (n <= 0)
                                printErr("CGI read error");
                            currClient.addChunkedStr(body);
                        }
                        if (currClient.getRequestClass().getBody().size() <= currClient.getChunkedStr().size())
                        {
                            std::string cgiBody = currClient.getChunkedStr().substr(currClient.getChunkedStr().find("\r\n\r\n") + 4);
                            if (currClient.getRequestClass().getBody().size() == cgiBody.size())
                            {
                                currClient.getRequestClass().setBody(cgiBody);
                                currClient.getResponseClass().CgiHeader = (currClient.getChunkedStr().substr(0, currClient.getChunkedStr().find("\r\n\r\n")));
                                currClient.getResponseClass().setBody(cgiBody);
                                currClient.getResponseClass().chunkedVec = makeChunkedVec(cgiBody);
                                currClient.getCurrLocation().setLocationType(LOCATIONTYPE_CGI_DONE);
                                
                                std::map <std::string, std::string>::iterator findIter = currClient.getRequestClass().getHeader().getContent().find("Content-Length");
                                currClient.setStatus(DONE);
                                
                                if (findIter != currClient.getRequestClass().getHeader().getContent().end())
                                    findIter->second = std::to_string(currClient.getRequestClass().getBody().size());
                                currClient.setChunkedStr("");
                                
                                int tmpWriteFd = currClient.getWriteFd()[0];
                                int tmpReadFd = currClient.getReadFd()[1];
                                close(tmpWriteFd);
                                close(tmpReadFd);
                                tmpWriteFd = currClient.getWriteFd()[1];
                                tmpReadFd = currClient.getReadFd()[0];
                                
                                int tmpPid = currClient.getCgiPid();
                                if (fdManager.find(tmpWriteFd) != fdManager.end())
                                    fdManager.erase(tmpWriteFd);
                                if (fdManager.find(tmpReadFd) != fdManager.end())
                                    fdManager.erase(tmpReadFd);
                                close(tmpWriteFd);
                                close(tmpReadFd);
                                waitpid(tmpPid, NULL, WNOHANG);
                            }
                        }
                    }
                }
                else if (clientsServerMap.find(curr_event->ident) != clientsServerMap.end() && serverMap.find(clientsServerMap[curr_event->ident]) != serverMap.end() && serverMap[clientsServerMap[curr_event->ident]].getClientMap().find(curr_event->ident) != serverMap[clientsServerMap[curr_event->ident]].getClientMap().end() )
                {
                    Client *currClient = &serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident];
                    
                    char buf[1024];
                    memset(buf,0,1024);
                    int n = read(curr_event->ident, buf, sizeof(buf) - 1);
                    if (n <= 0)
                    {
                        if (n < 0)
                        {
                            printErr("client read error!");
                            close(curr_event->ident);
                            usleep(2500);
                            if (clientsServerMap.find(curr_event->ident) != clientsServerMap.end())
                                clientsServerMap.erase(curr_event->ident);
                            if (serverMap[clientsServerMap[curr_event->ident]].getClientMap().find(curr_event->ident) != serverMap[clientsServerMap[curr_event->ident]].getClientMap().end())
                                serverMap[clientsServerMap[curr_event->ident]].getClientMap().erase(curr_event->ident);
						}
                        else if (n == 0)
                        {
                            close(curr_event->ident);
                            usleep(2500);
                            if (clientsServerMap.find(curr_event->ident) != clientsServerMap.end())
                                clientsServerMap.erase(curr_event->ident);
                            if (serverMap[clientsServerMap[curr_event->ident]].getClientMap().find(curr_event->ident) != serverMap[clientsServerMap[curr_event->ident]].getClientMap().end())
                                serverMap[clientsServerMap[curr_event->ident]].getClientMap().erase(curr_event->ident);
                        }
                    }
                    else
                    {
                        buf[n] = '\0';
                        currClient->getClientBody() += buf;
                        if (currClient->getStatus() == DONE)
                        {
                            currClient->getRequestClass().addBody(currClient->getClientBody());
                            currClient->getClientBody().clear();
                        }
                        else if (currClient->getStatus() == CHUNKED)
                        {
                            currClient->getRequestClass().addBody(currClient->getClientBody());
                            currClient->getClientBody().clear();
                            if (checkLastChunked(currClient->getRequestClass().getBody()))
                            {
                                currClient->parseChunkedBody();
                                currClient->setStatus(CHUNKED_FIN);
                                change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            }
                            else
                                currClient->setStatus(CHUNKED);
                        }
                        else if (currClient->getClientBody().find("\r\n\r\n") != std::string::npos)
                        {
                            currClient->getRequestClass().parseRequestMessage(currClient->getClientBody());
                            std::map <std::string, std::string>::iterator hostIter = currClient->getRequestClass().getHeader().getContent().find("Host");
                            if (hostIter != currClient->getRequestClass().getHeader().getContent().end())
                            {
                                for (unsigned long idx = 0 ; idx < this->servers.size(); idx++)
                                {
                                    for (unsigned long jdx = 0 ; jdx < servers[idx].getHost().size() ; jdx++)
                                    {
										if (servers[idx].getHost()[jdx] + ":" + std::to_string(servers[idx].getPort()) == hostIter->second)
										{
											std::string tmp = currClient->getClientBody();
											
											clientsServerMap[curr_event->ident] = servers[idx].getServerFd();
											serverMap[servers[idx].getServerFd()].addClient(curr_event->ident);
											if (serverMap[currClient->getServerFd()].getClientMap().find(curr_event->ident) != serverMap[currClient->getServerFd()].getClientMap().end())
												serverMap[currClient->getServerFd()].getClientMap().erase(curr_event->ident);
											currClient= &(serverMap[servers[idx].getServerFd()].getClientMap()[curr_event->ident]);
											currClient->getRequestClass().parseRequestMessage(tmp);
										}
									}
                                }
                            }
                            
                            std::map <std::string, std::string>::iterator chunkedIter = currClient->getRequestClass().getHeader().getContent().find("Transfer-Encoding");
                            currClient->getClientBody().clear();
                            if (chunkedIter != currClient->getRequestClass().getHeader().getContent().end() && chunkedIter->second == "chunked")
                            {
                                if(currClient->getRequestClass().getBody().find("0\r\n\r\n") != std::string::npos)
                                {
                                    currClient->parseChunkedBody();
                                    currClient->setStatus(CHUNKED_FIN);
                                    change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                                }
                                else
                                    currClient->setStatus(CHUNKED);
                            }
                            else
                            {
                                currClient->setStatus(DONE);
                                change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            }
                        }
                    }
                }
                else if (serverMap.find(curr_event->ident) != serverMap.end())
                {
                    int clientSocket = 0;
                    int serverSocket = serverMap.find(curr_event->ident)->first;
                    if ((clientSocket = accept(serverSocket, NULL, NULL)) == -1)
                    {
                        printErr("accept err");
                        usleep(10);
                    }
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                    change_events(change_list, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clientsServerMap[clientSocket] = serverSocket;
                    serverMap[serverSocket].addClient(clientSocket);
                }
            }
            else if (curr_event->filter == EVFILT_WRITE)
            {
                if (fdManager.find(curr_event->ident) != fdManager.end())
                {
                    Client &currClient= serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]];
                    if (currClient.getCurrLocation().getLocationType() == LOCATIONTYPE_CGI )
                    {
                        std::string &currStr = currClient.getRequestClass().getBody();
                        int n;
                        int size =  currStr.size() >= 1023 ? 1023 : currStr.size();
                        if (( n = write(curr_event->ident ,currStr.c_str(), size)) > 0)
                        {
                            currClient.addChunkedWriteSize(n);
                            if (currClient.getCgiPid() < 0)
                                currClient.forkCgiPid();
                            if (currClient.getCgiPid() == 0)
                            {
                                int tmpWriteFd = currClient.getWriteFd()[1];
                                int tmpReadFd = currClient.getReadFd()[0];
                                
                                dup2(currClient.getReadFd()[1],1);
                                close(tmpReadFd);
                                dup2(currClient.getWriteFd()[0],0);
                                close(tmpWriteFd);
                                
                                char *test[2] ;
                                test[0] = (char *)(currClient.getCurrLocation().getCgiPath().c_str());
                                test[1] = NULL;
                                if((execve(currClient.getCurrLocation().getCgiPath().c_str(),test, currClient.makeEnvp(currClient.getRequestClass().getBody().size()))) == -1 )
                                {
                                    printErr("execve err");
                                    exit(1);
                                }
                                exit(1);
                            }
                            if (currClient.getChunkedWriteSize() >= static_cast<int>(currClient.getRequestClass().getBody().size()) )
                            {
                                change_events(change_list, currClient.getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                                currClient.setChunkedWriteSize(0);
                            }
                            else
                                change_events(change_list, currClient.getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                        }
                        else
                        {
                            if (n == -1)
                                printErr("parent to child write err");
                            change_events(change_list, currClient.getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            currClient.setChunkedWriteSize(0);
                        }
                    }
                    else if (currClient.getCurrLocation().getLocationType() == LOCATIONTYPE_CGI_DONE || currClient.getCurrLocation().getLocationType() == LOCATIONTYPE_NORMAL)
                        currClient.writeFile(curr_event->ident);
                }
                else if (clientsServerMap.find(curr_event->ident) != clientsServerMap.end() &&serverMap.find(clientsServerMap[curr_event->ident]) != serverMap.end() && serverMap[clientsServerMap[curr_event->ident]].getClientMap().find(curr_event->ident) != serverMap[clientsServerMap[curr_event->ident]].getClientMap().end())
                {
                    Client &currClient = serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident];
                    if (currClient.getResponseClass().getStatusCode() != 0 )
                    {
                        int n = 0;
                        if (currClient.getVecIdx() == -1)
                        {
                            std::string ResponseMessage = currClient.getResponseClass().writeResponseMessage();
                            n = write(curr_event->ident, ResponseMessage.c_str(), ResponseMessage.size());
                            if (n == -1)
                            {
                                printErr("client write err");
                                close(curr_event->ident);
                                usleep(2500);
                                if (clientsServerMap.find(curr_event->ident) != clientsServerMap.end())
                                    clientsServerMap.erase(curr_event->ident);
                                if (serverMap[clientsServerMap[curr_event->ident]].getClientMap().find(curr_event->ident) != serverMap[clientsServerMap[curr_event->ident]].getClientMap().end())
                                    serverMap[clientsServerMap[curr_event->ident]].getClientMap().erase(curr_event->ident);
							}
                            else if (n >= 0)
                                currClient.getVecIdx()++;
                        }
                        else
                        {
                            n = write(curr_event->ident, currClient.getResponseClass().getBody().substr(currClient.getWriteCnt()).c_str() , currClient.getResponseClass().getBody().substr(currClient.getWriteCnt()).size()) ;
                            if ( n == -1)
                            {
                                printErr("client write err");
                                close(curr_event->ident);
								usleep(2500);
								if (clientsServerMap.find(curr_event->ident) != clientsServerMap.end())
									clientsServerMap.erase(curr_event->ident);
								if (serverMap[clientsServerMap[curr_event->ident]].getClientMap().find(curr_event->ident) != serverMap[clientsServerMap[curr_event->ident]].getClientMap().end())
									serverMap[clientsServerMap[curr_event->ident]].getClientMap().erase(curr_event->ident);
							}
                            else if (n >= 0)
                            {
                                currClient.getWriteCnt()+=n;
                            }
                            if (currClient.getWriteCnt() == static_cast<int>(currClient.getResponseClass().getBody().size()))
                                currClient.resetServerValues();
                        }
                    }
                    else if (currClient.getStatus()== DONE)
                    {
                        std::map <std::string, std::string>::iterator findIter = currClient.getRequestClass().getHeader().getContent().find("Content-Length");
                        if (findIter != currClient.getRequestClass().getHeader().getContent().end())
                        {   
                            if (std::atoi(findIter->second.c_str()) == static_cast<int>(currClient.getRequestClass().getBody().size()))
                            {
                                currClient.preProcess(currClient.getCurrLocation().getLocationType());
                                currClient.processMethod(change_list);
                                currClient.setStatus(READY);
                            }
                        }
                        else
                        {
                            currClient.preProcess(currClient.getCurrLocation().getLocationType());
                            currClient.processMethod(change_list);
                            currClient.setStatus(READY);
                        }
                    }
                    else if (currClient.getStatus() == CHUNKED_FIN)
                    {
                        currClient.preProcess(currClient.getCurrLocation().getLocationType());
                        currClient.processMethod(change_list);
                        currClient.setStatus(READY);
                    }
                }
            }            
        }
    }
    return ;
}

int main (int ac, char **av)
{
	std::string confPath;

    if ((checkArg(ac, av, confPath) == -1))
    {
		printErr("Too many argu");
        return 0;
    }
	WebServer myFirstWebServer(confPath);
	if (myFirstWebServer.parseConfig())
	    return 0;
	myFirstWebServer.listenServers();
	myFirstWebServer.monitorKqueue();
}