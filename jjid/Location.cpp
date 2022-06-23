#include "Location.hpp"
#include "uniHeader.hpp"

Location::Location(void)
{
    setLocationType(LOCATIONTYPE_NORMAL);
    setAutoIndex(false);
	setReturnCode(0);
	setClientBodySize(0);
}

std::string Location::getRoot()
{
 return (root);
}

std::string Location::getPath()
{
 return (path);
}

std::vector<std::string> Location::getIndex()
{
 return (index);
}

std::map<int, std::string> Location::getErrPage()
{
 return (errPage);
}

std::vector<std::string> Location::getAllowMethod()
{
 return (AllowMethod);
}

int Location::getLocationType()
{
	return (locationType);
}

std::string Location::getExtension()
{
	return (extension);
}

std::string Location::getCgiPath()
{
	return (cgiPath);
}

int Location::getReturnCode()
{
	return (returnCode);
}

std::string Location::getReturnUrl()
{
	return (returnUrl);
}

bool Location::getAutoIndex()
{
	return (autoIndex);
}

int Location::getClientBodySize()
{
	return (clientBodySize);
}

void Location::setClientBodySize(int size)
{
	this->clientBodySize = size;
}

void Location::setRoot(std::string root)
{
	this->root = root;
}

void Location::setPath(std::string path)
{
	this->path = path;
}

void Location::setIndex(std::vector<std::string> index)
{
	this->index = index;
}

void Location::setErrPage(std::map<int, std::string> errPage)
{
	this->errPage = errPage;
}

void Location::setAllowMethod(std::vector<std::string> AllowMethod)
{
	this->AllowMethod = AllowMethod;
}

void Location::setLocationType(int type)
{
	this->locationType = type;
}

void Location::setExtension(std::string extension)
{
	this->extension = extension;
}

void Location::setCgiPath(std::string cgiPath)
{
	this->cgiPath = cgiPath;
}

void Location::setReturnCode(int code)
{
	this->returnCode = code;	
}

void Location::setReturnUrl(std::string url)
{   
	this->returnUrl = url;
}

void Location::setAutoIndex(bool flag)
{
	this->autoIndex = flag;
}
