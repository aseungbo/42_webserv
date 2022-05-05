#include "parseConf.hpp"

int         checkArg(int ac, char **av, std::string& confPath)
{
    if (ac == 2)
		confPath = av[1];
	else if (ac == 1)
		confPath = DEFAULT_PATH;
	else
        return (-1);
    return (0);
}

std::string openConfigfile(std::string confPath)
{
    std::ifstream fin(confPath);
	std::string contents;
    int size; 

	if (fin.is_open())
    {
        fin.seekg(0, std::ios::end);
        size = fin.tellg();
        contents.resize(size);
        fin.seekg(0, std::ios::beg);
        fin.read(&contents[0], size);
    }
	else
    {
        contents.clear();
        return (contents);
    }
	fin.close();
    return (contents);
}

std::vector<Server> makeServers(std::string config)
{
    std::vector<Server> servers;
    std::cout << "servers size: " << servers.size()  << std::endl;

    // find
    // size_t nPos = config.find("fastcgi_pass");
    // if( nPos != std::string::npos )
    // { 
    //     // 찾고자 하는 문자열부터 이후까지 출력
    //     std::string subtext = config.substr(nPos);
    //     std::cout << subtext << std::endl;
    // }
    
    // check server block
    int idx = 0;
    int cnt = 0;
    while (config.find("server {", idx) != std::string::npos)
    {   
        idx = config.find("server {", idx + 1);
        cnt++;
    }
    std::cout << "cnt: " << cnt << std::endl;
    return (servers);
}