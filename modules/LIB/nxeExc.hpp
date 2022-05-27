#pragma once
#include <exception>
#include <string>
#include <sstream>

#include "nxeExcHead.hpp"
#include "../DEBUG/debug.hpp"

nexusException::nexusException(int line, const char* file) noexcept
    :
    line(line),
    file(file)
{}
const char* nexusException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << " (" << GetOrigin() << ")\n";
    oss << G_CS;
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}
const char* nexusException::GetType() const noexcept
{
    return "nexusBaseException";
}
int nexusException::GetLine() const noexcept
{
    return line;
}
const std::string& nexusException::GetFile() const noexcept
{
    return file;
}
std::string nexusException::GetOrigin() const noexcept
{
    std::ostringstream oss;
    oss << file << "(" << line << ")";
    return oss.str();
}