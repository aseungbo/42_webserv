#include "parseConf.hpp"

std::string Parser::getConfig()
{
    return (config);
}

void Parser::setConfig(std::string config)
{
    this->config = config;
}           

void Parser::insertExistKey(std::string key, std::vector<std::string> val)
{
    keyValueMap.find(key)->second.insert(keyValueMap.find(key)->second.end(), val.begin(), val.end());
}

void Parser::insertNewKey(std::string key, std::vector<std::string> val)
{
    keyValueMap.insert(std::pair<std::string, std::vector<std::string> >(key, val));
}

void Parser::openConfigfile(std::string confPath)
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
        setConfig(contents);
    }
	else
    {
        contents.clear();
        setConfig(contents);
    }
	fin.close();
}

std::vector<std::string> Parser::parseServerBlock(std::string config)
{
    std::vector<std::string> blockVec;
    unsigned long idx = config.find("server {");
    unsigned long idxNext = 0;

    if (idx == std::string::npos)
    {
        blockVec.clear();
        printErr("Not exist server block.");
        return (blockVec);
    }
    else
    {
        std::string temp;
        while (idx != std::string::npos)
        {   
            idxNext = config.find("server {", idx + 1);
            temp = config.substr(idx, idxNext - idx);
            idx = idxNext;
            blockVec.push_back(temp);
        }
    }
    return (blockVec);
}

void    Parser::parseCurrLine(std::string str)
{
    std::string charsToTrim = " ;";
    std::string currLine;

    trimString(str, charsToTrim);
    currLine = transferOneSpace(str);
    std::vector<std::string> lineSplit = charSplit(currLine, ' ');
    std::string key = lineSplit[0];
    lineSplit.erase(lineSplit.begin());

    if (keyValueMap.find(key) != keyValueMap.end())
    {
        insertExistKey(key, lineSplit);
    }
    else
    {
        insertNewKey(key, lineSplit);   
    }
    lineSplit.clear();
}

void    Parser::parseKeyValue(std::string content)
{
    std::vector<std::string> contSplit = charSplit(content, '\n');

    for (unsigned long i = 0; i < contSplit.size(); i++)
    {   
        // location block line
        if (contSplit[i].find("location") != std::string::npos)
        {   
            while (*(contSplit[++i].end() - 1) == ';')
            {
                // std::cout << "location: " << contSplit[i] << std::endl;
                ;
            }
            i++;
        }
        else if (*(contSplit[i].end() - 1) == ';')
        {
            parseCurrLine(contSplit[i]);
        }
    }
}

void    Parser::initServer(std::vector<Server>& servers, std::string content)
{
    parseKeyValue(content);

    // init server
    for (unsigned long i = 0; i < keyValueMap.find("listen")->second.size(); i++)
    {
        Server serv;

        serv.setHosts(keyValueMap.find("server_name")->second);
        serv.setPort(atoi((keyValueMap.find("listen")->second[i]).c_str()));
        serv.setClientBodySize(atoi((keyValueMap.find("client_max_body_size")->second[0].c_str())));
        // serv.setAllowMethod(keyValueMap.find(""));
        serv.setIndex(keyValueMap.find("index")->second);
        // serv.setErrPage(keyValueMap.find("error_page")->second);
        // serv.setLocation(locations);
        servers.push_back(serv);
    }
    keyValueMap.clear();
}

std::vector<Server> Parser::makeServers()
{
    std::vector<Server> servers;
    std::cout << "server size: " << servers.size() << std::endl;
    std::vector<std::string> serverBlocks = parseServerBlock(config);
    std::vector<std::string>::iterator serverBlock = serverBlocks.begin();

    while (serverBlock != serverBlocks.end())
    {
        initServer(servers, *serverBlock);
        serverBlock++;
    }
    std::cout << "after server size: " << servers.size() << std::endl;
    return (servers);
}