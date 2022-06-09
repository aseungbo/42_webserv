#include "WebServer.hpp"
#include <time.h>
			// std::vector<Server> servers;
			// // std::vector < int > serverFd; //자료형 고려
			// std::string confPath;
			// std::map < int, Server > serverMap; // 서버에서 뭘 받아온다면 다시 고려 
			// // EventHandler // 언젠가 꼭 고려해보쟈 ><
			
			std::vector<std::string > makeChunkedVec(std::string originStr);

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


void disconnect_client(int client_fd, std::map<int, std::string>& clients, std::map<int, int> &clientsServerMap)
{
    std::cout << "client disconnected: " << client_fd << std::endl;
    close(client_fd);
    clients.erase(client_fd);
    clientsServerMap.erase(client_fd);
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

    clock_t start, finish;
    double duration;
    
    start = clock();
 
	int kq;
    if ((kq = kqueue()) == -1)
        printErr("kqueue error");
        
    std::map<int, std::string> clients;
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
    
	int requestCnt = 0;
    while (1)
    {
        /*  apply changes and return new events(pending events) */
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, serverMap.size(), NULL);
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
                // std::map<int, Server>::iterator serverMap.find(curr_event->ident) = ;
                if (fdManager.find(curr_event->ident) != fdManager.end())
                {
                    std::cout << "[Read] curr ident: " << curr_event->ident << std::endl;
                    Server &currServer = serverMap[fdManager[curr_event->ident]];
                    if (currServer.getCurrLocation().getLocationType() == LOCATIONTYPE_NORMAL || currServer.getCurrLocation().getLocationType() == LOCATIONTYPE_CGI_DONE)
                        currServer.readFile(curr_event->ident);//일반 get 메소드 
                    else
                    {
                        //read
                        static std::string chunkedStr = "";
                        if (currServer.getCgiPid() != 0)
                        {
                            std::cout << "[parent]" << std::endl;

                            std::string body;
                            int n;
                            char buf[65000];
                            //exe 실행후
                            while ((n = read(currServer.getReadFd()[0], buf,64999)) > 0)
                            {
                                std::cout<< "n:" << n <<std::endl;
                                buf[n] = '\0';
                                body += buf;
                                // TODO : 클러스터에서 확인해보기 >< 꾸?
                                // if (body.find("\r\n") != std::string::npos)
                                //     break ;
                                
                                memset(buf, 0, 65000);
                            }
                            // std::cout << "execl buf: " << body <<std::endl;
                            // body =  "cgi done " + body;
                            chunkedStr += body;
                            // currServer.getRequestClass().addBody(body);
                            // currServer.getResponseClass().setBody(body.substr(body.find("\r\n\r\n") + 4));
                            // currServer.getResponseClass().addBody(body.substr(body.find("\r\n\r\n") + 4));
                            // currServer.getResponseClass().setBody(currServer.getRequestClass().getBody());
                            
                            
                            // waitpid(currServer.getCgiPid(), NULL, WNOHANG);
                            close(currServer.getWriteFd()[0]);
                            close(currServer.getReadFd()[1]);
                            // wait(pid,NULL,0);
                        }
                        std::cout << "!@#$%^ size: " <<chunkedStr.size()<<std::endl;
                        if (currServer.getRequestClass().getBody().size() <= chunkedStr.size())
                        {
                            std::cout << "hi!"<< chunkedStr << std::endl;
                            currServer.getRequestClass().setBody(chunkedStr.substr(chunkedStr.find("\r\n\r\n") + 4));
                            currServer.getResponseClass().setBody(chunkedStr.substr(chunkedStr.find("\r\n\r\n") + 4));
                            currServer.getCurrLocation().setLocationType(LOCATIONTYPE_CGI_DONE);
                            // std::cout << "204 type:" << currServer.getCurrLocation().getLocationType()<<std::endl;;
                            std::map <std::string, std::string>::iterator findIter = currServer.getRequestClass().getHeader().getContent().find("Content-Length");
                            currServer.setStatus(DONE);
                            fcntl(curr_event->ident, F_SETFL, O_NONBLOCK);//넌 이제부터 논블로킹이야
                            change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL); // add event
                            if (findIter != currServer.getRequestClass().getHeader().getContent().end())//길이헤더 찾았을때
                                findIter->second = std::to_string(currServer.getRequestClass().getBody().size());
                            fdManager.erase(curr_event->ident);
                            waitpid(currServer.getCgiPid(), NULL, WNOHANG);
                        }
                        // std::cout << " cgi dooooooon \n";
                    }
                }
                else if (clients.find(curr_event->ident) != clients.end())
                {
                    // parse request
                    char buf[65536];
                    memset(buf,0,65536);
                    int n = read(curr_event->ident, buf, sizeof(buf) - 1);
                    // std::cout << "[ after read ]" << std::endl;
                    if (n <= 0)
                    {
                        if (n < 0)
                            printErr("client read error!");
                        std::cout << "read:diconnect call" <<std::endl;
                        // serverMap[clientsServerMap[curr_event->ident]].setStatus(READY);
                        disconnect_client(curr_event->ident, clients, clientsServerMap);
                    }
                    else
                    {
                        // std::cout << "after read n:" << n <<"fd:: " << curr_event->ident<< std::endl;
                        buf[n] = '\0';
                        clients[curr_event->ident] += buf;
                        Server &currSever = serverMap[clientsServerMap[curr_event->ident]];//TODO::  청크드 로직에서 메세지의 끝은 rn 인데 우리는 rn이고 뭐고 그냥 때려 넣음
                        if (currSever.getStatus() == DONE)
                        {
                            // std::cout << "done add body if"<< std::endl;
                            // std::cout << "done add body if  before:" << currSever.getRequestClass().getBody()<< std::endl;
                            // currSever.getRequestClass().addBody(buf);
                            currSever.getRequestClass().addBody(clients[curr_event->ident]);
                            // std::cout << "done add body if  after:" <<currSever.getRequestClass().getBody()<< std::endl;
                        }
                        else if (currSever.getStatus() == CHUNKED)
                        {
                            currSever.getRequestClass().addBody(clients[curr_event->ident]);
                            std::cout << "size: " << currSever.getRequestClass().getBody().size() << std::endl;
                            clients[curr_event->ident] = "";
                            if ( checkLastChunked(currSever.getRequestClass().getBody()) )//
                            {
                                // 바디 rn기준으로 잘라넣을 거 추가;
                                currSever.parseChunkedBody();
                                currSever.setStatus(CHUNKED_FIN);
                                change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            }
                            else//지워도됨
                                currSever.setStatus(CHUNKED);
                        }
                        else if (clients[curr_event->ident].find("\r\n\r\n") != std::string::npos)
                        {
                            // std::cout << "header if "<< std::endl;
                            currSever.getRequestClass().parseRequestMessage(clients[curr_event->ident]);
                            // std::cout << "body 1024 under : " << clients[curr_event->ident] << std::endl;
                            // std::cout << "body 1024 under :" << currSever.getRequestClass().getBody() << ")"<< std::endl;
                            std::map <std::string, std::string>::iterator chunkedIter = currSever.getRequestClass().getHeader().getContent().find("Transfer-Encoding");
                            clients[curr_event->ident] = "";
                            if (chunkedIter != currSever.getRequestClass().getHeader().getContent().end() && chunkedIter->second == "chunked")
                            {
                                if(currSever.getRequestClass().getBody().find("0\r\n\r\n") != std::string::npos)
                                {
                                    // 바디 rn기준으로 잘라넣을 거 추가;
                                    currSever.parseChunkedBody();
                                    currSever.setStatus(CHUNKED_FIN);
                                    change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                                }
                                else
                                    currSever.setStatus(CHUNKED);
                            }
                            else
                            {
                                currSever.setStatus(DONE);
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
                        usleep(10);
                    }
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                    std::cout << "new accept : "<< clientSocket <<  std::endl;
                    change_events(change_list, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    // change_events(change_list, clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clients[clientSocket] = "";
                    clientsServerMap[clientSocket] = serverSocket;   
                }
                 
                
                // else if ()
            }
            else if (curr_event->filter == EVFILT_WRITE)
            {
                if (fdManager.find(curr_event->ident) != fdManager.end())
                {
                    static int writeSize = 0;
                    // Server &currServer = serverMap[fdManager[curr_event->ident]];
                    // std::cout << "type :: "<< serverMap[fdManager[curr_event->ident]].getCurrLocation().getLocationType() <<std::endl;;
                    if (serverMap[fdManager[curr_event->ident]].getCurrLocation().getLocationType() == LOCATIONTYPE_CGI)//cgi처리로직 조건
                    {
                        std::cout << "size:::" << serverMap[fdManager[curr_event->ident]].getRequestClass().getBody().size()<<std::endl;
                        int n;
                        if (( n = write(curr_event->ident, serverMap[fdManager[curr_event->ident]].getRequestClass().getBody().c_str(), serverMap[fdManager[curr_event->ident]].getRequestClass().getBody().size())) > 0)
                        {
                            writeSize += n;
                            std::cout << "write size:" << serverMap[fdManager[curr_event->ident]].getRequestClass().getBody().size() <<std::endl;
                            //TODO : pid 초기값 -1
                            if (serverMap[fdManager[curr_event->ident]].getCgiPid() < 0)
                                serverMap[fdManager[curr_event->ident]].forkCgiPid();
                            if (serverMap[fdManager[curr_event->ident]].getCgiPid() == 0)
                            {
                                std::string body;
                                char buf[1024];

                                dup2(serverMap[fdManager[curr_event->ident]].getReadFd()[1],1);
                                close(serverMap[fdManager[curr_event->ident]].getReadFd()[0]);
                                dup2(serverMap[fdManager[curr_event->ident]].getWriteFd()[0],0);
                                close(serverMap[fdManager[curr_event->ident]].getWriteFd()[1]);
                                
                                char *test[2] ;
                                test[0] = (char *)(serverMap[fdManager[curr_event->ident]].getCurrLocation().getCgiPath().c_str());
                                test[1] = NULL;
                                if((execve(serverMap[fdManager[curr_event->ident]].getCurrLocation().getCgiPath().c_str(),test, serverMap[fdManager[curr_event->ident]].makeEnvp(serverMap[fdManager[curr_event->ident]].getRequestClass().getBody().size()))) == -1 )
                                {
                                    write(serverMap[fdManager[curr_event->ident]].getReadFd()[1], "errororor\n", 11);
                                    exit(1);
                                }
                                exit(1);
                            }
                            serverMap[fdManager[curr_event->ident]].setFdManager(serverMap[fdManager[curr_event->ident]].getReadFd()[0], serverMap[fdManager[curr_event->ident]].getServerFd());
                            change_events(change_list, serverMap[fdManager[curr_event->ident]].getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            // fcntl(serverMap[fdManager[curr_event->ident]].getReadFd()[0], F_SETFL, O_NONBLOCK);
                            
                            // // if () //FD 다 쓴곳에서 다같이 삭제하는것도 방법임
                            //     fdManager.erase(curr_event->ident);
                            if (writeSize ==serverMap[fdManager[curr_event->ident]].getRequestClass().getBody().size() )
                            {
                            serverMap[fdManager[curr_event->ident]].setFdManager(serverMap[fdManager[curr_event->ident]].getReadFd()[0], serverMap[fdManager[curr_event->ident]].getServerFd());
                            change_events(change_list, serverMap[fdManager[curr_event->ident]].getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            std::cout << "read[fd]: " << serverMap[fdManager[curr_event->ident]].getReadFd()[0] << std::endl;
                            fdManager.erase(curr_event->ident);
                            std::cout << " hyopark is very cold"<<std::endl;
                            }
                        }
                        else
                        {
                            serverMap[fdManager[curr_event->ident]].setFdManager(serverMap[fdManager[curr_event->ident]].getReadFd()[0], serverMap[fdManager[curr_event->ident]].getServerFd());
                            change_events(change_list, serverMap[fdManager[curr_event->ident]].getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            std::cout << "read[fd]: " << serverMap[fdManager[curr_event->ident]].getReadFd()[0] << std::endl;
                            fdManager.erase(curr_event->ident);
                            std::cout << " hyopark is very cold"<<std::endl;
                        }
                    }
                    else if (serverMap[fdManager[curr_event->ident]].getCurrLocation().getLocationType() == LOCATIONTYPE_CGI_DONE || serverMap[fdManager[curr_event->ident]].getCurrLocation().getLocationType() == LOCATIONTYPE_NORMAL)
                    {
                        serverMap[fdManager[curr_event->ident]].writeFile(curr_event->ident);
                        fdManager.erase(curr_event->ident);
                    }
                    // continue;
                    
                }
                else if (clients.find(curr_event->ident)!= clients.end())
                {
                    Server &currSever = serverMap[clientsServerMap[curr_event->ident]];

                //  std::cout << currSever.getStatus() <<std::endl;
                    if (currSever.getStatus() == DONE)
                    {
                        // std::cout << "2\n";
                        std::map <std::string, std::string>::iterator findIter = currSever.getRequestClass().getHeader().getContent().find("Content-Length");
                        // std::cout << "done body : "  << currSever.getRequestClass().getBody();
                        // std::cout << "done body : "  << currSever.getRequestClass().getBody();
                        
                        if (findIter != currSever.getRequestClass().getHeader().getContent().end())//길이헤더 찾았을때
                        {   
                            // std::cout << "3\n";
                            if (std::atoi(findIter->second.c_str()) == currSever.getRequestClass().getBody().size())//바디사이즈까지 같을때
                            {
                                // std::cout << "4\n";
                                // if (currSever.getCurrLocation().getLocationType() != LOCATIONTYPE_CGI_DONE)
                                serverMap[clientsServerMap[curr_event->ident]].preProcess(currSever.getCurrLocation().getLocationType());
                                serverMap[clientsServerMap[curr_event->ident]].processMethod(change_list);
                                currSever.setStatus(READY);
                            }
                        }
                        else//못찾았을때인데 헤더파싱은 끝나야함
                        {
                            serverMap[clientsServerMap[curr_event->ident]].preProcess(currSever.getCurrLocation().getLocationType());
                            serverMap[clientsServerMap[curr_event->ident]].processMethod(change_list);
                            currSever.setStatus(READY);
                        }
                    }
                    else if (currSever.getStatus() == CHUNKED_FIN)
                    {
                        
                        serverMap[clientsServerMap[curr_event->ident]].preProcess(currSever.getCurrLocation().getLocationType());
                        serverMap[clientsServerMap[curr_event->ident]].processMethod(change_list);
                        currSever.setStatus(READY);
                        
                    }
                    else if (currSever.getResponseClass().getStatusCode() != 0)
                    {
                        std::string ResponseMessage = serverMap[clientsServerMap[curr_event->ident]].getResponseClass().writeResponseMessage();
                        // std::cout << "write3" <<std::endl;
                        // std::cout << "msf: " << ResponseMessage << std::endl;
                        // std::cout << "size" << ResponseMessage.size() << std::endl;
                        // std::cout << "curr identttttt: " << curr_event->ident << std::endl;
                        if (write(curr_event->ident, ResponseMessage.c_str(), ResponseMessage.size()) == -1)
                        {
                            printErr("client write err");
                            disconnect_client(curr_event->ident, clients, clientsServerMap);
                        }
                        else
                        {
                            clients[curr_event->ident].clear();
                            clientsServerMap.erase(curr_event->ident);
                            currSever.getResponseClass().setStatusCode(0);
                        }
                        currSever.resetServerValues();
                        //지우기 Server : curr들 초기화 initServerCurrResponseAndRequestAndLocation

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