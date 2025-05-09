#include <cstdlib>
#include <iostream>
#include "include/utils.hpp"

namespace utils {

/**
 * @brief 打印错误信息
 * @param msgs 错误信息列表
 */
void printError(std::initializer_list<std::string> msgs) {
    // 使用初始化列表实现变长参数传递功能
    for (const auto& msg : msgs) {
        std::cerr << msg << std::endl;
    }
    exit(1);
}

/**
 * @brief 打印版本信息
 */
void printVersion() {
    /*
     * 使用语义版本控制 (SemVer) 原则设置版本号
     * major.minor.patch
     */
    std::cout << "Toy compiler: version 0.3.0" << std::endl;
    std::cout << "This is a toy compiler developed by xh, csx and qsw." << std::endl;
    std::cout << "Have fun with it!" << std::endl;
}

/**
 * @brief 打印帮助信息
 * @param executable_file_name 可执行文件名
 */
void printHelp(const char* const executable_file_name) {
    std::cout << "Usage: " << executable_file_name << " [options]" << std::endl
        << std::endl
        << "This is a Rust-like programming language compiler." << std::endl
        << "By default, the \'-t\' and \'-p\' options have been configured." << std::endl
        << "The output is stored in \"output.token\" and \"output.dot\"." << std::endl
        << std::endl
        << "Options:" << std::endl
        << "  -h, --help             show help" << std::endl
        << "  -v, -V, --version      show version" << std::endl
        << "  -i, --input filename   set input file (with suffix, must set an input filename)" << std::endl
        << "  -o, --output filename  set output file (without suffix)" << std::endl
        << "  -t, --token            output the segmented token only" << std::endl
        << "  -p, --parse            output the abstract syntax tree (AST) only" << std::endl
        << std::endl
        << "Examples:" << std::endl
        << "  $ path/to/toy_compiler -t -i test.txt" << std::endl
        << "  $ path/to/toy_compiler -p -i test.txt" << std::endl
        << "  $ path/to/toy_compiler -t -p -i test.txt" << std::endl
        << "  $ path/to/toy_compiler -i test.txt -o output" << std::endl
        << std::endl
        << "Tips:" << std::endl
        << "  Upon completion of the program execution, you can run this command" << std::endl
        << "  to generate the abstract syntax tree diagram:" << std::endl
        << "    $ dot -Tpng path/to/output.dot -o AST.png" << std::endl;
}

} // namespace utils
