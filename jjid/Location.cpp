#include "Location.hpp"
#include "uniHeader.hpp"

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

std::map<std::vector<int>, std::string> Location::getErrPage()
{
 return (errPage);
}

std::vector<std::string> Location::getAllowMethod()
{
 return (AllowMethod);
}

// // bool getAutoIndex()
// {
//  return (AutoIndex);
// }

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

void Location::setErrPage(std::map<std::vector<int>, std::string> errPage)
{
	this->errPage = errPage;
}

void Location::setAllowMethod(std::vector<std::string> AllowMethod)
{
	this->AllowMethod = AllowMethod;
}

