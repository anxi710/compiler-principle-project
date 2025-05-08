#include <tuple>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <getopt.h>
#include <filesystem>
#include "include/ast.hpp"
#include "include/parser.hpp"
#include "include/preproc.hpp"
#include "include/toy_lexer.hpp"
#include "include/keyword_table.hpp"
#include "include/error_reporter.hpp"

std::unique_ptr<lexer::base::Lexer>   lex      {}; // 词法分析器
std::unique_ptr<parser::base::Parser> pars     {}; // 语法分析器
std::shared_ptr<error::ErrorReporter> reporter {}; // 错误报告器


/**
 * @brief 检查文件流是否正常打开
 * @param fs  fstream/ifstream/ofstream
 * @param msg 错误信息
 */
template<typename T>
inline void checkFileStream(T& fs, std::string msg) {
    static_assert(
        (std::is_same_v<T, std::fstream>  ||
         std::is_same_v<T, std::ofstream> ||
         std::is_same_v<T, std::ifstream>),
        "argument (fs) must be a file stream"
    ); // end static_assert
    if (!fs) { // 此时 error reporter 还未初始化
        std::cerr << msg;
        exit(1);
    } // end if
}

/**
 * @brief 打印版本信息
 */
void printVersion() {
    /*
     * 使用语义版本控制 (SemVer) 原则设置版本号
     * major.minor.patch
     */
    std::cout << "Toy compiler: version 0.4.1" << std::endl
              << "This is a toy compiler developed by xh, csx and qsw." << std::endl
              << "Have fun with it!" << std::endl;
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

/**
 * @brief 编译器初始化
 */
void initialize(std::ifstream& in) {
    std::ostringstream oss;
    oss << in.rdbuf();

    // 初始化错误报告器
    reporter = std::make_shared<error::ErrorReporter>(oss.str()); // 保留原始文本信息

    std::istringstream iss {preproc::removeAnnotations(oss.str())}; // 删除注释

    std::vector<std::string> text {};
    std::string              line {};
    while(std::getline(iss, line)) {
        text.push_back(std::move(line)); // 逐行读取待分析代码
    }

    if (!iss.eof()) {
        reporter->report(
            error::InternalErrorType::FileAccess,
            "编译器初始化时，文件读取错误"
        );
    }

    lex = std::make_unique<lexer::impl::ToyLexer>(std::move(text));
    lex->setErrReporter(reporter);
}

/**
 * @brief  参数解析
 * @param  argc argument counter
 * @param  argv argument vector
 * @return tuple: flag_default, flag_parse, flag_token, in_file, out_file
 */
auto argumentParsing(int argc, char* argv[]) {
    // 定义长选项
    static const struct option long_options[] = {
        {"help",    no_argument,       NULL, 'h'},
        {"version", no_argument,       NULL, 'v'},
        {"input",   required_argument, NULL, 'i'},
        {"output",  required_argument, NULL, 'o'},
        {"token",   no_argument,       NULL, 't'},
        {"parse",   no_argument,       NULL, 'p'},
        {NULL,      0,                 NULL, 0}  // 结束标志
    };

    int  opt          {};  // option
    int  option_index {0};

    bool flag_default {true};  // 默认情况
    bool flag_token   {false}; // 输出 token
    bool flag_parse   {false}; // 输出 AST

    std::string in_file  {}; // 输入文件名
    std::string out_file {}; // 输出文件名

    // 参数解析
    while ((opt = getopt_long(argc, argv, "hvVi:o:tp", long_options, &option_index)) != -1) {
        switch (opt) {
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
        case 'p': // parse
            flag_parse  = true;
            flag_default = false;
            break;
        case '?': // 无效选项
            std::cerr << "解析到未知参数" << std::endl
                << "尝试运行 \'./toy_compiler --help\' 获取更多信息" << std::endl;
            exit(1);
        } // end switch
    } // end while

    if (in_file.length() == 0) {
        std::cerr << "缺失命令行参数: -i/--input" << std::endl;
        exit(1);
    }

    return std::make_tuple(
        flag_default,
        flag_token,
        flag_parse,
        in_file,
        out_file
    );
}

/**
 * @brief 打印分析出的所有 token 到指定文件
 * @param out 输出文件流
 */
void printToken(std::ofstream& out) {
    while(true) {
        if (auto token = lex->nextToken();
            token.has_value()) {
            if (token->getType() == lexer::token::Type::END) {
                break;
            }
            out << token.value().toString() << std::endl;
        } else { // 未正确识别 token
            reporter->report(token.error());
        }
    }
    out.flush();

    if (reporter->hasLexErr()) {
        reporter->displayLexErrs();
    }
}

/**
 * @brief 以 dot 格式打印 AST
 * @param out 输出文件流
 */
void printAST(std::ofstream& out) {
    auto nextTokenFunc = []() {
        return lex->nextToken();
    };
    pars = std::make_unique<parser::base::Parser>(nextTokenFunc);

    try {
        auto ast_prog_node = pars->parseProgram();
        std::cout << "Parsing success" << std::endl;

        parser::ast::ast2Dot(out, ast_prog_node);
    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
    }
}

/**
 * @brief   toy compiler 主函数
 * @details 拼装各组件
 */
int main(int argc, char* argv[]) {
    auto [flag_default, flag_token, flag_parse, in_file, out_file]
        = argumentParsing(argc, argv);

    std::ifstream in        {};
    std::ofstream out_token {};
    std::ofstream out_parse {};

    in.open(in_file);
    checkFileStream(in, std::string{"Failed to open input file."});

    if (out_file.empty()) {
        out_token.open("output.token");
        out_parse.open("output.dot");
    } else {
        out_token.open(out_file + std::string{".token"});
        out_parse.open(out_file + std::string{".dot"});
    }
    checkFileStream(out_token, std::string{"Failed to open output file (token)"});
    checkFileStream(out_parse, std::string{"Failed to open output file (parse)"});

    initialize(in);

    if (flag_default | flag_token) {
        printToken(out_token);
    }
    if (flag_default | flag_parse) {
        lex->reset(lexer::base::Position(0, 0));
        printAST(out_parse);
    }

    in.close();
    out_token.close();
    out_parse.close();

    std::string base = out_file.empty() ? "output" : out_file;
    if (!flag_default) {
        if (flag_token && !flag_parse) {
            std::filesystem::remove(base + ".dot");
        } else if (flag_parse && !flag_token) {
            std::filesystem::remove(base + ".token");
        }
    }

    return 0;
}
