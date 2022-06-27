#include "Client.hpp"

void aliasRoot(Location currLocation, std::string &path);
int checkPath(std::string &path);
Client::Client(int _clientSocket)
{

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
	getResponseClass().setErrStatusCode(0);
	getResponseClass().CgiHeader = "";
	vecIdx = -1;
	writeCnt = 0;
}

void Client::setStatus(int stat)
{
	status = stat;
}

int Client::getStatus()
{
	return (status);
}

void Client::setLocations(std::vector<Location> _locations)
{
	locations = _locations;
}

std::vector<Location> Client::getLocations()
{
	return (locations);
}

int &Client::getClientSocket()
{
	return (clientSocket);
}

std::string &Client::getClientBody()
{
	return (clientBody);
}

int &Client::getVecIdx()
{
	return (vecIdx);
}
void Client::setVecIdx(int idx)
{
	vecIdx = idx;
}

int &Client::getWriteCnt()
{
	return (writeCnt);
}
void Client::setWriteCnt(int cnt)
{
	writeCnt = cnt;
}

void Client::setClientSocket(int _fd)
{
	clientSocket = _fd;
}

void Client::setClientBody(std::string _body)
{
	clientBody = _body;
}

int Client::getServerFd()
{
	return (serverFd);
}

void Client::setFdManager(int fd, int _clientFd)
{

	if (fdManager->find(fd) != fdManager->end())
	{

		exit(1);
	}
	(*fdManager).insert(std::pair<int, int>(fd, _clientFd));
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
	cgiPid = pid;
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
Request &Client::getRequestClass()
{
	return (currRequest);
}

Response &Client::getResponseClass()
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

void Client::setErrorResponse(int _statusCode)
{
	std::string path;
	if (currLocation.getErrPage().find(_statusCode) != currLocation.getErrPage().end())
	{
		path = currLocation.getErrPage()[_statusCode];
	}
	else
		path = "./defaultErrPage/" + std::to_string(_statusCode) + ".html";

	int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	if (fd != -1)
	{
		setFdManager(fd, getClientSocket());
		change_events(*changeList, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	}
	else
	{
		close(fd);
		return (printErr("errPageOpenFail"));
	}
	this->currResponse.setErrStatusCode(_statusCode);
}

void Client::linkFdManager(std::map<int, int> &fdManager)
{
	this->fdManager = &fdManager;
}
void Client::linkChangeList(std::vector<struct kevent> &changeList)
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

	DIR *dir = opendir(currRequest.getStartLine().path.c_str());
	struct dirent *dp;

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

char **Client::makeEnvp(int length)
{
	char **result = new char *[7];
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
	else if (ret == 4)
		str = "PUT";

	std::string temp = "REQUEST_METHOD=" + str;

	result[0] = new char[temp.size() + 1];
	result[0] = strcpy(result[0], temp.c_str());
	temp.clear();

	temp = "SERVER_PROTOCOL=HTTP/1.1";
	result[1] = new char[temp.size() + 1];
	result[1] = strcpy(result[1], temp.c_str());
	temp.clear();

	temp = "PATH_INFO=/Users/mac/goinfre/42_jjidserv/jjid/YoupiBanane/youpi.bla";
	result[2] = new char[temp.size() + 1];
	result[2] = strcpy(result[2], temp.c_str());
	temp.clear();

	currRequest.getHeader().getContent().find("Content-Length");

	temp = "CONTENT_LENGTH=" + std::to_string(length);

	result[3] = new char[temp.size() + 1];
	result[3] = strcpy(result[3], temp.c_str());
	temp.clear();

	temp = "CONTENT_TYPE=" + currRequest.getHeader().getContent().find("Content-Type")->second;
	result[4] = new char[temp.size() + 1];
	result[4] = strcpy(result[4], temp.c_str());
	temp.clear();

	std::map<std::string, std::string>::iterator XIter = getRequestClass().getHeader().getContent().find("X-Secret-Header-For-Test");
	if (XIter != getRequestClass().getHeader().getContent().end())
	{
		temp = "HTTP_X_SECRET_HEADER_FOR_TEST=" + XIter->second;
		result[5] = new char[temp.size() + 1];
		result[5] = strcpy(result[5], temp.c_str());
		temp.clear();
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
	std::vector<Location> locationVector = this->getLocations();
	Location tmpLocation;
	for (int idx = 0; idx < static_cast<int>(locationVector.size()); idx++)
	{
		if (!locationVector[idx].getExtension().empty())
		{
			std::string findPath = locationVector[idx].getPath();

			if (getRequestClass().getExtension() == locationVector[idx].getExtension() && path.find(findPath) == 0)
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

	std::string pathTmp = currRequest.getStartLine().path;

	if (type != LOCATIONTYPE_CGI_DONE)
		if (cgiLocation(pathTmp))
		{
			return;
		}
	currLocation = whereIsLocation(pathTmp);
	aliasRoot(currLocation, pathTmp);
	checkPath(pathTmp);

	currRequest.setPath(pathTmp);
}

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
	if (strVec.empty())
		return true;
	for (int idx = 0; idx < static_cast<int>(strVec.size()); idx++)
	{
		if (getRequestClass().methodToNum(strVec[idx]) == method)
			return true;
	}
	return false;
}

bool Client::checkClientMaxSize(int locatoinClientMaxSize, int currRequestSize)
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

void Client::processMethod(std::vector<struct kevent> &change_list)
{

	if (currLocation.getLocationType() == LOCATIONTYPE_CGI && getRequestClass().getBody().empty())
	{
		currLocation.setLocationType(LOCATIONTYPE_CGI_DONE);
		preProcess(LOCATIONTYPE_CGI_DONE);
	}
	if (currLocation.getLocationType() == LOCATIONTYPE_NORMAL || currLocation.getLocationType() == LOCATIONTYPE_CGI_DONE)
	{

		if (!checkAllowMethod(currLocation.getAllowMethod(), currRequest.getStartLine().method))
			return (setErrorResponse(405));
		if (!checkClientMaxSize(currLocation.getClientBodySize(), currRequest.getBody().size()))
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
		case DELETE:
			deleteMethod();
			break;
		case PUT:
			postMethod();
			break;
		default:
			return (setErrorResponse(405));
			break;
		}
	}
	else if (currLocation.getLocationType() == LOCATIONTYPE_CGI)
	{
		setReadFd();
		setWriteFd();

		setFdManager(writeFd[1], getClientSocket());
		setFdManager(readFd[0], getClientSocket());

		fcntl(writeFd[1], F_SETFL, O_NONBLOCK);
		fcntl(writeFd[0], F_SETFL, O_NONBLOCK);
		fcntl(readFd[1], F_SETFL, O_NONBLOCK);
		fcntl(readFd[0], F_SETFL, O_NONBLOCK);
		change_events(change_list, writeFd[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

		return;
	}
	else if (currLocation.getLocationType() == LOCATIONTYPE_REDIR)
	{
		currResponse.setStatusCode(currLocation.getReturnCode());
		currResponse.setBody(currLocation.getReturnUrl());
	}
}

int checkPath(std::string &path)
{
	struct stat buf;
	std::string originPath = path;

	if (path[path.size() - 1] == '/')
		path.erase(path.size() - 1);
	if (stat(path.c_str(), &buf) == -1)
		return NOT;
	if (S_ISDIR(buf.st_mode))
	{
		path = originPath;
		return _DIR;
	}
	else if (S_ISREG(buf.st_mode))
		return _FILE;

	else
		return (-1);
}

Location Client::getDefaultLocation()
{
	Location defaultLocation;
	std::vector<std::string> tmpIndex;
	tmpIndex.push_back("index.html");
	defaultLocation.setIndex(tmpIndex);
	defaultLocation.setPath("/");
	defaultLocation.setRoot(".");

	return defaultLocation;
}

Location Client::whereIsLocation(std::string const &path)
{
	std::vector<Location> locationVector = this->getLocations();
	Location tmpLocation;

	for (int idx = 0; idx < static_cast<int>(locationVector.size()); idx++)
	{

		std::string findPath = locationVector[idx].getPath();
		if (findPath.size() > 1 && findPath[findPath.size() - 1] == '/')
		{
			findPath.erase(findPath.size() - 1);
		}

		if (path.find(findPath) == 0)
		{
			if (locationVector[idx].getExtension().empty())
				return (locationVector[idx]);
		}
	}
	return (getDefaultLocation());
}

int Client::serchIndex(std::string &path, Location _currLocation, int flag)
{
	struct stat buf;

	if (path[path.size() - 1] != '/')
	{

		if (flag != POST)
		{
			setErrorResponse(403);
		}
		return (ADD_INDEX_FAIL);
	}
	if (_currLocation.getIndex().size() == 0)
	{

		path += DEFAULT_INDEX;
		return ADDED_INDEX;
	}

	std::vector<std::string> currIndex = _currLocation.getIndex();
	for (int idx = 0; idx < static_cast<int>(currIndex.size()); idx++)
	{
		std::string tryIndexPath = path + currIndex[idx];

		if (stat((tryIndexPath).c_str(), &buf) == 0)
		{
			path = tryIndexPath;

			return ADDED_INDEX;
		}
	}
	if (flag != POST && _currLocation.getAutoIndex() == false)
		setErrorResponse(404);
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
		ret += hexadecimal[i];
	return ret;
}

std::vector<std::string> makeChunkedVec(std::string originStr)
{
	int idx = 0;

	std::vector<std::string> returnVec;

	while (idx < static_cast<int>(originStr.size()))
	{
		std::string currString = originStr.substr(idx, 1024);
		idx += currString.size();

		currString = intToHexStr(currString.size()) + "\r\n" + currString + "\r\n";
		returnVec.push_back(currString);
		if (idx > static_cast<int>(originStr.size()))
			break;
	}
	returnVec.push_back("0\r\n\r\n");
	return (returnVec);
}

void Client::readFile(int fd)
{

	std::string content = "";
	int n;
	char buf[1024];
	while ((n = read(fd, buf, 1023)) > 0)
	{
		buf[n] = '\0';
		content += buf;
		memset(buf, 0, 1024);
	}
	if (n <= 0)
	{
		if (n == 0)
		{
			if (this->currResponse.getErrStatusCode() == 0)
				this->currResponse.setStatusCode(200);
			else
				this->currResponse.setStatusCode(this->currResponse.getErrStatusCode());
			this->currResponse.setBody(content);
		}
		if ((*fdManager).find(fd) != (*fdManager).end())
			(*fdManager).erase(fd);
		close(fd);
	}
}

void Client::writeFile(int fd)
{
	int n = write(fd, currRequest.getBody().c_str(), currRequest.getBody().size());
	if (n == -1 || (n == 0 && currRequest.getBody().size() != 0))
		printErr("write file err");
	else 
		this->currResponse.setStatusCode(201);
	if ((*fdManager).find(fd) != (*fdManager).end())
		(*fdManager).erase(fd);
	close(fd);
}

void Client::openFile(std::string path, int isHead)
{
	int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);

	if (fd != -1)
	{
		if (isHead == NO_HEAD)
		{

			setFdManager(fd, getClientSocket());
			change_events(*changeList, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
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

	return ("." + currLocation.getRoot() + "/");
}

void aliasRoot(Location currLocation, std::string &path)
{

	std::string currRoot = currLocation.getRoot();
	std::string locationPath = currLocation.getPath();

	if (path[path.size() - 1] != '/')
		path = path + "/";
	std::string originPath = path;

	path = currRoot + originPath.substr(locationPath.size(), originPath.size() - locationPath.size());
}

void Client::getMethod(int isHead)
{

	std::string path = this->currRequest.getStartLine().path;

	int pathType = checkPath(path);
	switch (pathType)
	{
	case _DIR:
		if (serchIndex(path, currLocation, GET) == ADD_INDEX_FAIL)
		{
			if (currLocation.getAutoIndex() == true)
			{
				currResponse.setBody(autoIndexBody());
				currResponse.setErrStatusCode(0);
				currResponse.setStatusCode(200);
			}
			return;
		}
	case _FILE:
		openFile(path, isHead);
		break;
	case NOT:
		return (setErrorResponse(404));
		break;
	default:
		return (setErrorResponse(404));
		break;
	}
}

void Client::postMethod()
{

	std::string path = this->currRequest.getStartLine().path;
	int pathType = checkPath(path);

	if (serchIndex(path, currLocation, POST) == ADD_INDEX_FAIL)
	{
		currResponse.setStatusCode(0);
		currResponse.setErrStatusCode(0);
	}

	int fd = 0;

	if (pathType == _FILE || pathType == _DIR)
	{

		if ((fd = open(path.c_str(), O_WRONLY | O_APPEND | O_NONBLOCK, 0644)) == -1)
			return (setErrorResponse(500));
	}
	else if (pathType == NOT)
	{

		int findIdx = path.find_last_of("/");
		if (findIdx != 1)
		{
			std::string dirPath = path.substr(0, path.find_last_of("/"));

			mkdir(dirPath.c_str(), 0777);
		}
		if ((fd = open(path.c_str(), O_WRONLY | O_CREAT | O_NONBLOCK, 0644)) == -1)
			return (setErrorResponse(500));
	}
	else if (pathType == _DIR)
	{
		return (setErrorResponse(403));
	}

	fcntl(fd, F_SETFL, O_NONBLOCK);

	setFdManager(fd, getClientSocket());
	change_events(*changeList, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void Client::deleteMethod()
{
	std::string path = this->currRequest.getStartLine().path;
	int pathType = checkPath(path);


	if (pathType == _FILE )
	{
		if (remove(path.c_str()) != 0)
			return(setErrorResponse(500));
		currResponse.setStatusCode(200);
	}
	else if (pathType == NOT)
	{
		return(setErrorResponse(404));
	}
	else if (pathType == _DIR)
	{

		if (rmdir(path.c_str())!= 0)
			return(setErrorResponse(500));
		currResponse.setStatusCode(200);
	}
}

void Client::resetServerValues()
{

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

	getRequestClass().setPath("");
	getResponseClass().CgiHeader = "";
	currRequest.clearRequest();
	getResponseClass().setErrStatusCode(0);

	if (getCurrLocation().getLocationType() == LOCATIONTYPE_CGI_DONE)
		getCurrLocation().setLocationType(LOCATIONTYPE_CGI);

	envp = NULL;
	clientBody.clear();
	status = READY;

	setCgiPid(-1);
	readFd[0] = -1;
	readFd[1] = -1;
	writeFd[0] = -1;
	writeFd[1] = -1;

	serverStatus = SERVER_READY;
}

void Client::parseChunkedBody()
{

	int idx = 0;
	int cunkeSize = 0;
	std::string orginBody = getRequestClass().getBody();
	std::string resultBody;
	while (idx < static_cast<int>(orginBody.size()))
	{
		int find = orginBody.find("\r\n", idx);
		cunkeSize = std::strtol(orginBody.substr(idx, find - idx).c_str(), NULL, 16);

		if (cunkeSize == 0)
			break;
		resultBody += orginBody.substr(find + 2, cunkeSize);
		idx = find + 2 + cunkeSize + 1;
	}
	getRequestClass().setBody(resultBody);
}
