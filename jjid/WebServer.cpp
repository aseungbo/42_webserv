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
	std::ifstream fin(this->confPath);
	std::string str;
	
	if (fin.is_open())
		fin >> str;
	else
		printErr("Can not open file.");
	fin.close();
	
	Server server;
	
	this->servers.push_back(server);
	// parse 후 server에 할당
	std::vector<std::string> host;
	host.push_back("127.0.0.1");
	this->servers[0].setHost(host);
	
	int port;
	// port.push_back(80);
	this->servers[0].setPorts(8081);
	
	// this->servers[0].setLocation(Location(""));
	this->servers[0].setClientBodySize(1000);
	// this->servers[0].setErrPage();
	// std::vector<Server> servers;
	// std::vector < int > serverFd; //자료형 고려
	// std::string confPath;	
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
int read_data(int fd, char *buffer, int buf_size)
{
	int size = 0;
	int len;
	while (1)
	{
		if ((len = read(fd, &buffer[size], buf_size - size)) > 0)
		{
			size += len;
			if (size == buf_size)
			{
				return size;
			}
		}
		else if (len == 0)
		{
			return size;
		}
		else
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				return -1;
			}
		}
	}
}

void WebServer::monitorKqueue()
{
	while(42)
	{
		for (std::map<int, Server>::iterator iter = this->serverMap.begin(); iter != this->serverMap.end(); iter++)
		{
			struct sockaddr_in clntAddr;
			socklen_t clntAddrSize = sizeof(clntAddr);
			int clientSocket;
			
			// std::cout << "first: " << iter->first << " second: " << iter->second.getPort() << std::endl;
			if ((clientSocket = accept(iter->first, (struct sockaddr *)&clntAddr, &clntAddrSize)) != -1)
			{
				char buf[1024];
				std::string str;
				int n;
				
				// n = read_data(clientSocket, buf, 1024);
				
				str.clear();
				while (1)
				{
					n = read(clientSocket, buf, sizeof(buf));
					if (n > 0)
					{
						str += buf;
						break;
					}
				}
				std::cout << str << std::endl;
				exit(0);
			}
			// std::cout << "Accept new client: " << client_socket << std::endl;
			fcntl(clientSocket, F_SETFL, O_NONBLOCK);
			
		}	
	}
}
//->안의 기능은 따로 뺄예정


int main (int ac, char **av)
{
	std::string confPath;
	// ac 예외처리 함수로 분기
	if (ac == 2)
		confPath = av[1];
	else if (ac == 1)
		confPath = DEFAULT_PATH;
	else
		printErr("Too many argu");
	WebServer myFirstWebServer(confPath);
	myFirstWebServer.parseConfig();
	myFirstWebServer.listenServers();
	myFirstWebServer.monitorKqueue();
	// string confPath = string checkArgu(ac, av);// -> 1 : conf파일 제대로 들어온경우 2 : default로 가야하는경우 0 : 종료해야할 경우(잘못된 파일,)
	// // WebServer a;
	// Server a;
}