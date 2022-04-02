#include<string>
#include<vector>
#include<iomanip>
#include<sstream>
//check if string starts with substring
bool startsWith(const std::string& str, const std::string& prefix)
{
    return str.compare(0, prefix.length(), prefix) == 0;
}

//check if string ends with substring
bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

//convert string to lowercase
std::string toLower(const std::string& str)
{
    std::string lowered;
    for (size_t i = 0; i < str.length(); ++i)
    {
        lowered += std::tolower(str[i]);
    }
    return lowered;
}

//convert string to uppercase
std::string toUpper(const std::string& str)
{
    std::string uppered;
    for (size_t i = 0; i < str.length(); ++i)
    {
        uppered += std::toupper(str[i]);
    }
    return uppered;
}

//split string into substrings separated by delimiter
std::vector<std::string> split(const std::string& str, char delimiter)
{
    std::vector<std::string> splitted;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        splitted.push_back(token);
    }
    return splitted;
}

//split string into substrings separated by delimiter
std::vector<std::string> split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> splitted;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter[0]))
    {
        splitted.push_back(token);
    }
    return splitted;
}

//remove whitespace from string
std::string removeWhitespace(const std::string& str)
{
    std::string trimmed;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (!std::isspace(str[i]))
        {
            trimmed += str[i];
        }
    }
    return trimmed;
}

//remove whitespace from string
std::string removeWhitespace(const std::string& str, const std::string& whitespace)
{
    std::string trimmed;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (whitespace.find(str[i]) == std::string::npos)
        {
            trimmed += str[i];
        }
    }
    return trimmed;
}

//remove leading whitespace from string
std::string removeLeadingWhitespace(const std::string& str)
{
    std::string trimmed;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (!std::isspace(str[i]))
        {
            trimmed += str.substr(i);
            break;
        }
    }
    return trimmed;
}

//remove leading whitespace from string
std::string removeLeadingWhitespace(const std::string& str, const std::string& whitespace)
{
    std::string trimmed;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (whitespace.find(str[i]) == std::string::npos)
        {
            trimmed += str.substr(i);
            break;
        }
    }
    return trimmed;
}

//remove trailing whitespace from string
std::string removeTrailingWhitespace(const std::string& str)
{
    std::string trimmed;
    for (int i = str.length() - 1; i >= 0; --i)
    {
        if (!std::isspace(str[i]))
        {
            trimmed = str.substr(0, i + 1);
            break;
        }
    }
    return trimmed;
}

//remove trailing whitespace from string
std::string removeTrailingWhitespace(const std::string& str, const std::string& whitespace)
{
    std::string trimmed;
    for (int i = str.length() - 1; i >= 0; --i)
    {
        if (whitespace.find(str[i]) == std::string::npos)
        {
            trimmed = str.substr(0, i + 1);
            break;
        }
    }
    return trimmed;
}

//replace all occurrences of a substring with another substring
std::string replace(const std::string& str, const std::string& from, const std::string& to)
{
    std::string replaced;
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos)
    {
        replaced += str.substr(0, pos) + to;
        pos += from.length();
    }
    return replaced;
}

//remove all occurrences of a substring
std::string remove(const std::string& str, const std::string& substr)
{
    std::string removed;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos)
    {
        removed += str.substr(0, pos);
        pos += substr.length();
    }
    return removed;
}

//remove all occurrences of a substring
std::string remove(const std::string& str, const char substr)
{
    std::string removed;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] != substr)
        {
            removed += str[i];
        }
    }
    return removed;
}

//remove all occurrences of a substring
std::string remove(const std::string& str, const std::string& substr, const std::string& whitespace)
{
    std::string removed;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos)
    {
        if (whitespace.find(str[pos - 1]) == std::string::npos && whitespace.find(str[pos + substr.length()]) == std::string::npos)
        {
            removed += str.substr(0, pos) + substr;
        }
        pos += substr.length();
    }
    return removed;
}

//remove all occurrences of a substring
std::string remove(const std::string& str, const std::string& substr, const char whitespace)
{
    std::string removed;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos)
    {
        if (str[pos - 1] != whitespace && str[pos + substr.length()] != whitespace)
        {
            removed += str.substr(0, pos) + substr;
        }
        pos += substr.length();
    }
    return removed;
}

//remove all occurrences of a substring
std::string remove(const std::string& str, const char substr, const std::string& whitespace)
{
    std::string removed;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos)
    {
        if (whitespace.find(str[pos - 1]) == std::string::npos && whitespace.find(str[pos + 1]) == std::string::npos)
        {
            removed += str.substr(0, pos) + substr;
        }
        pos += 1;
    }
    return removed;
}

//remove all occurrences of a substring
std::string remove(const std::string& str, const char substr, const char whitespace)
{
    std::string removed;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos)
    {
        if (str[pos - 1] != whitespace && str[pos + 1] != whitespace)
        {
            removed += str.substr(0, pos) + substr;
        }
        pos += 1;
    }
    return removed;
}

//find where substring is in string
size_t find(const std::string& str, const std::string& substr)
{
    return str.find(substr);
}

//find where substring is in string
size_t find(const std::string& str, const std::string& substr, size_t pos)
{
    return str.find(substr, pos);
}

//find where a character is in string
size_t find(const std::string& str, const char substr)
{
    return str.find(substr);
}

//find where a character is in string
size_t find(const std::string& str, const char substr, size_t pos)
{
    return str.find(substr, pos);
}

//is string in string
bool isIn(const std::string& str, const std::string& substr)
{
    return str.find(substr) != std::string::npos;
}

//is string in string
bool isIn(const std::string& str, const std::string& substr, size_t pos)
{
    return str.find(substr, pos) != std::string::npos;
}

//is character in string
bool isIn(const std::string& str, const char substr)
{
    return str.find(substr) != std::string::npos;
}

//is character in string
bool isIn(const std::string& str, const char substr, size_t pos)
{
    return str.find(substr, pos) != std::string::npos;
}

std::string __charlist_for_ints = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
unsigned char charlistToNumber(unsigned char c) {
    if (isIn(__charlist_for_ints, c)) {
        return 255;
    }
    return find(__charlist_for_ints, c);
}