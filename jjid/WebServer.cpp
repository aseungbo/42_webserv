#include "WebServer.hpp"
			// std::vector<Server> servers;
			// // std::vector < int > serverFd; //자료형 고려
			// std::string confPath;
			// std::map < int, Server > serverMap; // 서버에서 뭘 받아온다면 다시 고려 
			// // EventHandler // 언젠가 꼭 고려해보쟈 ><

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
	for (int idx = 0; idx < this->servers.size() ; idx++)
	{
		struct sockaddr_in serverAddr;
		int serverSocketFD = socket(PF_INET, SOCK_STREAM, 0);
		if (serverSocketFD == -1)	
			printErr("Socket error");
		
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        std::cout << "serv port: " << this->servers[idx].getPort() << std::endl;
		serverAddr.sin_port = htons(this->servers[idx].getPort());
		if (bind(serverSocketFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
			printErr("bind error");
		if (listen(serverSocketFD, 5) == -1)
			printErr("listen error");
		fcntl(serverSocketFD, F_SETFL, O_NONBLOCK);//넌 이제부터 논블로킹이야
		this->serverMap.insert(std::pair<int, Server>(serverSocketFD, servers[idx]));
	}
}
//서버들 리슨 바인드 까지
// void WebServer::mapFd()
// {
	
// }

// servers 시작 -> serverFd -> serverMap
// int read_data(int fd, char *buffer, int buf_size)
// {
// 	int size = 0;
// 	int len;
// 	while (1)
// 	{
// 		if ((len = read(fd, &buffer[size], buf_size - size)) > 0)
// 		{
// 			size += len;
// 			if (size == buf_size)
// 			{
// 				return size;
// 			}
// 		}
// 		else if (len == 0)
// 		{
// 			return size;
// 		}
// 		else
// 		{
// 			if (errno == EINTR)
// 			{
// 				continue;
// 			}
// 			else
// 			{
// 				return -1;
// 			}
// 		}
// 	}
// }

void change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

void disconnect_client(int client_fd, std::map<int, std::string>& clients, std::map<int, int> &clientsServerMap)
{
    std::cout << "client disconnected: " << client_fd << std::endl;
    close(client_fd);
    clients.erase(client_fd);
    clientsServerMap.erase(client_fd);
}

void WebServer::monitorKqueue()
{
	int kq;
    if ((kq = kqueue()) == -1)
        printErr("kqueue error");
        
    std::map<int, std::string> clients;
    std::map<int, int> clientsServerMap;
    std::vector <struct kevent> change_list;
    struct kevent event_list[serverMap.size()];
    
    for (std::map<int, Server>::iterator iter = this->serverMap.begin(); iter != this->serverMap.end(); iter++)
    {
        
        change_events(change_list, iter->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	}
	std::cout << "echo server started" << std::endl;
	
	
	int new_events;
    struct kevent* curr_event;
    
	int requestCnt = 0;
    while (1)
    {
        /*  apply changes and return new events(pending events) */
        // std::cout << "Hello JJIDRAGON WORLD" << std::endl;
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, serverMap.size(), NULL);
        if (new_events == -1)
        {
            // printErr("kevent() error\n");
            usleep(10);
        }

        change_list.clear(); // clear change_list for new changes
		// std::cout << "new_events : " << new_events<<std::endl;
        for (int i = 0; i < new_events; ++i)
        {
            curr_event = &event_list[i];
            /* check error event return */
            if (curr_event->flags & EV_ERROR)
            {
                std::map<int, Server>::iterator serverIter = serverMap.find(curr_event->ident);
                if (serverIter != serverMap.end())
                    printErr("server socket error");
                // else
                // {
                    // printErr("client socket error");
                    // disconnect_client(curr_event->ident, clients);
                // }
            }
            else if (curr_event->filter == EVFILT_READ)
            {
                // map indexing으로 접근 가능한지 확인해볼 것
                //TODO : 어떤서버에 연결할지 함수로 만들었으면 좋겠다.
                std::map<int, Server>::iterator serverIter = serverMap.find(curr_event->ident);
                //알맞은서버찾아서 이터든 뭐든 반환?
                if (serverIter != serverMap.end())
                // if (curr_event->ident == serverIter->first)
                {
                    /* accept new client */
                    int clientSocket = 0;
                    int serverSocket = serverIter->first;
                    if ((clientSocket = accept(serverSocket, NULL, NULL)) == -1)
                    {
                        usleep(10);
                        // printErr("accept() error\n");
                        // exit(0);
                    }
                    // serverMap[curr_event->ident].setClientSocket(clientSocket);
                    // std::cout << "accept new client: " << clientSocket << std::endl;
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK);

                    /* add event for client socket - add read && write event */
                    change_events(change_list, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    change_events(change_list, clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clients[clientSocket] = "";
                    clientsServerMap[clientSocket] = serverSocket;
                }
                else if (clients.find(curr_event->ident) != clients.end())
                {
                    // parse request
                    char buf[1024];
                    memset(buf,0,1024);
                    int n = read(curr_event->ident, buf, sizeof(buf));
                    if (n <= 0)
                    {
                        if (n < 0)
                            printErr("client read error!");
                        std::cout << "read:diconnetc call" <<std::endl;
                        // serverMap[clientsServerMap[curr_event->ident]].setStatus(READY);
                        disconnect_client(curr_event->ident, clients, clientsServerMap);
                    }
                    else
                    {
                        buf[n] = '\0';
                        clients[curr_event->ident] += buf;
                        Server &currSever = serverMap[clientsServerMap[curr_event->ident]];
                        if (currSever.getStatus() == DONE)
                        {
                            currSever.getRequestClass().addBody(buf);
                        }
                        else if (currSever.getStatus() == CHUNKED || currSever.getStatus() == CHUNKED_ALIVE || currSever.getStatus() == CHUNKED_DONE)
                        {
                            if (currSever.getStatus() == CHUNKED || currSever.getStatus() == CHUNKED_DONE)
                            {
                                std::cout << "ident : " << clients[curr_event->ident]  << std::endl;
                                int findIdx = clients[curr_event->ident].find("\r\n");
                                std::string firstLine = clients[curr_event->ident].substr(0, findIdx+2);
                                long chunkedSize = std::strtol(firstLine.c_str(), NULL, 16);
                                std::cout << "chunkedSize : " << chunkedSize  << std::endl;
                                currSever.setChunkedSize(chunkedSize);
                                if (chunkedSize == 0)
                                {
                                    std::cout << "chunked fin " << std::endl;
                                    currSever.setStatus(CHUNKED_FIN);
                                    continue ;
                                }
                                std::cout <<"origin :" << clients[curr_event->ident] <<std::endl;
                                clients[curr_event->ident].erase(0, findIdx+2);
                                std::cout <<"sub :" << clients[curr_event->ident] <<std::endl;
                                // for (int i = 0 ; i <clients[curr_event->ident].size();i++ )
                                //     printf("c: %d\n",clients[curr_event->ident][i]);
                                currSever.getRequestClass().addBody(clients[curr_event->ident]);
                                // int currSize = clients[curr_event->ident].size();
                                // currSever.setCurrChunkedSize(currSize);
                                clients[curr_event->ident] = "";
                                // if (chunkedSize == currSize)//한방에 끝남?
                                std::cout << "body size1 :::"  << currSever.getRequestClass().getBody().size() << std::endl;
                                std::cout << "chubnke size1 :::"  << currSever.getChunkedSize() << std::endl;
                                if (chunkedSize == currSever.getRequestClass().getBody().size())
                                {
                                    currSever.setStatus(CHUNKED_DONE);
                                }
                                else
                                    currSever.setStatus(CHUNKED_ALIVE);
                            }
                            else if (currSever.getStatus() == CHUNKED_ALIVE)
                            {
                                std::cout << " allive" << std::endl;
                                currSever.getRequestClass().addBody(clients[curr_event->ident].substr(0,clients[curr_event->ident].find("\r\n")));
                                std::cout << "body size2 :::"  << currSever.getRequestClass().getBody().size() << std::endl;
                                std::cout << "chubnke size2 :::"  << currSever.getChunkedSize() << std::endl;
                                clients[curr_event->ident] = "";
                                if (currSever.getChunkedSize() == currSever.getRequestClass().getBody().size())
                                    currSever.setStatus(CHUNKED_DONE);
                                // else
                                //     currSever.setStatus(CHUNKED_ALIVE);
                            }
                            // if ((이전)메세지다읽었다 플래그)
                            // {
                            //     첫번째줄 자르기
                            //     사이즈정보가져와요
                            //     이번메세지가 사이즈정보 만큼 다 들어왓니?     아니요-> 아직 읽고있어요 플래그 : 바디에붙이기, 이번에 읽은사이즈 저장, 클라이언트[컬->아이덴트] 널로 바꾸기
                            //                                             ->그런데 왠걸 0\r\n이 들어왓네? 바로 청크드 던 플래그로 바꿔버려
                            //                                         네-> 바디에 냅다 저장(해당메세지 다읽었어요 플래그로 변경)
                            // }
                            // if (clients[curr_event->ident] == "0\r\n")
                            // {
                            //     currSever.setStatus();
                            // }
                        }
                        else if (clients[curr_event->ident].find("\r\n\r\n") != std::string::npos)
                        {   
                            currSever.getRequestClass().parseRequestMessage(clients[curr_event->ident]);
                            // std::cout << "body 1024 under : " << clients[curr_event->ident] << std::endl;
                            std::cout << "body 1024 under : " << currSever.getRequestClass().getBody() << std::endl;
                            std::map <std::string, std::string>::iterator chunkedIter = currSever.getRequestClass().getHeader().getContent().find("Transfer-Encoding");
                            clients[curr_event->ident] = "";
                            if (chunkedIter != currSever.getRequestClass().getHeader().getContent().end())
                                for (int i = 0 ; i < chunkedIter->second.size() ; i++)
                                    printf("c:%d\n",chunkedIter->second[i]);
                                // std::cout << "|" << chunkedIter->second<< "|" <<std::endl;
                            if (chunkedIter != currSever.getRequestClass().getHeader().getContent().end() && chunkedIter->second == "chunked\r")
                            {
                                std::cout << "chuenkkkkkkk\n";
                                if (currSever.getRequestClass().getBody().size() <= 0)
                                    currSever.setStatus(CHUNKED);
                                else//바디사이즈가 0 이상이면 청크드 정보 저장 이랑  첫줄날리고 청크드가아니라 청크드 던
                                {
                                    int findIdx = currSever.getRequestClass().getBody().find("\r\n");
                                    std::string firstLine = currSever.getRequestClass().getBody().substr(0, findIdx);
                                    long chunkedSize = std::strtol(firstLine.c_str(), NULL, 16);
                                    std::cout << "chunkedSize : " << chunkedSize  << std::endl;
                                    currSever.setChunkedSize(chunkedSize);
                                    if (chunkedSize == 0)
                                    {
                                        std::cout << "chunked fin " << std::endl;
                                        currSever.setStatus(CHUNKED_FIN);
                                        continue ;
                                    }
                                    std::cout <<"origin :" << currSever.getRequestClass().getBody() <<std::endl;
                                    std::string tmpBody = currSever.getRequestClass().getBody();
                                    currSever.getRequestClass().setBody(tmpBody.erase(0,findIdx+2));
                                    std::cout <<"sub :" << currSever.getRequestClass().getBody() <<std::endl;
                                    // currSever.getRequestClass().addBody(currSever.getRequestClass().getBody());
                                    int currSize = currSever.getRequestClass().getBody().size();
                                    currSever.setCurrChunkedSize(currSize);
                                    std::cout << "body size3 :::"  << currSize << std::endl;
                                    std::cout << "chubnke size3 :::"  << chunkedSize << std::endl;
                                    if (chunkedSize == currSize)//한방에 끝남?
                                        currSever.setStatus(CHUNKED_DONE);
                                    else
                                        currSever.setStatus(CHUNKED_ALIVE);
                                }
                            }
                            else
                                currSever.setStatus(DONE);
                        }
                    }
                }
            }
            else if (curr_event->filter == EVFILT_WRITE)
            {
                if (clients.find(curr_event->ident)!= clients.end())
                {
                    // std::cout << "find if" <<std::endl;
                    static int a = 0;
                    Server &currSever = serverMap[clientsServerMap[curr_event->ident]];
                    if (currSever.getStatus() == DONE)
                    {
                        // std::cout << "done if" <<std::endl;
                        // std::string tmp = clients[curr_event->ident];
                        std::map <std::string, std::string>::iterator findIter = currSever.getRequestClass().getHeader().getContent().find("Content-Length");
                        
                        if (findIter != currSever.getRequestClass().getHeader().getContent().end())//길이헤더 찾았을때
                        {
                            // std::cout << "length if" <<std::endl;
                            // std::cout << findIter->second << std::endl;
                            // std::cout << "bodysize:" << currSever.getRequestClass().getBody().size() << std::endl;
                            
                            if (std::atoi(findIter->second.c_str()) == currSever.getRequestClass().getBody().size())//바디사이즈까지 같을때
                            {
                                // std::cout << "body same if" <<std::endl;
                                serverMap[clientsServerMap[curr_event->ident]].processMethod();
                                std::string ResponseMessage = serverMap[clientsServerMap[curr_event->ident]].getResponseClass().writeResponseMessage();
                                std::cout << "write1" <<std::endl;
                                if (write(curr_event->ident, ResponseMessage.c_str(), ResponseMessage.size()) == -1)
                                {
                                    printErr("client write err");
                                    disconnect_client(curr_event->ident, clients, clientsServerMap);
                                }
                                else
                                {
                                    clients[curr_event->ident].clear();
                                    clientsServerMap.erase(curr_event->ident);
                                }
                                currSever.setStatus(READY);
                            }
                            //아무고토안함
                            // std::cout << "nothing" <<std::endl;
                        }
                        else//못찾았을때인데 헤더파싱은 끝나야함
                        {
                            // std::cout << "not and header done" <<std::endl;
                            serverMap[clientsServerMap[curr_event->ident]].processMethod();
                            std::string ResponseMessage = serverMap[clientsServerMap[curr_event->ident]].getResponseClass().writeResponseMessage();
                                          std::cout << "write2" <<std::endl;      
                            if ( write(curr_event->ident, ResponseMessage.c_str(), ResponseMessage.size()) == -1)
                            {
                                printErr("client write err");
                                disconnect_client(curr_event->ident, clients, clientsServerMap);
                            }
                            else
                                {
                                    clients[curr_event->ident].clear();
                                    clientsServerMap.erase(curr_event->ident);
                                }
                                currSever.setStatus(READY);
                        }
                    }
                    else if (currSever.getStatus() == CHUNKED_FIN)
                    {
                        // std::cout << "body same if" <<std::endl;
                        // int findRet = currSever.getRequestClass().getBody().find_last_of("0\r\n");
                        // if (findRet != std::string::npos)
                        // {
                        serverMap[clientsServerMap[curr_event->ident]].processMethod();
                        std::string ResponseMessage = serverMap[clientsServerMap[curr_event->ident]].getResponseClass().writeResponseMessage();
                        std::cout << "write3" <<std::endl;
                        if (write(curr_event->ident, ResponseMessage.c_str(), ResponseMessage.size()) == -1)
                        {
                            printErr("client write err");
                            disconnect_client(curr_event->ident, clients, clientsServerMap);
                        }
                        else
                        {
                            clients[curr_event->ident].clear();
                            clientsServerMap.erase(curr_event->ident);
                        }
                        currSever.setStatus(READY);
                        // }
                        
                    }
                    // currSever.setStatus(READY);
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