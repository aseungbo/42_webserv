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
		if (listen(serverSocketFD, 200) == -1)
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

    clock_t start, finish;
    double duration;
    
    start = clock();
 
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
    
	int requestCnt = 0;
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
                // std::map<int, Server>::iterator serverMap.find(curr_event->ident) = ;
                if (fdManager.find(curr_event->ident) != fdManager.end())
                {
                    std::cout << "client sockke:" << fdManager[curr_event->ident] << std::endl;
                    std::cout << "server sockke:" << clientsServerMap[fdManager[curr_event->ident]] << std::endl;
                    Server &currServer = serverMap[clientsServerMap[fdManager[curr_event->ident]]];
                    std::cout << "[Read] curr ident: " << curr_event->ident << std::endl;
                    std::cout << "locatype " << currServer.getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getLocationType() << std::endl;
                    if (currServer.getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getLocationType() == LOCATIONTYPE_NORMAL || currServer.getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getLocationType() == LOCATIONTYPE_CGI_DONE)
                    {
                        currServer.getClientMap()[fdManager[curr_event->ident]].readFile(curr_event->ident);//일반 get 메소드 
                        std::cout <<"fd:" <<fdManager[curr_event->ident]<<"statecod " <<currServer.getClientMap()[fdManager[curr_event->ident]].getResponseClass().getStatusCode()<<std::endl;
                        change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL); // add event
                        fdManager.erase(curr_event->ident);//씨지아이던은 따로해야할지두?
                        // std::cout << "hell"<<currServer.getClientMap()[fdManager[curr_event->ident]].getResponseClass().getStatusCode()<<std::endl;
                        // std::cout << "fdManager[curr_event->ident]"<<fdManager[curr_event->ident]<<std::endl;
                    }
                    else
                    {
                        //read
                        // static std::string chunkedStr = "";
                        if (currServer.getClientMap()[fdManager[curr_event->ident]].getCgiPid() != 0)
                        {
                            std::cout << "[parent]" << std::endl;
                            

                            std::string body;
                            int n;
                            char buf[65536];
                            //exe 실행후
                            while ((n = read(currServer.getClientMap()[fdManager[curr_event->ident]].getReadFd()[0], buf,65535)) > 0)
                            {
                                std::cout<< "n:" << n <<std::endl;
                                buf[n] = '\0';
                                body += buf;
                                // TODO : 클러스터에서 확인해보기 >< 꾸?
                                
                                memset(buf, 0, 65000);
                            }
                            currServer.getClientMap()[fdManager[curr_event->ident]].addChunkedStr(body);
                            // std::cout << "execl buf: " << body <<std::endl;
                            // body =  "cgi done " + body;
                            // chunkedStr += body;
                            
                            // waitpid(currServer.getCgiPid(), NULL, WNOHANG);
                            // wait(pid,NULL,0);
                            close(currServer.getClientMap()[fdManager[curr_event->ident]].getWriteFd()[0]);
                            close(currServer.getClientMap()[fdManager[curr_event->ident]].getReadFd()[1]);
                        }
                        std::cout << "!@#$%^ size: " <<currServer.getClientMap()[fdManager[curr_event->ident]].getChunkedStr().size()<<std::endl;
                        if (currServer.getClientMap()[fdManager[curr_event->ident]].getRequestClass().getBody().size() <= currServer.getClientMap()[fdManager[curr_event->ident]].getChunkedStr().size())//추후에 \r\n 이후거만 비교 해서 ==으로 수정
                        {
                            // std::cout << "hi!"<< currServer.getClientMap()[fdManager[curr_event->ident]].getChunkedStr() << std::endl;
                            currServer.getClientMap()[fdManager[curr_event->ident]].getRequestClass().setBody(currServer.getClientMap()[fdManager[curr_event->ident]].getChunkedStr().substr(currServer.getClientMap()[fdManager[curr_event->ident]].getChunkedStr().find("\r\n\r\n") + 4));
                            currServer.getClientMap()[fdManager[curr_event->ident]].getResponseClass().setBody(currServer.getClientMap()[fdManager[curr_event->ident]].getChunkedStr().substr(currServer.getClientMap()[fdManager[curr_event->ident]].getChunkedStr().find("\r\n\r\n") + 4));
                            currServer.getClientMap()[fdManager[curr_event->ident]].getResponseClass().chunkedVec = makeChunkedVec(currServer.getClientMap()[fdManager[curr_event->ident]].getChunkedStr().substr(currServer.getClientMap()[fdManager[curr_event->ident]].getChunkedStr().find("\r\n\r\n") + 4));
                            currServer.getClientMap()[fdManager[curr_event->ident]].getCurrLocation().setLocationType(LOCATIONTYPE_CGI_DONE);
                            // std::cout << "204 type:" << currServer.getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getLocationType()<<std::endl;;
                            std::map <std::string, std::string>::iterator findIter = currServer.getClientMap()[fdManager[curr_event->ident]].getRequestClass().getHeader().getContent().find("Content-Length");
                            currServer.getClientMap()[fdManager[curr_event->ident]].setStatus(DONE);
                            // fcntl(curr_event->ident, F_SETFL, O_NONBLOCK);//넌 이제부터 논블로킹이야
                            // change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL); // add event
                            if (findIter != currServer.getClientMap()[fdManager[curr_event->ident]].getRequestClass().getHeader().getContent().end())//길이헤더 찾았을때
                                findIter->second = std::to_string(currServer.getClientMap()[fdManager[curr_event->ident]].getRequestClass().getBody().size());
                            currServer.getClientMap()[fdManager[curr_event->ident]].setChunkedStr("");
                            waitpid(currServer.getClientMap()[fdManager[curr_event->ident]].getCgiPid(), NULL, WNOHANG);
                            fdManager.erase(curr_event->ident);//erase하나 더해야함
                        }
                        // std::cout << " cgi dooooooon \n";
                        
                    }
                }
                else if (clientsServerMap.find(curr_event->ident) != clientsServerMap.end() && serverMap[clientsServerMap[curr_event->ident]].getClientMap().find(curr_event->ident) != serverMap[clientsServerMap[curr_event->ident]].getClientMap().end())
                {
                    // if ()
                    // if ( serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getServerStatus() != SERVER_READY)
                    // {
                    //     std::cout << "다음기회에!"<<std::endl;
                    //     change_events(change_list, curr_event->ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    //     break;
                    // }
                    std::cout << "curr_event->ident" <<curr_event->ident<<std::endl;
                    std::cout <<"33"<<std::endl;
                    Server &currServer = serverMap[clientsServerMap[curr_event->ident]];
                    
                    std::cout <<"c" << currServer.getClientMap()[curr_event->ident].getServerStatus()<<std::endl;
                    char buf[65536];
                    memset(buf,0,65536);
                    int n = read(curr_event->ident, buf, sizeof(buf) - 1);
                    // std::cout << "[ after read ]" << std::endl;
                    if (n <= 0)
                    {
                        if (n < 0)
                            printErr("client read error!");
                        std::cout << "read:diconnect call" <<std::endl;
                        // currServer.setStatus(READY);
                        // currServer.getClientMap()[curr_event->ident].resetServerValues();
                        // clientsServerMap.erase(curr_event->ident);
                        disconnect_client(curr_event->ident, currServer, clientsServerMap);// 0612역시 여기서 냅다 초기화날려야할지두
                        
                        
                    }
                    else
                    {
                    
                        std::cout <<"44"<<std::endl;
                        // std::cout << "after read n:" << n <<"fd:: " << curr_event->ident<< std::endl;
                        buf[n] = '\0';
                        currServer.getClientMap()[curr_event->ident].getClientBody() += buf;
                        // Server &currSever = serverMap[clientsServerMap[curr_event->ident]];//TODO::  청크드 로직에서 메세지의 끝은 rn 인데 우리는 rn이고 뭐고 그냥 때려 넣음
                        std::cout <<"!"<<std::endl;
                        if (currServer.getClientMap()[curr_event->ident].getStatus() == DONE)
                        {
                            std::cout <<"2"<<std::endl;
                            // std::cout << "done add body if"<< std::endl;
                            // std::cout << "done add body if  before:" << currServer.getClientMap()[curr_event->ident].getRequestClass().getBody()<< std::endl;
                            // currServer.getClientMap()[curr_event->ident].getRequestClass().addBody(buf);
                            currServer.getClientMap()[curr_event->ident].setServerStatus(SERVER_ING);
                            currServer.getClientMap()[curr_event->ident].getRequestClass().addBody(currServer.getClientMap()[curr_event->ident].getClientBody());
                            currServer.getClientMap()[curr_event->ident].getClientBody().clear();//0612추가
                            // std::cout << "done add body if  after:" <<currServer.getClientMap()[curr_event->ident].getRequestClass().getBody()<< std::endl;
                        }
                        else if (currServer.getClientMap()[curr_event->ident].getStatus() == CHUNKED)
                        {
                            currServer.getClientMap()[curr_event->ident].getRequestClass().addBody(currServer.getClientMap()[curr_event->ident].getClientBody());
                            std::cout << "size: " << currServer.getClientMap()[curr_event->ident].getRequestClass().getBody().size() << std::endl;
                            currServer.getClientMap()[curr_event->ident].getClientBody().clear();
                            if ( checkLastChunked(currServer.getClientMap()[curr_event->ident].getRequestClass().getBody()) )//
                            {
                                // 바디 rn기준으로 잘라넣을 거 추가;
                                currServer.getClientMap()[curr_event->ident].parseChunkedBody();
                                currServer.getClientMap()[curr_event->ident].setStatus(CHUNKED_FIN);
                                currServer.getClientMap()[curr_event->ident].setServerStatus(SERVER_ING);
                                change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            }
                            else//지워도됨
                                currServer.getClientMap()[curr_event->ident].setStatus(CHUNKED);
                        }
                        else if (currServer.getClientMap()[curr_event->ident].getClientBody().find("\r\n\r\n") != std::string::npos)
                        {
                            // std::cout << "header if "<< std::endl;
                            std::cout << "request header in ident:"<<curr_event->ident<<std::endl;
                            currServer.getClientMap()[curr_event->ident].getRequestClass().parseRequestMessage(currServer.getClientMap()[curr_event->ident].getClientBody());
                            // std::cout << "body 1024 under : " << currServer.getClientMap()[curr_event->ident].getClientBody() << std::endl;
                            // std::cout << "body 1024 under :" << currServer.getClientMap()[curr_event->ident].getRequestClass().getBody() << ")"<< std::endl;
                            std::map <std::string, std::string>::iterator chunkedIter = currServer.getClientMap()[curr_event->ident].getRequestClass().getHeader().getContent().find("Transfer-Encoding");
                            currServer.getClientMap()[curr_event->ident].getClientBody().clear();
                            if (chunkedIter != currServer.getClientMap()[curr_event->ident].getRequestClass().getHeader().getContent().end() && chunkedIter->second == "chunked")
                            {
                                if(currServer.getClientMap()[curr_event->ident].getRequestClass().getBody().find("0\r\n\r\n") != std::string::npos)
                                {
                                    // 바디 rn기준으로 잘라넣을 거 추가;
                                    currServer.getClientMap()[curr_event->ident].parseChunkedBody();
                                    currServer.getClientMap()[curr_event->ident].setStatus(CHUNKED_FIN);
                                    currServer.getClientMap()[curr_event->ident].setServerStatus(SERVER_ING);
                                    change_events(change_list, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                                }
                                else
                                    currServer.getClientMap()[curr_event->ident].setStatus(CHUNKED);
                            }
                            else
                            {
                                currServer.getClientMap()[curr_event->ident].setStatus(DONE);
                                currServer.getClientMap()[curr_event->ident].setServerStatus(SERVER_ING);
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
                    clientsServerMap[clientSocket] = serverSocket;
                    std::cout <<"!"<<std::endl;
                    serverMap[serverSocket].addClient(clientSocket);
                    // serverMap.find(curr_event->ident)->second.addClient(clientSocket);
                    std::cout <<"1"<<std::endl;
                }
                 
                
                // else if ()
            }
            else if (curr_event->filter == EVFILT_WRITE)
            {
                
                if (fdManager.find(curr_event->ident) != fdManager.end())
                {
                    static int writeSize = 0;
                    // if (serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getChunkedWriteSize() == 0)
                    //    serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setChunkedSize(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getRequestClass().getBody().size()) ;
                    
                    // Server &currServer = serverMap[fdManager[curr_event->ident]];
                    // std::cout << "type :: "<< serverMap[fdManager[curr_event->ident]].getCurrLocation().getLocationType() <<std::endl;;
                    if (serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getLocationType() == LOCATIONTYPE_CGI)//cgi처리로직 조건
                    {
                        std::string &currStr = serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getRequestClass().getBody();
                        std::cout << "size:::" << serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getRequestClass().getBody().size()<<std::endl;
                        int n;
                        if (( n = write(curr_event->ident ,currStr.c_str(), currStr.size())) > 0)
                        {
                            serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].addChunkedWriteSize(n);
                            writeSize += n;
                            
                            // serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getRequestClass().setBody(currStr.substr(n));
                            std::cout << "write size:" << serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getRequestClass().getBody().size() <<std::endl;
                            //TODO : pid 초기값 -1
                            if (serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getCgiPid() < 0)
                                serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].forkCgiPid();
                            if (serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getCgiPid() == 0)
                            {
                                std::string body;
                                char buf[1024];

                                dup2(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[1],1);
                                close(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0]);
                                dup2(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getWriteFd()[0],0);
                                close(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getWriteFd()[1]);
                                
                                char *test[2] ;
                                test[0] = (char *)(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getCgiPath().c_str());
                                test[1] = NULL;
                                if((execve(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getCgiPath().c_str(),test, serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].makeEnvp(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getRequestClass().getBody().size()))) == -1 )
                                {
                                    write(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[1], "errororor\n", 11);
                                    exit(1);
                                }
                                exit(1);
                            }
                            // serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0], serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getClientSocket());
                            std::cout << "먼지:" << serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getLocationType() <<std::endl;
                            std::cout << "혹쉬?:" << fdManager[curr_event->ident]<<std::endl;
                            std::cout << "혹쉬>>????:" << serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0]<<std::endl;
                            std::cout << "혹쉬!!!:" << fdManager[curr_event->ident]<<std::endl;
                            std::cout << "혹쉬!!!:" << fdManager[serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0]]<<std::endl;
                            // serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0]  ,serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]]   );
                            change_events(change_list, serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            
                            // fcntl(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getReadFd()[0], F_SETFL, O_NONBLOCK);
                            
                            // // if () //FD 다 쓴곳에서 다같이 삭제하는것도 방법임
                            //     fdManager.erase(curr_event->ident);
                            // std::cout << "writeSize:"<<writeSize<<", bodysize:"<<serverMap[clientsServerMap[fdManager[curr_event->ident]]].getRequestClass().getBody().size()<<std::endl;
                            if (writeSize >= serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getRequestClass().getBody().size() )
                            {
                                std::cout <<"cgi write end not else"<<std::endl;
                                // serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0], serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getServerFd());
                                // serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager( serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0],serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getClientSocket() );
                                // serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0],  serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getClientSocket() );
                                // serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0], serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getClientSocket());
                                change_events(change_list, serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                                
                                std::cout << "read[fd]: " << serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0] << std::endl;
                                std::cout << " hyopark is very cold"<<std::endl;
                                writeSize = 0;
                                serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setChunkedWriteSize(0);
                                fdManager.erase(curr_event->ident);
                            }
                        }
                        else
                        {
                            std::cout <<"cgi write end else"<<std::endl;
                            // serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setFdManager(serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0], serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getClientSocket());
                            change_events(change_list, serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                            std::cout << "read[fd]: " << serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getReadFd()[0] << std::endl;
                            std::cout << " hyopark is very cold"<<std::endl;
                            writeSize = 0;
                            serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].setChunkedWriteSize(0);
                            fdManager.erase(curr_event->ident);
                        }
                    }
                    else if (serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getLocationType() == LOCATIONTYPE_CGI_DONE || serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].getCurrLocation().getLocationType() == LOCATIONTYPE_NORMAL)
                    {
                        serverMap[clientsServerMap[fdManager[curr_event->ident]]].getClientMap()[fdManager[curr_event->ident]].writeFile(curr_event->ident);
                        fdManager.erase(curr_event->ident);
                    }
                    // continue;
                    
                }
                else if (clientsServerMap.find(curr_event->ident) != clientsServerMap.end() && serverMap[clientsServerMap[curr_event->ident]].getClientMap().find(curr_event->ident) != serverMap[clientsServerMap[curr_event->ident]].getClientMap().end())
                {
                    Server &currSever = serverMap[clientsServerMap[curr_event->ident]];
                    
                    // std::cout << "!!!!! 0 "<<std::endl;
                    // std::cout << "curr_event->ident"<<curr_event->ident<<std::endl;
                    // std::cout << currSever.getClientMap()[curr_event->ident].getResponseClass().getStatusCode()<<std::endl;
                //  std::cout << currSever.getStatus() <<std::endl;
                if (serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getResponseClass().getStatusCode() != 0)
                    {
                        Server &currSever = serverMap[clientsServerMap[curr_event->ident]];
                        // std::cout << "!!!!! 0 "<<std::endl;
                        std::cout <<"호쌍새찌"<< curr_event->ident << std::endl;
                        
                        // std::cout << "write3" <<std::endl;
                        // std::cout << "msf: " << ResponseMessage << std::endl;
                        // std::cout << "size" << ResponseMessage.size() << std::endl;
                        // std::cout << "curr identttttt: " << curr_event->ident << std::endl;
                        // fcntl(curr_event->ident,F_SETFL, O_NONBLOCK);
                            

                        // std::vector< std::string > chunkedVec = makeChunkedVec(serverMap[clientsServerMap[curr_event->ident]].getResponseClass().getBody());
                        static int vecIdx = -1;
                        if (serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getResponseClass().getBody().size() > 65535)
                        {
                            std::cout << " 요기" <<std::endl;
                            std::string tmpHeader;
                            if (vecIdx == -1)
                            {
                                int statusCode = serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getResponseClass().getStatusCode();
                                tmpHeader = "HTTP/1.1 " + std::to_string(statusCode) + " " + serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getResponseClass().statusMessage(statusCode) + "\r\n";
                                tmpHeader += "Server: a\r\nLast-Modified: a\r\nETag: 'A'\r\nAccept-Ranges: bytes\r\nConnection: close\r\nContent-Type: text/html;charset=UTF-8\r\nTransfer-Encoding: chunked\r\n\r\n";
                                std::cout << tmpHeader <<std::endl;
                                write(curr_event->ident, tmpHeader.c_str(), tmpHeader.size());
                                vecIdx++;
                            }
                            else if (vecIdx < serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getResponseClass().chunkedVec.size())
                            {
                                // std::cout << "보내는중" <<vecIdx<<std::endl;
                                // std::cout << "얼만큼?"<< serverMap[clientsServerMap[curr_event->ident]].getResponseClass().chunkedVec[vecIdx].size() <<std::endl;
                                    std::cout << " 조기" <<std::endl;
                                // std::cout << "뭘?" << serverMap[clientsServerMap[curr_event->ident]].getResponseClass().chunkedVec[vecIdx] <<std::endl;
                                if (write(curr_event->ident, serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getResponseClass().chunkedVec[vecIdx].c_str(), serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getResponseClass().chunkedVec[vecIdx].size()) == -1)
                                    {
                                    disconnect_client(curr_event->ident, serverMap[clientsServerMap[curr_event->ident]], clientsServerMap);
                                    std::cout << "고장나땅!" <<std::endl;
                                    exit(1);
                                    }
                                vecIdx++;
                            }
                            else
                            {
                                std::cout << "다보내땅!" <<std::endl;
                                // currSever.getClientMap()[curr_event->ident].getResponseClass().setStatusCode(0);
                                // serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getClientBody().clear();
                                // disconnect_client(curr_event->ident, serverMap[clientsServerMap[curr_event->ident]], clientsServerMap);
                                // clientsServerMap.erase(curr_event->ident);
                                // currSever.getClientMap().erase(curr_event->ident);
                                currSever.getClientMap()[curr_event->ident].resetServerValues();
                                vecIdx = -1;
                            }
                        }
                        else
                        {
                            std::string ResponseMessage = serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getResponseClass().writeResponseMessage();
                            if (write(curr_event->ident, ResponseMessage.c_str(), ResponseMessage.size()) == -1)
                            {
                                printErr("client write err");
                                disconnect_client(curr_event->ident, serverMap[clientsServerMap[curr_event->ident]], clientsServerMap);
                            }
                            else
                            {
                                // serverMap[clientsServerMap[curr_event->ident]].getClientMap()[curr_event->ident].getClientBody().clear();
                                // clientsServerMap.erase(curr_event->ident);
                                // currSever.getClientMap()[curr_event->ident].getResponseClass().setStatusCode(0);
                                // disconnect_client(curr_event->ident, serverMap[clientsServerMap[curr_event->ident]], clientsServerMap);
                                // currSever.getClientMap()[curr_event->ident].resetServerValues();
                                // clientsServerMap.erase(curr_eclientBodySizevent->ident);
                                // currSever.getClientMap().erase(curr_event->ident);
                                currSever.getClientMap()[curr_event->ident].resetServerValues();//왠지 여기클라이언트 삭제 할지말지고민해바야할덧 0612 -> 안해도되지않을까 왜냐면 리셋만하니까... 근데 현스키 케큐는 바디로 판단하기때문에 클라이언트 조건을 바꿀 필요가있음... 지금은 클라이언트 서버 맵으로 보는데 사실은 서버에 클라이언트가 있는지 find해야할 덧? 그게아니면 여기서 잘 지우는게 필요한데 지우면 안될것같은디 음... 1번 서버클라이언트만 지워보기 2번 조건을 바꿔보기
                                // currSever.getClientMap().erase(curr_event->ident);
                            }
                        }
                        
                        //지우기 Server : curr들 초기화 initServerCurrResponseAndRequestAndLocation

                    }
                    else if (currSever.getClientMap()[curr_event->ident].getStatus()== DONE)
                    {
                        std::cout << "satatus DONE"<<std::endl;;
                        std::map <std::string, std::string>::iterator findIter = currSever.getClientMap()[curr_event->ident].getRequestClass().getHeader().getContent().find("Content-Length");
                        // std::cout << "done body : "  << currSever.getClientMap()[curr_event->ident].getRequestClass().getBody();
                        // std::cout << "done body : "  << currSever.getClientMap()[curr_event->ident].getRequestClass().getBody();
                        
                        if (findIter != currSever.getClientMap()[curr_event->ident].getRequestClass().getHeader().getContent().end())//길이헤더 찾았을때
                        {   
                            // std::cout << "3\n";
                            if (std::atoi(findIter->second.c_str()) == currSever.getClientMap()[curr_event->ident].getRequestClass().getBody().size())//바디사이즈까지 같을때
                            {
                                // std::cout << "4\n";
                                // if (currSever.getCurrLocation().getLocationType() != LOCATIONTYPE_CGI_DONE)
                                currSever.getClientMap()[curr_event->ident].preProcess(currSever.getClientMap()[curr_event->ident].getCurrLocation().getLocationType());
                                currSever.getClientMap()[curr_event->ident].processMethod(change_list);
                                currSever.getClientMap()[curr_event->ident].setStatus(READY);
                            }
                        }
                        else//못찾았을때인데 헤더파싱은 끝나야함
                        {
                            std::cout << "satatus!! "<< currSever.getClientMap()[curr_event->ident].getCurrLocation().getLocationType()<<std::endl;;
                            currSever.getClientMap()[curr_event->ident].preProcess(currSever.getClientMap()[curr_event->ident].getCurrLocation().getLocationType());
                            currSever.getClientMap()[curr_event->ident].processMethod(change_list);
                            currSever.getClientMap()[curr_event->ident].setStatus(READY);
                        }
                    }
                    else if (currSever.getClientMap()[curr_event->ident].getStatus() == CHUNKED_FIN)
                    {
                        currSever.getClientMap()[curr_event->ident].preProcess(currSever.getClientMap()[curr_event->ident].getCurrLocation().getLocationType());
                        currSever.getClientMap()[curr_event->ident].processMethod(change_list);
                        currSever.getClientMap()[curr_event->ident].setStatus(READY);   
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