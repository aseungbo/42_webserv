#ifndef PARSEUTIL_HPP
# define PARSEUTIL_HPP

# include "uniHeader.hpp"

std::string& leftTrim(std::string& str, std::string& chars);
std::string& rightTrim(std::string& str, std::string& chars);
std::string& trimString(std::string& str, std::string& chars);
std::string transferOneSpace(std::string str);
std::vector<std::string> charSplit(std::string input, char delimiter);
std::vector<std::string> splitCurrLine(std::string currLine);

#endif