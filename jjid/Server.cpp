#include "Server.hpp"

void aliasRoot(Location currLocation, std::string &path);
int checkPath(std::string &path);

void Server::setPort(int port)
{
	this->port = port;
}
Server::Server()
{
	// setClientSocket(0);
	// setStatus(READY);
	// setChunkedSize(0);
	// setCurrChunkedSize(0);
	setPort(0);
	setClientBodySize(0);
	// setServerFd(0);
	// fdFlag = 0;
	// setCgiPid(-1);
	// getResponseClass().setStatusCode(0);
	// serverStatus = SERVER_READY;
	// std::map<int, int> *fdManager;
}




// int *Server::getCgiWriteFd()
// {
// 	return (cgiWriteFd);
// }
// int *Server::getCgiReadFd()
// {
// 	return (cgiReadFd);
// }

// std::string Server::getCgiBody()
// {
// 	return (cgiBody);
// }
// char Server::**getEnvp()
// {
// 	return (envp);
// }

// void Server::setCgiWriteFd(int cgiWriteFd[2])
// {
// 	this->cgiWriteFd = cgiWriteFd;
// }
// void Server::setCgiReadFd(int cgiReadFd[2])
// {
// 	this->cgiReadFd = cgiReadFd;
// }
// void Server::setCgiBody(std::string str)
// {
// 	this->cgiBody = str;
// }
// void Server::setEnvp(char **envp)
// {
// 	this->envp = envp;
// }


void Server::setServerFd(int fd)
{
	serverFd = fd;
}
void change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

void Server::setRoot(std::string root)
{
	this->root = root;
}

void Server::setHosts(std::vector<std::string> hosts)
{
	this->hosts = hosts;
}


void Server::setClientBodySize(int clientBodySize)
{
	this->clientBodySize = clientBodySize;
}

void Server::setIndex(std::vector<std::string> index)
{
	this->index = index;
}

void Server::setErrPage(std::map<std::vector<int>, std::string> errPage)
{
	this->errPage = errPage;
}

void Server::setLocation(std::vector<Location> locations)
{
	this->locations = locations;
};

void Server::setAllowMethod(std::vector<std::string> allowMethod)
{
	this->allowMethod = allowMethod;
}

std::string Server::getRoot()
{
	return (root);
}

std::vector<std::string> Server::getHost()
{
	return (hosts);
}

int Server::getPort()
{
	return (port);
}

int Server::getClientBodySize()
{
	return (clientBodySize);
}

std::vector<std::string> Server::getIndex()
{
	return (index);
}

std::map<std::vector<int>, std::string> Server::getErrPage()
{
	return (errPage);
}

std::vector<std::string> Server::getAllowMethod()
{
	return (allowMethod);
}

std::vector<Location> Server::getLocations()
{
	return (locations);
}



void Server::addClient(int clientSocket)
{
	// Client cli(clientSocket);
	// std::cout << "addclient call!"<< std::endl;
	Client cli = Client(clientSocket);
	
	std::cout << "Made client socket fd: " << clientSocket << std::endl;
	cli.linkChangeList(*changeList);
	cli.linkFdManager(*fdManager);
	// clientMap[clientSocket] = client;
	cli.setClientBodySize(clientBodySize);
	cli.setLocations(locations);
	clientMap.insert(std::pair< int, Client>(clientSocket, cli));
}


void Server::linkFdManager(std::map<int, int> &fdManager)
{
	this->fdManager = &fdManager;
}

void Server::linkChangeList(std::vector <struct kevent> &changeList)
{
	this->changeList = &changeList;
}








// void Server::setCgiEvent(std::vector <struct kevent> &change_list)
// {
// 	envp = makeEnvp();
// 	pipe(cgiWriteFd);
// 	pipe(cgiReadFd);
// 	change_events(change_list, writeFd[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
// 	change_events(change_list, writeFd[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
// 	change_events(change_list, readFd[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
// 	change_events(change_list, readFd[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
// }




