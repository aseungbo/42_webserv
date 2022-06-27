#include "Server.hpp"

void aliasRoot(Location currLocation, std::string &path);
int checkPath(std::string &path);

void Server::setPort(int port)
{
	this->port = port;
}

Server::Server()
{
	setPort(0);
	setClientBodySize(0);
	setServerFd(0);
}

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

void Server::setErrPage(std::map<int, std::string> errPage)
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

int Server::getServerFd()
{
	return (serverFd);
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

std::map<int, std::string> Server::getErrPage()
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

std::map< int, Client >& Server::getClientMap()
{
	return clientMap; 
}

void Server::addClient(int clientSocket)
{
	Client cli = Client(clientSocket);
	
	cli.linkChangeList(*changeList);
	cli.linkFdManager(*fdManager);
	cli.setClientBodySize(clientBodySize);
	cli.setLocations(locations);
	cli.setServerFd(serverFd);
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
