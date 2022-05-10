#ifndef PARSECONF_HPP
# define PARSECONF_HPP

# include "parseUtil.hpp"
# include "WebServer.hpp"

class Parser
{
		private :
            std::string config;
			std::map<std::string, std::vector<std::string> > keyValueMap;

		public : 
			Parser();
            void openConfigfile(std::string confPath);
            std::vector<std::string> parseServerBlock(std::string config);
            void parseCurrLine(std::string contSplit);
            void parseKeyValue(std::string content);
            void initServer(std::vector<Server>& servers, std::string content);
            std::vector<Server> makeServers();

            std::string getConfig();
            std::map<std::string, std::vector<std::string> > getKeyValueMap();

            void setConfig(std::string config);
            void insertExistKey(std::string key, std::vector<std::string> val);
            void insertNewKey(std::string key, std::vector<std::string> val);
};

#endif