#ifndef PARSECONF_HPP
# define PARSECONF_HPP

# include "uniHeader.hpp"
# include "WebServer.hpp"

int         checkArg(int ac, char **av, std::string& confPath);
std::string openConfigfile(std::string confPath);
std::vector<Server> makeServers(std::string config);

#endif