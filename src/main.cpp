#include <tuple>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <getopt.h>
#include "include/token.hpp"
#include "include/preproc.hpp"
#include "include/toy_lexer.hpp"
#include "include/keyword_table.hpp"


std::unique_ptr<compiler::lexer::Lexer> lexer {};

/**
 * @brief 编译器初始化
 */
void initialize(std::ifstream& in) {
    std::ostringstream oss;
    oss << in.rdbuf();

    std::string text {oss.str()};
    lexer = std::make_unique<compiler::lexer::ToyLexer>(compiler::preproc::removeAnnotations(text));
}

[[noreturn]]
void printError(std::initializer_list<std::string> msgs) {
    for (const auto& msg : msgs) {
        std::cerr << msg << std::endl;
    }
    exit(1);
}

template<typename T>
inline void checkFileStream(T& fs, std::string error_msg) {
    if (!fs) {
        std::cerr << error_msg << std::endl;
        exit(1);
    }
}

/**
 * @brief   打印版本信息
 */
void printVersion() {
    /*
     * 使用语义版本控制 (SemVer) 原则设置版本号
     * major.minor.patch
     */
    std::cout << "Toy compiler: version 0.1.0" << std::endl;
    std::cout << "This is a toy compiler developed by xh, csx and qsw." << std::endl;
    std::cout << "Have fun with it!" << std::endl;
}

/**
 * @brief 打印帮助信息
 * @param executable_file_name 可执行文件名
 */
void printHelp(const char* const executable_file_name ) {
    std::cout << "Usage: " << executable_file_name << " [options]" << std::endl
        << std::endl
        << "This is a Rust-like programming language compiler." << std::endl
        << "By default, the \'-t\' and \'-s\' options have been configured." << std::endl
        << "The output is stored in \"output.token\" and \"output.dot\"." << std::endl
        << std::endl
        << "Options:" << std::endl
        << "  -h, --help             show help" << std::endl
        << "  -v, -V, --version      show version" << std::endl
        << "  -i, --input filename   set input file (with suffix, must set an input filename)" << std::endl
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
}

auto argumentParsing(int argc, char* argv[]) {
    // 定义长选项
    static const struct option long_options[] = {
        {"help",    no_argument,       NULL, 'h'},
        {"version", no_argument,       NULL, 'v'},
        {"input",   required_argument, NULL, 'i'},
        {"output",  required_argument, NULL, 'o'},
        {"token",   no_argument,       NULL, 't'},
        {"syntax",  no_argument,       NULL, 's'},
        {NULL,      0,                 NULL, 0}  // 结束标志
    };

    int  opt          {};
    int  option_index {0};

    bool flag_default {true};
    bool flag_token   {false};
    bool flag_syntax  {false};

    std::string in_file  {};
    std::string out_file {};

    while ((opt = getopt_long(argc, argv, "hvVi:o:ts", long_options, &option_index)) != -1) {
        switch (opt) {
            default:
                printError({"command-line argument parsing error!"});
            case 'h': // help
                printHelp(argv[0]);
                exit(0);
            case 'v': // version
            case 'V': // version
                printVersion();
                exit(0);
            case 'i': // input
                in_file  = std::string{optarg};
                break;
            case 'o': // output
                out_file = std::string{optarg};
                break;
            case 't': // token
                flag_token   = true;
                flag_default = false;
                break;
            case 's': // syntax
                flag_syntax  = true;
                flag_default = false;
                break;
            case '?': // 无效选项
                printError({
                    "Unknown option or missing argument",
                    "Try \'compiler --help\' for more information."
                });
        } // end switch
    } // end while

    return std::tuple<bool, bool, bool, std::string, std::string>
        {flag_default, flag_syntax, flag_token, in_file, out_file};
}

/**
 * @brief   toy compiler 主函数
 * @details 拼装各组件
 */
int main(int argc, char* argv[]) {
    auto [flag_default, flag_syntax, flag_token, in_file, out_file]
        = argumentParsing(argc, argv);

    std::ifstream in        {};
    std::ofstream out_token {};
    std::ofstream out_parse {};

    if (!in_file.empty()) {
        in.open(in_file);
        checkFileStream(in, std::string{"Failed to open input file."});
    } else {
        printError({"You must set an input filename"});
    }

    if (out_file.empty()) {
        out_token.open("output.token");
        out_parse.open("output.dot");
    } else {
        out_token.open(out_file + std::string{".token"});
        out_parse.open(out_file + std::string{".dot"});
    }
    checkFileStream(out_token, std::string{"Failed to open output file(token)"});
    checkFileStream(out_parse, std::string{"Failed to open output file(parse)"});

    initialize(in);

    return 0;
}
