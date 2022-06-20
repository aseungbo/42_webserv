#include "WebServer.hpp"
#include <time.h>
// std::vector<Server> servers;
// // std::vector < int > serverFd; //자료형 고려
// std::string confPath;
// std::map < int, Server > serverMap; // 서버에서 뭘 받아온다면 다시 고려 
// // EventHandler // 언젠가 꼭 고려해보쟈 ><
			
std::vector<std::string > makeChunkedVec(std::string originStr);
std::string makeCgiHeader(std::string str)
{
    // 			    Status: 200 OK
    // Content-Type: text/html; charset=utf-8
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
	//  confPath 나중에 리드할때 (파스 컨피그)에서 오류체크할거임
	std::vector<Server> servers;
	std::map < int, Server > serverMap; // 서버에서 뭘 받아온다면 다시 고려 
	// std::vector < int > serverFd; //자료형 고려
	// EventHandler // 언젠가 꼭 고려해보쟈 ><
}
// ConfigParser 불러옴

void WebServer::parseConfig()
{
    Parser parser;
    
    parser.openConfigfile(this->confPath);
    if (parser.getConfig().empty() == 1)
		printErr("Can not open file.");
    this->servers = parser.makeServers();
    if (this->servers.size() == 0)
        printErr("Can not make servers.");
}

// kqueue 생성 
// void WebServer::makeKqueue()
// {
	
// }

void WebServer::listenServers()
{
    std::cout << "\033[47;30m[ Available Port ]\033[0m" << std::endl;;
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
        std::cout << "serv port: " << this->servers[idx].getPort() << std::endl;
		serverAddr.sin_port = htons(this->servers[idx].getPort());
		if (bind(serverSocketFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
			printErr("bind error");
		if (listen(serverSocketFD, 1000) == -1)
			printErr("listen error");
		fcntl(serverSocketFD, F_SETFL, O_NONBLOCK);//넌 이제부터 논블로킹이야
		this->serverMap.insert(std::pair<int, Server>(serverSocketFD, servers[idx]));
	}
}

void WebServer::disconnect_client(int client_fd, Server &currServer, std::map<int, int> &clientsServerMap)
{
    std::cout << "client disconnected: " << client_fd << std::endl;
    
    close(client_fd);
    // clients.erase(client_fd);
    // currServer.getClientMap().erase(currServer.getClientMap().find(client_fd));//이거 잘지워짐?
    clientsServerMap.erase(client_fd);
    currServer.getClientMap().erase(client_fd);
    // clients.erase(client_fd);
    // clientsServerMap.erase(client_fd);
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
    // clock_t start, finish;
    // double duration;   
    // start = clock();
 
	int kq;
    if ((kq = kqueue()) == -1)
        printErr("kqueue error");
        
    // std::map<int, std::string> clients;
    std::map<int, int> clientsServerMap;
    std::map<int, int> fdManager;//cgi, resource  저장 할건데 이
    std::vector <struct kevent> change_list;
    struct kevent event_list[serverMap.size()];
    
    for (std::map<int, Server>::iterator iter = this->serverMap.begin(); iter != this->serverMap.end(); iter++)
    {
        change_events(change_list, iter->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        iter->second.setServerFd(iter->first);
        iter->second.linkFdManager(fdManager); 
        iter->second.linkChangeList(change_list); 
	}
	std::cout << "echo server started" << std::endl;
	
	int new_events;
    struct kevent* curr_event;
    struct timespec kTime;
    
	// int requestCnt = 0;
    while (1)
    {
        memset(&kTime, 0x00, sizeof(kTime));
        kTime.tv_sec = 5;

        /*  apply changes and return new events(pending events) */
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, serverMap.size(), &kTime);
        // new_events = kevent(kq, &change_list[0], change_list.size(), event_list, serverMap.size(), NULL);
        
        // std::cout << "======= " << new_events << "=====" << std::endl;
        
        if (new_events == -1)
        {
            usleep(10);
        }
        
        change_list.clear();
        for (int i = 0; i < new_events; ++i)
        {
            curr_event = &event_list[i];
            /* check error event return */
            if (curr_event->flags & EV_ERROR)
            {
                std::map<int, Server>::iterator serverIter = serverMap.find(curr_event->ident);
                if (serverIter != serverMap.end())
                    printErr("server socket error");
            }
            else if (curr_event->filter == EVFILT_READ)
            {
                // map indexing으로 접근 가능한지 확인해볼 것
                //TODO : 어떤서버에 연결할지 함수로 만들었으면 좋겠다.
                if (fdManager.find(curr_event->ident) != fdManager.end())
                {
                    Client &currClient = serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]];
                    if (currClient.getCurrLocation().getLocationType() == LOCATIONTYPE_NORMAL || currClient.getCurrLocation().getLocationType() == LOCATIONTYPE_CGI_DONE)
                    {
                        change_events(change_list, fdManager[curr_event->ident], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL); // add event
                        currClient.readFile(curr_event->ident);//일반 get 메소드 
                        // std::cout << "리드파일 끝나고 잘 지워졌니?" << (int)(fdManager.find(curr_event->ident) != fdManager.end()) << std::endl;
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
                            currClient.addChunkedStr(body);
                        }
                        if (currClient.getRequestClass().getBody().size() <= currClient.getChunkedStr().size())//추후에 \r\n 이후거만 비교 해서 ==으로 수정
                        {
                            std::string cgiBody = currClient.getChunkedStr().substr(currClient.getChunkedStr().find("\r\n\r\n") + 4);
                            if (currClient.getRequestClass().getBody().size() == cgiBody.size())
                            {
                                currClient.getRequestClass().setBody(cgiBody);
                                currClient.getResponseClass().CgiHeader = (currClient.getChunkedStr().substr(0, currClient.getChunkedStr().find("\r\n\r\n")));
                                currClient.getResponseClass().setBody(cgiBody);
                                std::cout << "set size" <<  currClient.getResponseClass().getBody().size() << std::endl;
                                currClient.getResponseClass().chunkedVec = makeChunkedVec(cgiBody);
                                currClient.getCurrLocation().setLocationType(LOCATIONTYPE_CGI_DONE);
                                
                                std::map <std::string, std::string>::iterator findIter = currClient.getRequestClass().getHeader().getContent().find("Content-Length");
                                currClient.setStatus(DONE);
                                
                                if (findIter != currClient.getRequestClass().getHeader().getContent().end())//길이헤더 찾았을때
                                    findIter->second = std::to_string(currClient.getRequestClass().getBody().size());
                                currClient.setChunkedStr("");
                                
                                int tmpWriteFd = currClient.getWriteFd()[0];
                                int tmpReadFd = currClient.getReadFd()[1];
                                close(tmpWriteFd);
                                close(tmpReadFd);
                                tmpWriteFd = currClient.getWriteFd()[1];
                                tmpReadFd = currClient.getReadFd()[0];

                                
                                int tmpPid = currClient.getCgiPid();
                                fdManager.erase(tmpWriteFd);
                                fdManager.erase(tmpReadFd);
                                close(tmpWriteFd);
                                close(tmpReadFd);
                                // std::cout << "erase result" << (int)(fdManager.find(tmpWriteFd) != fdManager.end()) <<", "<<(int)(fdManager.find(tmpReadFd) != fdManager.end())<<std::endl;
                                waitpid(tmpPid, NULL, WNOHANG);
                            }
                        }
                    }
                }
                else if (clientsServerMap.find(curr_event->ident) != clientsServerMap.end() && serverMap.find(clientsServerMap[curr_event->ident]) != serverMap.end() && serverMap[clientsServerMap[curr_event->ident]].getClientMap().find(curr_event->ident) != serverMap[clientsServerMap[curr_event->ident]].getClientMap().end() )
                {
                    Client &currClient =  serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident];
                    
                    char buf[1024];
                    memset(buf,0,1024);
                    int n = read(curr_event->ident, buf, sizeof(buf) - 1);
                    if (n <= 0)
                    {
                        if (n < 0)
                            printErr("client read error!");
                        // std::cout << "read:diconnect call" <<std::endl;
                        disconnect_client(curr_event->ident, serverMap[clientsServerMap[curr_event->ident]], clientsServerMap);// 0612역시 여기서 냅다 초기화날려야할지두
                        usleep(300);
                    }
                    else
                    {
                        buf[n] = '\0';
                        currClient.getClientBody() += buf;
                        if (currClient.getStatus() == DONE)
                        {
                            currClient.getRequestClass().addBody(currClient.getClientBody());
                            currClient.getClientBody().clear();//0612추가
                        }
                        else if (currClient.getStatus() == CHUNKED)
                        {
                            currClient.getRequestClass().addBody(currClient.getClientBody());
                            // std::cout << "size: " << currClient.getRequestClass().getBody().size() << std::endl;
                            currClient.getClientBody().clear();
                            if ( checkLastChunked(currClient.getRequestClass().getBody()) )//
                            {
                                // 바디 rn기준으로 잘라넣을 거 추가;
                                currClient.parseChunkedBody();
                                currClient.setStatus(CHUNKED_FIN);
                                change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            }
                            else//지워도됨
                                currClient.setStatus(CHUNKED);
                        }
                        else if (currClient.getClientBody().find("\r\n\r\n") != std::string::npos)
                        {
                            // std::cout << "request header in ident:"<<curr_event->ident<<std::endl;
                            currClient.getRequestClass().parseRequestMessage(currClient.getClientBody());
                            std::map <std::string, std::string>::iterator chunkedIter = currClient.getRequestClass().getHeader().getContent().find("Transfer-Encoding");
                            currClient.getClientBody().clear();
                            if (chunkedIter != currClient.getRequestClass().getHeader().getContent().end() && chunkedIter->second == "chunked")
                            {
                                if(currClient.getRequestClass().getBody().find("0\r\n\r\n") != std::string::npos)
                                {
                                    // 바디 rn기준으로 잘라넣을 거 추가;
                                    currClient.parseChunkedBody();
                                    currClient.setStatus(CHUNKED_FIN);
                                    change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                                }
                                else
                                    currClient.setStatus(CHUNKED);
                            }
                            else
                            {
                                currClient.setStatus(DONE);
                                change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            }
                        }
                    }
                }
                //알맞은서버찾아서 이터든 뭐든 반환?
                else if (serverMap.find(curr_event->ident) != serverMap.end())
                {
                    /* accept new client */
                    int clientSocket = 0;
                    int serverSocket = serverMap.find(curr_event->ident)->first;
                    if ((clientSocket = accept(serverSocket, NULL, NULL)) == -1)
                    {
                        printErr("accept err");
                        usleep(10); //이부분 제거하고 알맞은 로직 필요할듯 당장 예상하는 방법은 continue;
                    }
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                    // struct linger solinger = { 1, 0 }; 
                    // if (setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, &solinger, sizeof(struct linger)) == -1) {
                    //     perror("setsockopt(SO_LINGER)"); 
                    // }
                    // std::cout << "new accept : "<< clientSocket <<  std::endl;
                    change_events(change_list, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clientsServerMap[clientSocket] = serverSocket;
                    serverMap[serverSocket].addClient(clientSocket);
                }
            }
            else if (curr_event->filter == EVFILT_WRITE)
            {
                if (fdManager.find(curr_event->ident) != fdManager.end())
                {
                    Client &currClient = serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]];
                    if (currClient.getCurrLocation().getLocationType() == LOCATIONTYPE_CGI )//cgi처리로직 조건
                    {
                        std::string &currStr = currClient.getRequestClass().getBody();
                        // std::cout << "size:::" << currClient.getRequestClass().getBody().size()<<std::endl;
                        int n;
                        int size =  currStr.size() >= 1023 ? 1023 : currStr.size(); // Body size가 1023보다 작다면 Body size만큼만 write
                        if (( n = write(curr_event->ident ,currStr.c_str(), size)) > 0)
                        {
                            currClient.addChunkedWriteSize(n);
                            
                            // std::cout << "write size:" << currClient.getRequestClass().getBody().size() <<std::endl;
                            //TODO : pid 초기값 -1
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
                                    // write(currClient.getReadFd()[1], "errororor\n", 11);
                                    exit(1);
                                }
                                exit(1);
                            }
                            if (currClient.getChunkedWriteSize() >= static_cast<int>(currClient.getRequestClass().getBody().size()) )
                            {
                                // std::cout <<"cgi write end not else"<<std::endl;
                                change_events(change_list, currClient.getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                                // std::cout << "read[fd]: " << currClient.getReadFd()[0] << std::endl;
                                currClient.setChunkedWriteSize(0);
                            }
                            else
                                change_events(change_list, currClient.getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                        }
                        else
                        {
                            if (n == -1)
                                printErr("parent to child write err");
                            // std::cout <<"cgi write end else"<<std::endl;
                            change_events(change_list, currClient.getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            // std::cout << "read[fd]: " << currClient.getReadFd()[0] << std::endl;
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
                        if (currClient.vecIdx == -1)//vecIdx 이름 바꿔 헤더인지아닌지 판단하려고 있던거 일단 쓴거임
                        {
                            std::string ResponseMessage = currClient.getResponseClass().writeResponseMessage();
                            n = write(curr_event->ident, ResponseMessage.c_str(), ResponseMessage.size());
                            if (n == -1)
                            {
                                printErr("client write err");
                                // disconnect_client(curr_event->ident, serverMap[clientsServerMap[curr_event->ident]], clientsServerMap);
                            }
                            else if (n >= 0)
                            {
                                currClient.vecIdx++;
                            }
                        }
                        else
                        {
                            // std::cout << "writecnt"<< currClient.writeCnt<<std::endl;
                            n = write(curr_event->ident, currClient.getResponseClass().getBody().substr(currClient.writeCnt).c_str() , currClient.getResponseClass().getBody().substr(currClient.writeCnt).size()) ;
                            // std::cout << "n"<< n<<std::endl;
                            if ( n == -1)
                            {
                                printErr("client write err");
                                // disconnect_client(curr_event->ident, serverMap[clientsServerMap[curr_event->ident]], clientsServerMap); // lsof -p pid 찍어보면 server에는 문제가 없음.
                            }
                            else if (n >= 0)
                            {
                                currClient.writeCnt+=n;
                            }
                            if (currClient.writeCnt == static_cast<int>(currClient.getResponseClass().getBody().size()))
                            {
                                currClient.resetServerValues();
                                // close(curr_event->ident);
                                disconnect_client(curr_event->ident, serverMap[clientsServerMap[curr_event->ident]], clientsServerMap); // lsof -p pid 찍어보면 server에는 문제가 없음.
                                usleep(1500);
                            }
                        }
                    }
                    else if (currClient.getStatus()== DONE)
                    {
                        // std::cout << "satatus DONE"<<std::endl;;
                        std::map <std::string, std::string>::iterator findIter = currClient.getRequestClass().getHeader().getContent().find("Content-Length");
                        if (findIter != currClient.getRequestClass().getHeader().getContent().end())//길이헤더 찾았을때
                        {   
                            if (std::atoi(findIter->second.c_str()) == static_cast<int>(currClient.getRequestClass().getBody().size()))//바디사이즈까지 같을때
                            {
                                currClient.preProcess(currClient.getCurrLocation().getLocationType());
                                currClient.processMethod(change_list);
                                currClient.setStatus(READY);
                            }
                        }
                        else//못찾았을때인데 헤더파싱은 끝나야함
                        {
                            // std::cout << "satatus!! "<< currClient.getCurrLocation().getLocationType()<<std::endl;;
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
		printErr("Too many argu");
	WebServer myFirstWebServer(confPath);
	myFirstWebServer.parseConfig();
	myFirstWebServer.listenServers();
	myFirstWebServer.monitorKqueue();
	// string confPath = string checkArgu(ac, av);// -> 1 : conf파일 제대로 들어온경우 2 : default로 가야하는경우 0 : 종료해야할 경우(잘못된 파일,)
	// // WebServer a;
	// Server a;
}