#include "parseUtil.hpp"

// util
std::string& leftTrim(std::string& str, std::string& chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& rightTrim(std::string& str, std::string& chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& trimString(std::string& str, std::string& chars)
{
    
    return leftTrim(rightTrim(str, chars), chars);
}

std::string transferOneSpace(std::string str)
{
    std::string answer;
    int spaceFlag = 0;

    for (unsigned long idx = 0; idx < str.size(); idx++)
    {
        if (str[idx] == ' ')
        {
            if (!spaceFlag)
            {
                answer += str[idx];
                spaceFlag = 1;
            }
        }
        else
        {
            if (spaceFlag)
                spaceFlag = 0;
            answer += str[idx];
        }
    }
    return (answer);
}

std::vector<std::string> charSplit(std::string input, char delimiter)
{
    std::vector<std::string> answer;
    std::stringstream ss(input);
    std::string temp;
 
    while (getline(ss, temp, delimiter))
        answer.push_back(temp);
    return answer;
}

std::vector<std::string> splitCurrLine(std::string currLine)
{
    std::string charsToTrim = " ;";
    std::string cur;

    trimString(currLine, charsToTrim);
    cur = transferOneSpace(currLine);
    std::vector<std::string> lineSplit = charSplit(cur, ' ');
    return (lineSplit);
}