#include <iostream>
#include <memory>
#include <getopt.h>
#include "include/token.hpp"
#include "include/toy_lexer.hpp"
#include "include/keyword_table.hpp"

// 定义长选项
struct option long_options[] = {
    {"help",    no_argument,       NULL, 'h'},
    {"version", no_argument,       NULL, 'v'},
    {"input",   required_argument, NULL, 'i'},
    {"output",  required_argument, NULL, 'o'},
    {"token",   no_argument,       NULL, 't'},
    {"syntax",  no_argument,       NULL, 's'},
    {NULL,      0,                 NULL, 0}  // 结束标志
};

int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "hvVo:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                std::cout << "Usage: " << argv[0] << " [options]" << std::endl
                          << std::endl
                          << "This is a Rust-like programming language compiler." << std::endl
                          << "By default, the \'-t\' and \'-s\' options have been configured." << std::endl
                          << "The output is stored in \"output.token\" and \"output.dot\"." << std::endl
                          << std::endl
                          << "Options:" << std::endl
                          << "  -h, --help             show help" << std::endl
                          << "  -v, -V, --version      show version" << std::endl
                          << "  -i, --input filename   set input file" << std::endl
                          << "  -o, --output filename  set output file (without suffix)" << std::endl
                          << "  -t, --token            output the segmented token only" << std::endl
                          << "  -s, --syntax           output the abstract syntax tree (AST) only" << std::endl
                          << std::endl
                          << "Examples:" << std::endl
                          << "  $ path/to/toy_compiler -t -i test.txt" << std::endl
                          << "  $ path/to/toy_compiler -s -i test.txt" << std::endl
                          << "  $ path/to/toy_compiler -t -s -i test.txt" << std::endl
                          << "  $ path/to/toy_compiler -i test.txt -o output" << std::endl
                          << std::endl
                          << "Tips:" << std::endl
                          << "  Upon completion of the program execution, you can run this command" << std::endl
                          << "  to generate the abstract syntax tree diagram:" << std::endl
                          << "    $ dot -Tpng path/to/output.dot -o AST.png" << std::endl;
                return 0;
            case 'v':
            case 'V':
                std::cout << "Toy compiler: version 0.02" << std::endl;
                std::cout << "This is a toy compiler developed by xh, csx and qsw." << std::endl;
                std::cout << "Have fun with it!" << std::endl;
                return 0;
            case 'i':
                freopen(optarg, "r", stdin);
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
