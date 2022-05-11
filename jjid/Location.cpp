#include "Location.hpp"
#include "uniHeader.hpp"

			// std::string root;
			// // std::string index;
			// std::vector<std::string> index;
			// // bool autoIndex;
			// // std::map<std::vector<int>, std::string> errPage;
			// std::vector<std::string> methodsAllowed;
			
			// std::string path;

std::string Location::getRoot()
{
 return (root);
}
std::vector<std::string> Location::getIndex()
{
 return (index);
}
// // bool getAutoIndex()
// {
//  return (AutoIndex);
// }
// // std::map<std::vector<int>, std::string> getErrPage()
// {
//  return (ErrPage);
// }
// std::vector<std::string> Location::getMethodsAllowed()
// {
//  return (methodsAllowed);
// }

std::string Location::getPath()
{
 return (path);
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

// void Location::setMethodsAllowed(std::vector<std::string> methodsAllowed)
// {
// 	this->methodsAllowed = methodsAllowed;
// }

