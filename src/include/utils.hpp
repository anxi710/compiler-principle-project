#pragma once

#include <string>

namespace utils {

[[noreturn]]
void printError(std::initializer_list<std::string> msgs);

/**
 * @brief 检查文件流是否正常打开
 * @param fs        fstream/ifstream/ofstream
 * @param error_msg 错误信息
 */
template<typename T>
inline void checkFileStream(T& fs, std::string error_msg) {
    if (!fs) {
        std::cerr << error_msg << std::endl;
        exit(1);
    }
}

void printVersion();

void printHelp(const char* const executable_file_name);

} // namespace utils
