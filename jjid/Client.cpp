#include "Client.hpp"


void aliasRoot(Location currLocation, std::string &path);
int checkPath(std::string &path);
Client::Client(int _clientSocket)
{

	// std::cout << "struct call"<< std::endl;
	clientSocket = _clientSocket;
	
	clientBody = "";
	chunkedStr = "";
	chunkedSize = 0;
	chunkedWriteSize = 0;
	status = 0;
	serverStatus = 0;
	readFd[0] = 0;
	readFd[1] = 0;
	writeFd[0] = 0;
	writeFd[1] = 0;
	cgiPid = -1;
	currRequest.clearRequest();
	envp = NULL;
	getResponseClass().setStatusCode(0);
	getResponseClass().CgiHeader = "";
	readBuf.clear();
	FDreadBuf.clear();
	vecIdx = -1;
	writeCnt = 0;
	
	
}

int &Client::getClientSocket()
{
	return (clientSocket);
}

std::string &Client::getClientBody()
{
	return (clientBody);
}

void Client::setClientSocket(int _fd)
{
	clientSocket= _fd;
}

void Client::setClientBody(std::string _body)
{
	clientBody = _body;
}






//~!@#$%^&*()PI*&^%$#@!$%^&*(

int Client::getServerFd()
{
	return (serverFd);
}

void Client::setFdManager(int fd, int _clientFd)
{
// std::cout <<"setFdManager" << fd<<", "<<_clientFd<<std::endl;
	if (fdManager->find(fd) != fdManager->end()) 
	{
		// std::cout << "매니저에 등록되면 안됨"<<std::endl;
		exit(1);
	}
	(*fdManager).insert(std::pair<int, int>(fd,_clientFd));
	// std::cout <<"등록결과 클라이언트:"<< (*fdManager)[fd]<<std::endl;
}

std::string &Client::getChunkedStr()
{
	return (chunkedStr);
}
void Client::addChunkedStr(std::string str)
{
	chunkedStr += str;
}
void Client::setChunkedStr(std::string str)
{
	chunkedStr += str;
}

int *Client::getReadFd()
{
	return (readFd);
}
int *Client::getWriteFd()
{
	return (writeFd);
}
	
void Client::setReadFd()
{
	pipe(readFd);
}

void Client::setWriteFd()
{
	pipe(writeFd);
}

void Client::setCgiPid(int pid)
{
	cgiPid=pid;
}

pid_t Client::getCgiPid()
{
	return (cgiPid);
}

void Client::forkCgiPid()
{
	cgiPid = fork();
}

Location &Client::getCurrLocation()
{
	return (this->currLocation);
}
Request& Client::getRequestClass()
{
	return (currRequest);
}

Response& Client::getResponseClass()
{
	return (currResponse);
}

int Client::getServerStatus()
{
	return (serverStatus);
}

void Client::setServerStatus(int serverStatus)
{
	this->serverStatus = serverStatus;
}



void Client::setErrorResponse(int statusCode)
{
	this->currResponse.setStatusCode(statusCode);
}
void Client::linkFdManager(std::map<int, int> &fdManager)
{
	this->fdManager = &fdManager;
}
void Client::linkChangeList(std::vector <struct kevent> &changeList)
{
	this->changeList = &changeList;
}

void Client::setServerFd(int fd)
{
	serverFd = fd;
}

std::string Client::autoIndexBody()
{
    std::string body = "";
	
	// 예외처리 고려
	DIR *dir = opendir(currRequest.getStartLine().path.c_str());
    struct dirent *dp;

	// std::cout << "[ Curr path ]" << currRequest.getStartLine().path << std::endl;
    body += "<h1>Index of /</h1><hr><pre>";
    for (dp = readdir(dir); dp; dp = readdir(dir))
    {
        std::string name = dp->d_name;
		std::string cont;
		struct stat buf;

		stat(name.c_str(), &buf);
		if (S_ISDIR(buf.st_mode))
			name += "/";
        cont = "<a href=\"" + name + "\">" + name + "</a>\r\n";
		body += cont;
	}
    body += "</pre><hr></body>\r\n";
    closedir(dir);
	return (body);
}

char  **Client::makeEnvp(int length)
{
	char **result = new char*[7];
	std::string str;

	int ret = currRequest.getStartLine().method;
	if (ret == 0)
		str = "GET";
	else if (ret == 1)
		str = "HEAD";
	else if (ret == 2)
		str = "POST";
	else if (ret == 3)
		str = "DELETE";
	
	//result[0]
	std::string temp = "REQUEST_METHOD=" + str;
	// std::cout << "jjibal0"<<temp<<std::endl;
	result[0] = new char[temp.size() + 1];
	result[0] = strcpy(result[0], temp.c_str());
	temp.clear();
	
	// // std::cout << "jjibal"<<result[0]<<std::endl;
	temp = "SERVER_PROTOCOL=HTTP/1.1";
	result[1] = new char[temp.size() + 1];
	result[1] = strcpy(result[1], temp.c_str());
	temp.clear();

	// std::cout <<"[" << currRequest.getStartLine().path << "]" << std::endl;

	//보류
	temp = "PATH_INFO=/Users/mac/goinfre/42_jjidserv/jjid/YoupiBanane/youpi.bla";
	result[2] = new char[temp.size() + 1];
	result[2] = strcpy(result[2], temp.c_str());
	temp.clear();
	
	// std::cout << "~~~ "<< currRequest.getHeader().getContent().find("Content-Length")->second << std::endl;
	// std::cout << currRequest.getBody().size() << std::endl;
	
	currRequest.getHeader().getContent().find("Content-Length");
	// temp = "CONTENT_LENGTH=" + currRequest.getHeader().getContent().find("Content-Length")->second;
	temp = "CONTENT_LENGTH=" + std::to_string(length);
	// std::cout << "jjibal0"<<temp<<std::endl;
	result[3] = new char[temp.size() + 1];
	result[3] = strcpy(result[3], temp.c_str());
	temp.clear();

	
	temp = "CONTENT_TYPE=" + currRequest.getHeader().getContent().find("Content-Type")->second;
	result[4] = new char[temp.size() + 1];
	result[4] = strcpy(result[4], temp.c_str());
	temp.clear();
	
	std::map< std::string, std::string >::iterator XIter = getRequestClass().getHeader().getContent().find("X-Secret-Header-For-Test");
	if (XIter != getRequestClass().getHeader().getContent().end())
	{
		temp = "HTTP_X_SECRET_HEADER_FOR_TEST=" + XIter->second;
		result[5] = new char[temp.size() + 1];
		result[5] = strcpy(result[5], temp.c_str());
		temp.clear();
		// result[6] = NULL;
	}
	else
		result[5] = NULL;
		result[6] = NULL;
	return (result);
}

void Client::setClientBodySize(int size)
{
	clientBodySize = size;
}

bool Client::cgiLocation(std::string const &path)
{
	std::vector < Location > locationVector = this->getLocations();
	Location tmpLocation;
	for (int idx = 0 ; idx < static_cast<int>(locationVector.size()); idx++)
	{	
		if (!locationVector[idx].getExtension().empty())// TODO 앞에 path 까지 확인하는거 추가 필요
		{
			std::string findPath = locationVector[idx].getPath();
			// if (findPath.size() > 1 && findPath[findPath.size()-1] == '/')
			// {
			// 	findPath.erase(findPath.size() - 1);
			// 	std::cout << "findpath erase : " << findPath << std::endl;
			// }
			if(getRequestClass().getExtension() == locationVector[idx].getExtension() && path.find(findPath) == 0)
			{
				currLocation = locationVector[idx];
				return (true);

			}
		}
	}
	return (false);
		
}

void Client::preProcess(int type)
{
    //allow method확인할것 // -> 메소드 함수 안에서 로케이션 정보 있이 하거나, 여기서 로케이션 결정후 확인하거나
	//isAllowMethod();
	//405ls
	// this->currResponse.setBody("");
	// this->currResponse.setStatusCode(0);
	// this->currResponse.setHeader(0);
	std::string pathTmp = currRequest.getStartLine().path;
	// std::cout << "ori pathTmp: "<< pathTmp << std::endl;
	// std::cout << "pte process location type: "<< type << std::endl;
	if (type != LOCATIONTYPE_CGI_DONE)
		if (cgiLocation(pathTmp))
		{
			// std::cout << "cgi find good" <<std::endl;
			return ;
		}
	currLocation = whereIsLocation(pathTmp);
	aliasRoot(currLocation, pathTmp);
	checkPath(pathTmp);
	// std::cout << "ori pathTmp after: "<< pathTmp << std::endl;
	currRequest.setPath(pathTmp);
	// std::cout << "pathTmp: "<< pathTmp << std::endl;
	// std::cout << "path2: "<< currRequest.getStartLine().path << std::endl;
	// std::cout << "prepro :: " << currLocation.getLocationType()  <<std::endl;
}

// int Client::checkMethod()
// {
	
// }


int &Client::getChunkedSize()
{
	return (chunkedSize);
}

void Client::setChunkedSize(int size)
{
	chunkedSize = size;
}

int &Client::getChunkedWriteSize()
{
	return (chunkedWriteSize);
}

void Client::setChunkedWriteSize(int size)
{
	chunkedWriteSize = size;
}

void Client::addChunkedWriteSize(int size)
{
	chunkedWriteSize += size;
}

bool Client::checkAllowMethod(std::vector<std::string> strVec, int method)
{
	
	for (int idx = 0 ; idx < static_cast<int>(strVec.size()); idx++)
	{
		if (getRequestClass().methodToNum(strVec[idx]) == method || method == -1)
			return true;
	}
	return false;
}

bool Client::checkClientMaxSize(int locatoinClientMaxSize , int currRequestSize)
{
	if (locatoinClientMaxSize != 0)
	{
		if (currRequestSize <= locatoinClientMaxSize)
			return true;
		else
			return false;
	}
	else
	{
		if (currRequestSize <= clientBodySize)
			return true;
		else
			return false;
	}
}

void Client::processMethod(std::vector <struct kevent> &change_list)
{
	// std::cout << "process call lcation type :: " << currLocation.getLocationType() << std::endl;
	if (currLocation.getLocationType() == LOCATIONTYPE_CGI && getRequestClass().getBody().empty())
	{
		currLocation.setLocationType(LOCATIONTYPE_CGI_DONE);
		preProcess(LOCATIONTYPE_CGI_DONE);
	}
	if (currLocation.getLocationType() == LOCATIONTYPE_NORMAL || currLocation.getLocationType() == LOCATIONTYPE_CGI_DONE)
	{
		// std::cout << "nomal !"<<std::endl;
		if (!checkAllowMethod(currLocation.getAllowMethod(), currRequest.getStartLine().method))
			return (setErrorResponse(405));
		if (!checkClientMaxSize( currLocation.getClientBodySize(),currRequest.getBody().size()) )
		{
			return (setErrorResponse(413));
		}
		switch (currRequest.getStartLine().method)
		{
			case GET:
				getMethod(NO_HEAD);
				break;
			case HEAD:
				getMethod(YES_HEAD);
				break;
			case POST:
				postMethod();
				break;
			case -1:
				postMethod();
				break;
			case DELETE:
				deleteMethod();
				break;
			default:
				return (setErrorResponse(405));
				//405와 같은 에러코드 처리
				break;
		}
	}
	else if (currLocation.getLocationType() ==  LOCATIONTYPE_CGI)
	{
		setReadFd();
		setWriteFd();
		
		// std::cout << "cGI !\n";
		setFdManager(writeFd[1], getClientSocket());
		setFdManager(readFd[0], getClientSocket());
		
		fcntl(writeFd[1], F_SETFL, O_NONBLOCK);
		fcntl(writeFd[0], F_SETFL, O_NONBLOCK);
		fcntl(readFd[1], F_SETFL, O_NONBLOCK);
		fcntl(readFd[0], F_SETFL, O_NONBLOCK);
		change_events(change_list, writeFd[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		// std::cout << "write[fd]: " << writeFd[1] << std::endl;
		// std::cout << "cgi에서 size: " << getRequestClass().getBody().size() << std::endl;
		// currLocation.setLocationType(LOCATIONTYPE_CGI_DONE);
		return ;
	}
	else if (currLocation.getLocationType() ==  LOCATIONTYPE_REDIR)
	{
		// std::cout << "cod::: " << currLocation.getReturnCode() << std::endl; 
		currResponse.setStatusCode(currLocation.getReturnCode());
		// TODO: 리다이렉션 정보 
	}
}


int checkPath(std::string &path)
{
	struct stat buf;
	std::string originPath = path;
	
	if (path[path.size()-1] == '/')
		path.erase(path.size()-1);
	if (stat(path.c_str(), &buf) == -1)
		return NOT;
	if (S_ISDIR(buf.st_mode)) // 테스트해본결과 /로 끝나던 말던 디렉토리면 걍 디렉토리임
	{
		path = originPath;
		return _DIR;
	}
	else if (S_ISREG(buf.st_mode))
		return _FILE;
	// else if (S_ISLNK())
	// 	return LINK;
	else
		return (-1);
}



Location Client::getDefaultLocation()
{
	Location defaultLocation;
	std::vector<std::string > tmpIndex;
	tmpIndex.push_back("index.html");
	defaultLocation.setIndex(tmpIndex);
	defaultLocation.setPath("/");
	defaultLocation.setRoot(".");
	//TODO
	return defaultLocation;
}

// checkLocation(std::string locationPath, std::string originpath)
// {
	
// }

Location Client::whereIsLocation(std::string const & path)
{
	std::vector < Location > locationVector = this->getLocations();
	Location tmpLocation;
	// std::cout << "whereis call!"<< std::endl;
	for (int idx = 0 ; idx < static_cast<int>(locationVector.size()); idx++)
	{
		// std::cout << "whereis idx!"<< idx << std::endl;
		std::string findPath = locationVector[idx].getPath();
		if (findPath.size() > 1 && findPath[findPath.size()-1] == '/')
		{
			findPath.erase(findPath.size() - 1);
			// std::cout << "findpath erase : " << findPath << std::endl;
		}
		
		if (path.find(findPath) == 0)
		{
			if (locationVector[idx].getExtension().empty())
				return (locationVector[idx]);
				
			// std::cout << "checkechekcek" <<std::endl;
			// if (locationVector[idx].getExtension().empty())//확장자가 없으면
			// {
			// 	std::cout << "empty?:"<< locationVector[idx].getExtension().empty() <<std::endl;
			// 	std::cout << "exten:"<< locationVector[idx].getExtension() <<std::endl;
			// 	return (locationVector[idx]);
			// }
			// else if (locationVector[idx].getExtension() == currRequest.getExtension())
			// {
			// 	std::cout << "req exten:"<< currRequest.getExtension() <<std::endl;
			// }
			// }
		}
	}
	return (getDefaultLocation());
}

// Location Client::whereIsLocation(std::string &path, std::vector<Location> locations)
// {
// 	// TODO : 로케이션이 디렉토리형식인지 파일형식인지 구분해서 다르게 처리 할 것
// 	//테스트 해보니까 로케이션이 없는경우 기본값 로케이션으로 하나봄 -> 메서드로 기본값로케이션 반환하는 거 하나 만들어서 (로케이션 사이즈가 0이거나 일치하는 로케이션이 없을때)에 예외처리 ㄱ
// 	//김진베는 뎁스로 가능한 로케이션 다 확인해서 구체적인거-> 일반적인거 순서로 해본다함
// 	std::cout << getPort() << getRoot() << "size:: " << locations.size() <<std::endl;
//  	std::string originpath = path;
// 	if (locations.size() == 0 )//|| pathType == )
// 	{
// 		path = DEFAULT_ROOT + path;// TODO : 디폴트의 루트를 넣어줘야함
// 		return (getDefaultLocation());
// 	}
// 	//형식 맞춰줌 인덱싱으로 연산 빠를 거라고 예상...	
// 	// 아무거토 없을때 현재로케이션에 있/없 서버에 있/없
// 	path = this->getRoot() + path; // TODO : 만약 서버블록안에도 루트가 있게 구조한다면 서버의 루트를 넣어줘야함
// 	std::cout << "seuan : " << path <<std::endl;
// 	// if (path[path.length() - 1] != '/')
// 	if (path[path.length() - 1] != '/'  && checkPath(path) == DIR )
// 			path += '/';
// 	//반복문 돌면서 일치하는거 확인
// 	for (int idx = 0 ; idx < locations.size(); idx++)
// 	{
// 		std::cout << "loca get path" << locations[idx].getPath() << std::endl;
// 		std::string locationPath = locations[idx].getPath();
// 		// if (locationPath[locationPath.length() - 1] != '/')
// 		// 	locationPath += '/';
// 		// if (checkLocation(locationPath,originpath))
// 		// {
		
// 		if (originpath.find(locationPath) == 0)
// 		{
// 			std::cout <<  "ori : " << originpath <<std::endl;
// 			std::cout <<  "location : " << locationPath <<std::endl;
// 			if (locationPath.size() != originpath.size())
// 			{
// 				path = locations[idx].getRoot() + originpath.substr(locationPath.size(), originpath.size() - locationPath.size() );
// 				std::cout << "get root : " <<locations[idx].getRoot() << " sub :: " << originpath.substr(locationPath.size(), originpath.size() - locationPath.size() ) << "path : " << path << std::endl; 
// 			}
// 			else
// 				path = locations[idx].getRoot() ;
// 			std::cout <<  "sub : " << originpath.substr(locationPath.size(), originpath.size() - locationPath.size() ) <<std::endl;
			
// 			// path = locations[idx].getRoot() + originpath;
// 			// path = (originpath.replace(0,locationPath.size(),locations[idx].getRoot()));
// 			std::cout << "path::: " << path << " ori::: "<<originpath << std::endl;
// 			// path = originpath.erase(0, locations[idx].getRoot().size());
// 			// path = locations[idx].getRoot() + originpath.substr(path.size(),locationPath.size());
// 			// // TODO getRoot 도 / 추가 할지말지
// 			// path = locations[idx].getRoot() ;//+ "/" ;//+ originpath; // alias 면 이거 , root면 오리진페스 까지 추가하면됨
// 			return (locations[idx]);
// 		}
// 	}
// 	return (getDefaultLocation());
// }

int Client::serchIndex(std::string &path, Location _currLocation)
{
	struct stat buf;
	
	// std::cout << "serchIndex" <<std::endl;
	//설정 인덱스가 없으면 디폴트 인덱스(현재는 index.html파일) 추가 후 종료
	if (path[path.size() - 1] != '/')
	{
		// std::cout << "400000000000003"<<std::endl;
		setErrorResponse(403);
		return (ADD_INDEX_FAIL);
	}
	if (_currLocation.getIndex().size() == 0)
	{
		// std::cout << "index 0 \n" ;
		path += DEFAULT_INDEX;
		return ADDED_INDEX;
	}
	//설정 인덱스가 있다면 가장 처음 있는 파일 경로 넣고 종료
	std::vector<std::string> currIndex = _currLocation.getIndex();
	for (int idx = 0; idx < static_cast<int>(currIndex.size()); idx++)
	{
		std::string tryIndexPath = path + currIndex[idx];
		// std::cout << tryIndexPath <<std::endl;
		if (stat((tryIndexPath).c_str(), &buf) == 0)
		{
			path = tryIndexPath;
			// std::cout <<"fin"<< path<<std::endl;
			return ADDED_INDEX;
		}
	}
	setErrorResponse(404);//??
	return (ADD_INDEX_FAIL);
}


std::string intToHexStr(int decimal)
{
   std::string hexadecimal; 
    int position = 0;
    while (1)
    {
        int mod = decimal % 16;    
        if (mod < 10) 
            hexadecimal[position] = 48 + mod;
        else       
            hexadecimal[position] = 97 + (mod - 10);
        decimal = decimal / 16;    
        position++;    
        if (decimal == 0)    
            break;
    }
    std::string ret;
    for (int i = position - 1; i >= 0; i--)
        ret +=hexadecimal[i];
    return ret;
}


std::vector<std::string > makeChunkedVec(std::string originStr)
{
	int idx = 0;
	// int currSzie = 0;
	
	std::vector<std::string > returnVec;

	while (idx < static_cast<int>(originStr.size()))
	{
		std::string currString = originStr.substr(idx,1024);
		idx += currString.size();
		
		// dec2hex(currString.size(), )
		
		currString = intToHexStr(currString.size())+ "\r\n" + currString + "\r\n";
		returnVec.push_back(currString);
		if (idx > static_cast<int>(originStr.size()))
			break ;
	}
	returnVec.push_back("0\r\n\r\n");
	return (returnVec);
}


void Client::readFile(int fd)
{
	// std::cout << "readFile call" << fd<<std::endl;
    std::string content = "";
    int n;
    char buf[1024];
    while ((n = read(fd, buf,1023)) > 0)
    {
        buf[n] = '\0';
        content += buf;
        memset(buf, 0, 1024);
    }
    this->currResponse.setStatusCode(200);
    this->currResponse.setBody(content);
    (*fdManager).erase(fd);
    // std::cout << "erase 결과" << (int)((*fdManager).find(fd) != (*fdManager).end())<<std::endl;
    close(fd);
    // std::cout << currResponse.getStatusCode() << std::endl;
    // std::cout << "readFile done" << std::endl;
    
}



void Client::writeFile(int fd)
{
	// std::cout << " wrtie File " << std::endl;
	// if (currRequest.getBody().size() > 65000)
	// {
	// 	std::cout << " very big!" << std::endl;//??????
	// 	std::vector <std::string > chunkedStrVec;

	// 	chunkedStrVec = makeChunkedVec(currRequest.getBody());
	// 	for (int idx = 0 ; idx < chunkedStrVec.size() ; idx++)
	// 	{
	// 		int n ;
	// 		// std::cout << idx << std::endl;
	// 		// std::cout <<"size" << chunkedStrVec[idx].size() << std::endl;
	// 		// std::cout << "fd" << fd <<std::endl;
	// 		// std::cout << "vec str:" << chunkedStrVec[idx] <<std::endl;
	// 		n = write(fd, chunkedStrVec[idx].c_str(), chunkedStrVec[idx].size());
	// 		// std::cout <<"n" << n << std::endl;
	// 	}
	// }
	// else
		// write(fd, currRequest.getBody().c_str(), currRequest.getBody().size());
	int n = write(fd, currRequest.getBody().c_str(), currRequest.getBody().size()); // CGI Parent process에서 request body도 바꾸기때문에 문제 없음.
	if (n == -1)
		printErr("write file err");
	this->currResponse.setStatusCode(201);
	(*fdManager).erase(fd);
	// std::cout << "erase 결과" << (int)((*fdManager).find(fd) != (*fdManager).end())<<std::endl;
	close(fd);
	// fdManager->erase(fd);
	// std::cout << " wrtie File Done" << std::endl;
    // this->currResponse.setBody(content);
    
}

void Client::openFile(std::string path, int isHead)
{		
	int	fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
	
	if (fd != -1)
	{
		if (isHead == NO_HEAD)
		{
			// std::cout << "오픈파일" <<fd<<"+"<<getClientSocket()<<std::endl;
			setFdManager(fd, getClientSocket());
			change_events(*changeList, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0,0,NULL);
		}
	}
	else
	{
		close(fd);
		return (setErrorResponse(404));
	}
	if (isHead == YES_HEAD)
	{
		this->currResponse.setStatusCode(405);
		close(fd);
	}
}

std::string whereIsRoot(std::string path, Location currLocation)
{
	(void)path;
	// std::string retPath;
	
	// path = 
	return ("." +currLocation.getRoot() + "/");
}

void aliasRoot(Location currLocation, std::string &path)
{
// path : /aaa
// locationpath : /aaa/
// root : ./ccc
// expect : ./ccc/

	std::string currRoot = currLocation.getRoot();
	std::string locationPath = currLocation.getPath();
	
	if (path[path.size() - 1 ] != '/')
		path = path + "/";
	std::string originPath = path; // [curr Rot]: ./[lo path]: /post_body[ori path]:/post_body/ ./post_body
	// std::cout << "[curr Rot]: "  << currRoot << "[lo path]: "<< locationPath << "[ori path]:" <<  originPath << std::endl;
	path = currRoot + originPath.substr(locationPath.size(), originPath.size() - locationPath.size() ); 
	
}

void Client::getMethod(int isHead)
{
	
	std::string path = this->currRequest.getStartLine().path;
	// std::cout << "resul ::" << path << std::endl;
	int pathType = checkPath(path);
	switch (pathType)
	{
		case _DIR ://디렉토리 안에 설정된 인덱스 파일들 탐색 해볼것임 ,  인덱스 파일 없다면(권한없어도) 403 // 만약 설정된 인덱스가 두개 이상이라면 첫번째꺼 // 만약 설정이 없다면 기본적으로 index.html 을 탐색함
			if (serchIndex(path, currLocation) == ADD_INDEX_FAIL)
			{
				// std::cout << "auto : " << currLocation.getAutoIndex() << std::endl;
				if (currLocation.getAutoIndex() == true)
				{
					currResponse.setBody(autoIndexBody());
					currResponse.setStatusCode(200);
				}
				return ;
			}
		case _FILE ://해당파일찾아볼것 마찬가지로 없다면 403	
			openFile(path, isHead);
			break;
		case NOT ://404
			return (setErrorResponse(404));
			/* throw 404 error */
			break;
		default:
			return (setErrorResponse(404));
			break;
	}
	
	
}

void Client::postMethod()
{
	// 기존 파일이 있는지 확인 후 있으면 수정, 없으면 크리에이트 , 디렉토리면...?일단 에러
	// 쿼리날리는거, 디비접근 등도 고려해야하는지 섭젝 읽어볼것
		// std::string path = this->currRequest.getStartLine().path;
	// path = "." + path;//root 키워드로 설정하기 설정 없다면 디폴트로 추가하기, 절대경로로 바꿀것
	// std::cout << "postmethod:: " << path << std::endl;
	// int pathType = checkPath(path);
	
	
	// std::string path = this->currRequest.getStartLine().path;
	// path = "." + path;//root 키워드로 설정하기 설정 없다면 디폴트로 추가하기, 절대경로로 바꿀것
	// int pathType = checkPath(path);
	// Location currLocation = whereIsLocation(path, locations);
	
	// return (setErrorResponse(405));
	
	
	// std::string path = this->currRequest.getStartLine().path;
	// Location currLocation = whereIsLocation(path);//find or  map match 등 다른이름 추천받음
	// aliasRoot(currLocation, path);
	
	
	// path = "." + path;//root 키워드로 설정하기 설정 없다면 디폴트로 추가하기, 절대경로로 바꿀것 // 서버의 루트 먼저 붙이고 로케이션 붙이기
	
	std::string path = this->currRequest.getStartLine().path;
	int pathType = checkPath(path);
	//TODO serchIndex 인자값으로 해당 메소드 넘겨줘서 겟이면 바꾸고 포스트면 안바꾸고로 수정필요
	if (serchIndex(path, currLocation) == ADD_INDEX_FAIL)
		currResponse.setStatusCode(0);

	int fd = 0;
	// std::cout << "post result:" << path << std::endl;
	if (pathType == _FILE || pathType == _DIR)
	{
		// std::cout << "file " << std::endl;
		if ((fd = open(path.c_str(), O_WRONLY | O_APPEND | O_NONBLOCK, 0644)) == -1)
			return (setErrorResponse(500));
	}
	else if (pathType == NOT)
	{
		// std::cout << "not " << std::endl;
		int findIdx = path.find_last_of("/");// ./posytsas
		if (findIdx != 1)
		{
			std::string dirPath = path.substr(0, path.find_last_of("/"));
			// std::cout << dirPath << std::endl;
			mkdir(dirPath.c_str(),0777);
		}
		if ((fd = open(path.c_str(), O_WRONLY | O_CREAT | O_NONBLOCK, 0644)) == -1)
			return (setErrorResponse(500));
	}
	else if (pathType == _DIR)
	{   
		// std::cout << "dir " << std::endl;
		return (setErrorResponse(403));
	}
	// currResponse.setStatusCode(201);//이거 안바꿔야함
	// // std::cout << "bbbbbbeutetful" << currRequest.getBody() << std::endl;
	// if (currRequest.getHeader().getContent()["Content-Length"][0] == '0')
	// if (currRequest.getBody().size() != 0)
	// {
		fcntl(fd, F_SETFL, O_NONBLOCK);
		// std::cout << "post client fd" <<getClientSocket()<<std::endl;
		setFdManager(fd, getClientSocket());
		change_events(*changeList, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0,0,NULL);
		// write(fd, currRequest.getBody().c_str(), currRequest.getBody().size());
	// }
	// else
	// {
	// 	std::cout << " 504:::"<<std::endl;
	// 	currResponse.setStatusCode(405);
	// 	close(fd);
	// }
	//write 처리후 fd 닫기 까먹지 않기 ><
}

void Client::deleteMethod()
{
	std::string path = this->currRequest.getStartLine().path;
	path = "." + path;
	currResponse.setStatusCode(200);
	this->currResponse.setBody("complite delete");
	
	std::remove(path.c_str());
}

void Client::resetServerValues()
{
	// std::cout << "reset caaaaallllll"<<std::endl;
	getResponseClass().setBody("");
	getResponseClass().setStatusCode(0);
	getResponseClass().getHeader().getContent().clear();
	
	getRequestClass().setBody("");
	getRequestClass().setPath("");
	chunkedStr = "";
	chunkedWriteSize = 0;
	chunkedSize = 0;
	vecIdx = -1;
	writeCnt = 0;
	// getRequestClass().getStartLine().http = "";
	getRequestClass().setPath("");
	getResponseClass().CgiHeader = "";
	currRequest.clearRequest();
	
	if (getCurrLocation().getLocationType() == LOCATIONTYPE_CGI_DONE)
		getCurrLocation().setLocationType(LOCATIONTYPE_CGI);
	// TODO w/r fd 들 초기화 해야하나?
	// delete [] envp;
	envp = NULL;
	clientBody.clear();
	status=READY;
	// clientBodySize = 0;
	// setChunkedSize(0);
	// setCurrChunkedSize(0);
	
	
	setCgiPid(-1);
	readFd[0] = -1;
	readFd[1] = -1;
	writeFd[0] = -1;
	writeFd[1] = -1;
	// TODO fdFlag 초기화?
	serverStatus = SERVER_READY;
	readBuf.clear();
	FDreadBuf.clear();
}
void Client::parseChunkedBody()
{
	// std::cout << "파스청크바디" << getRequestClass().getBody().size()<<std::endl;
	int idx = 0;
	int cunkeSize = 0;
	std::string orginBody = getRequestClass().getBody();
	std::string resultBody;
	while (idx < static_cast<int>(orginBody.size()))
	{
		int find = orginBody.find("\r\n", idx);
		cunkeSize = std::strtol(orginBody.substr(idx, find - idx).c_str(), NULL, 16);
		// std::cout << "청크드 사이즈 " << cunkeSize << std::endl;
		if (cunkeSize == 0)//|| npos
			break;
		resultBody += orginBody.substr(find + 2, cunkeSize);
		idx = find + 2 + cunkeSize + 1;
	}
	getRequestClass().setBody(resultBody);
	// std::cout << "파스청크바디 끝" << getRequestClass().getBody().size()<<std::endl;
}
// void Client::headMethod()
// {
		
// }
