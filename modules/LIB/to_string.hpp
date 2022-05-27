#pragma once
#include <string>
#include <sstream>
#include <vector>
inline std::string to_string(const std::string& str) {
    return str;
}
inline std::string to_string(const char* str) {
    return std::string(str);
}
inline std::string to_string(const wchar_t* str) {
    std::wstring wstr(str);
    return std::string(wstr.begin(), wstr.end());
}
inline std::string to_string(unsigned long long i) {
    return std::to_string(i);
}
inline std::string to_string(long long i) {
    return std::to_string(i);
}
inline std::string to_string(unsigned long i) {
    return std::to_string(i);
}
inline std::string to_string(long i) {
    return std::to_string(i);
}
inline std::string to_string(unsigned int i) {
    return std::to_string(i);
}
inline std::string to_string(int i) {
    return std::to_string(i);
}
inline std::string to_string(unsigned short i) {
    return std::to_string(i);
}
inline std::string to_string(short i) {
    return std::to_string(i);
}
inline std::string to_string(unsigned char i) {
    std::string s;
    s.push_back(i);
    return s;
}
inline std::string to_string(char i) {
    std::string s;
    s.push_back(i);
    return s;
}
inline std::string to_string(bool i) {
    return i ? "true" : "false";
}
inline std::string to_string(float i) {
    return std::to_string(i);
}
inline std::string to_string(double i) {
    return std::to_string(i);
}
inline std::string to_string(long double i) {
    return std::to_string(i);
}
inline std::string to_string(void* i) {
    std::stringstream ss;
    ss << std::hex << "0x" << std::uppercase <<i;
    return ss.str();
}
template <typename T>
inline std::string to_string(std::vector<T> v) {
    if (v.size() == 0) {
        return "[]";
    }
    std::stringstream ss;
    ss << "[";
    for (auto& i : v) {
        ss << to_string(i) << ",";
    }
    ss.seekp(-1,ss.cur);
    ss << "]";
    return ss.str();
}