#include <sys/stat.h>
#include <string>
#include <iostream>


std::string checkPath(std::string path)
{
	struct stat buf;

	if (stat(path.c_str(), &buf) == -1)
		return "not ";
	if (S_ISDIR(buf.st_mode))
		return "dir ";
	else if (S_ISREG(buf.st_mode))
		return "file ";
	// else if (S_ISLNK())
	// 	return LINK;
	else
		return ("erro");
}

int main()
{
std::string path;
	while (1)
	{
	std::cin >> path ;
	std::cout << checkPath(path)<<std::endl;
	}
}