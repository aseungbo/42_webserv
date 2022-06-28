#include "uniHeader.hpp"

void printErr(std::string str)
{
	std::cerr << str << std::endl;
}

std::string miniToString(int val)
{
	std::stringstream ssInt;
	ssInt << val;

	return (ssInt.str());
}