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

// // bool getAutoIndex()
// {
//  return (AutoIndex);
// }
// std::vector<std::string> Location::getMethodsAllowed()
// {
//  return (methodsAllowed);
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
// void Location::setMethodsAllowed(std::vector<std::string> methodsAllowed)
// {
// 	this->methodsAllowed = methodsAllowed;
// }

