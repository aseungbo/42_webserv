#include "WebServer.hpp"
			// std::vector<Server> servers;
			// // std::vector < int > serverFd; //자료형 고려
			// std::string confPath;
			// std::map < int, Server > serverMap; // 서버에서 뭘 받아온다면 다시 고려 
			// // EventHandler // 언젠가 꼭 고려해보쟈 ><


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
	// hyopark 유언
    std::string config;
    
    config = openConfigfile(this->confPath);
    if(config.empty() == 1)
		printErr("Can not open file.");
    makeServers(this->servers, config);
    exit(0);

    // func makeServer
	Server server;
	Server server2;
	
	this->servers.push_back(server);
	// parse 후 server에 할당
	std::vector<std::string> host;
	host.push_back("127.0.0.1");
	this->servers[0].setHost(host);
	
	int port;
	// port.push_back(80);
	this->servers[0].setPorts(8080);
	
	// this->servers[0].setLocation(Location(""));
	this->servers[0].setClientBodySize(1000);
	// this->servers[0].setErrPage();
	// std::vector<Server> servers;
	// std::vector < int > serverFd; //자료형 고려
	// std::string confPath;
	
	
	// only for test
	this->servers.push_back(server2);
	std::vector<std::string> host2;
	
	host2.push_back("127.0.0.1");
	this->servers[1].setHost(host2);
	
	this->servers[1].setPorts(8081);
	this->servers[1].setClientBodySize(1000);
}
// this->servers.push_back()

// kqueue 생성 
void WebServer::makeKqueue()
{
	
}

void WebServer::listenServers()
{
	for (int idx = 0; idx < this->servers.size() ; idx++)
	{
		struct sockaddr_in serverAddr;
		int serverSocketFD = socket(PF_INET, SOCK_STREAM, 0);
		if (serverSocketFD == -1)	
			printErr("Socket error");
		
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
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

void disconnect_client(int client_fd, std::map<int, std::string>& clients)
{
    std::cout << "client disconnected: " << client_fd << std::endl;
    close(client_fd);
    clients.erase(client_fd);
}

void WebServer::monitorKqueue()
{
	int kq;
    if ((kq = kqueue()) == -1)
        printErr("kqueue error");
        
    std::map<int, std::string> clients;
    std::vector <struct kevent> change_list;
    struct kevent event_list[8];
    
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
        std::cout << "Hello JJIDRAGON WORLD" << std::endl;
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
        if (new_events == -1)
            printErr("kevent() error\n");

        change_list.clear(); // clear change_list for new changes
		std::cout << "new_events : " << new_events<<std::endl;
        for (int i = 0; i < new_events; ++i)
        {
            curr_event = &event_list[i];
            /* check error event return */
            if (curr_event->flags & EV_ERROR)
            {
                std::map<int, Server>::iterator serverIter = serverMap.find(curr_event->ident);
                if (curr_event->ident == serverIter->first)
                    printErr("server socket error");
                else
                {
                    printErr("client socket error");
                    // disconnect_client(curr_event->ident, clients);
                }
            }
            else if (curr_event->filter == EVFILT_READ)
            {
				
				requestCnt++;
				std::cout << "==========================" << std::endl;
				std::cout << "requestCnt: " << requestCnt << std::endl;
				std::cout << "==========================" << std::endl;
                // if (curr_event->ident == serverSocket)
                // map indexing으로 접근 가능한지 확인해볼 것
                std::map<int, Server>::iterator serverIter = serverMap.find(curr_event->ident);
                if (serverIter != serverMap.end())
                {
                    /* accept new client */
                    int clientSocket = 0;
                    int serverSocket = serverIter->first;
                    if ((clientSocket = accept(serverSocket, NULL, NULL)) == -1)
                        printErr("accept() error\n");
                    // serverMap[curr_event->ident].setClientSocket(clientSocket);
                    std::cout << "accept new client: " << clientSocket << std::endl;
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK);

                    /* add event for client socket - add read && write event */
                    change_events(change_list, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    change_events(change_list, clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clients[clientSocket] = "";
                }
                else if (clients.find(curr_event->ident)!= clients.end())
                {
                    /* read data from client */
                    char buf[1024];
                    int n = read(curr_event->ident, buf, sizeof(buf));
                    
                    // this-> server.request = Request;
                    if (n <= 0)
                    {
                        if (n < 0)
                            printErr("client read error!");
                        disconnect_client(curr_event->ident, clients);
                    }
                    else
                    {
                        // parse request
                        buf[n] = '\0';
                        clients[curr_event->ident] += buf;
                        serverMap[curr_event->ident].getRequestClass().parseRequestMessage(clients[curr_event->ident]);
                        serverMap[curr_event->ident].processMethod();
                    }
                }
            }
            else if (curr_event->filter == EVFILT_WRITE)
            {
                if (clients.find(curr_event->ident)!= clients.end())
                {
                    int n;
                    // std::string ResponseMessage = serverMap[curr_event->ident].getResponseClass().writeResponseMessage();
                    std::string ResponseMessage = "HTTP/1.1 200 GOOD\r\nDate: a\r\nServer: a\r\nLast-Modified: a\r\nETag: 'A'\r\nAccept-Ranges: bytes\r\nContent-Length: 6\r\nConnection: close\r\nContent-Type: text/html\r\n\n<h1>My page</h1>";
					                    
                    if ((n = write(curr_event->ident, ResponseMessage.c_str(), ResponseMessage.size())) == -1)
                    {
                        printErr("client write err");
                        disconnect_client(curr_event->ident, clients);
                    }
                    disconnect_client(curr_event->ident, clients);
                }
            }
        }
    }
    return ;
}
    
	


// void WebServer::monitorKqueue()
// {
// 	while(42)
// 	{
// 		for (std::map<int, Server>::iterator iter = this->serverMap.begin(); iter != this->serverMap.end(); iter++)
// 		{
// 			struct sockaddr_in clntAddr;
// 			socklen_t clntAddrSize = sizeof(clntAddr);
// 			int clientSocket;
			
// 			// std::cout << "first: " << iter->first << " second: " << iter->second.getPort() << std::endl;
// 			if ((clientSocket = accept(iter->first, (struct sockaddr *)&clntAddr, &clntAddrSize)) != -1)
// 			{
// 				char buf[512];
// 				std::string str;
// 				int n;
				
// 				// n = read_data(clientSocket, buf, 1024);
				
// 				str.clear();
// 				while (1)
// 				{
// 					if (read(clientSocket, &buf, sizeof(buf)) > 0)
// 					{
// 						str = buf;
// 					while((n = read(clientSocket, &buf, sizeof(buf))) > 0)
// 					{
// 						str += buf;
// 					}
// 					}
// 					// std::cout << n << std::endl;
// 					// std::cout << "===========" << std::endl;
// 					std::cout << "[" << buf << "]" << std::endl; 
// 					std::cout << "{" << str << "}" << std::endl;
// 					// std::cout << "===========" << std::endl;
// 					break;
// 				}
// 				std::cout << "====== end ======" << std::endl;
// 				std::cout << str << std::endl;
// 				exit(0);
// 			}
// 			// std::cout << "Accept new client: " << clientSocket << std::endl;
// 			fcntl(clientSocket, F_SETFL, O_NONBLOCK);
			
// 		}	
// 	}
// }
//->안의 기능은 따로 뺄예정

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