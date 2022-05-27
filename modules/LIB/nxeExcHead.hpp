#pragma once
#include <exception>
#include <string>

class nexusException : public std::exception
{
public:
    nexusException(int line, const char* file) noexcept;
    const char* what() const noexcept override;
    virtual const char* GetType() const noexcept;
    int GetLine() const noexcept;
    const std::string& GetFile() const noexcept;
    std::string GetOrigin() const noexcept;
private:
    int line;
    std::string file;
protected:
    mutable std::string whatBuffer;
};