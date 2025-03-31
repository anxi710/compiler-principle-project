#include <iostream>
#include <memory>
#include <getopt.h>
#include "include/token.hpp"
#include "include/toy_lexer.hpp"
#include "include/keyword_table.hpp"

// 定义长选项
struct option long_options[] = {
    {"help",    no_argument,       nullptr, 'h'},
    {"version", no_argument,       nullptr, 'v'},
    {"output",  required_argument, nullptr, 'o'},
    {0, 0, 0, 0}  // 结束标志
};

int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "hvVo:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                std::cout << "Usage: " << argv[0] << " <option>..." << std::endl
                          << "Options:" << std::endl
                          << "  -h, --help         show help" << std::endl
                          << "  -v, -V, --version  show version" << std::endl
                          << "  -o, --output FILE  set output file" << std::endl;
                return 0;
            case 'v':
            case 'V':
                std::cout << "Toy compiler: version 0.01" << std::endl;
                std::cout << "This is a toy compiler developed by xh, csx and qsw." << std::endl;
                std::cout << "Have fun with it!" << std::endl;
                break;
            case 'o':
                freopen(optarg, "w", stdout);
                break;
            case '?':  // 无效选项
                std::cerr << "Unknown option or missing argument" << std::endl;
                std::cerr << "Try \'compiler --help\' for more information." << std::endl;
                return 1;
        }
    }

    return 0;
}
