#ifndef UNIHEADER_HPP
# define UNIHEADER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#include <filesystem>
#include <stdio.h>
#include <stdlib.h>

//socket
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <istream>
#include <dirent.h>

#include <sys/stat.h>
#include <cstdio>

void printErr(std::string str);

#endif