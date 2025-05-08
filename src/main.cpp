#include <tuple>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <getopt.h>
#include <filesystem>
#include "include/ast.hpp"
#include "include/utils.hpp"
#include "include/parser.hpp"
#include "include/preproc.hpp"
#include "include/toy_lexer.hpp"
#include "include/keyword_table.hpp"


std::unique_ptr<lexer::base::Lexer>   lex  {}; // 词法分析器
std::unique_ptr<parser::base::Parser> pars {}; // 语法分析器

/**
 * @brief 编译器初始化
 */
void initialize(std::ifstream& in) {
    std::ostringstream oss;
    oss << in.rdbuf();
    std::string text {oss.str()};

    lex  = std::make_unique<lexer::impl::ToyLexer>(preproc::removeAnnotations(text));
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
        default:
            utils::printError({"command-line argument parsing error!"});
        case 'h': // help
            utils::printHelp(argv[0]);
            exit(0);
        case 'v': // version
        case 'V': // version
            utils::printVersion();
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
            utils::printError({
                "Unknown option or missing argument",
                "Try \'compiler --help\' for more information."
            });
        } // end switch
    } // end while

    return std::tuple<bool, bool, bool, std::string, std::string>
        {flag_default, flag_token, flag_parse, in_file, out_file};
}

/**
 * @brief 打印分析出的所有 token 到指定文件
 * @param out 输出文件流
 */
void printToken(std::ofstream& out) {
    while(true) {
        if (auto token = lex->nextToken()) {
            if (token.value() == lexer::token::Token::END) {
                break;
            }
            out << token.value().toString() << std::endl;
        } else { // 未正确识别 token
            exit(1);
        }
    }
}

/**
 * @brief 以 dot 格式打印 AST
 * @param out 输出文件流
 */
void printAST(std::ofstream& out) {
    std::function<std::optional<lexer::token::Token>()> nextTokenFunc = []() {
        return lex->nextToken();
    };
    pars = std::make_unique<parser::base::Parser>(nextTokenFunc);

    try {
        auto ast_prog_node = pars->parseProgram();
        std::cout << "Parsing success" << std::endl;

        parser::ast::ast2Dot(out, ast_prog_node);
        //TODO 添加 dot 格式打印 AST 功能
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

    if (!in_file.empty()) {
        in.open(in_file);
        utils::checkFileStream(in, std::string{"Failed to open input file."});
    } else {
        utils::printError({"You must set an input filename"});
    }

    if (out_file.empty()) {
        out_token.open("output.token");
        out_parse.open("output.dot");
    } else {
        out_token.open(out_file + std::string{".token"});
        out_parse.open(out_file + std::string{".dot"});
    }
    utils::checkFileStream(out_token, std::string{"Failed to open output file(token)"});
    utils::checkFileStream(out_parse, std::string{"Failed to open output file(parse)"});

    initialize(in);

    if (flag_default | flag_token) {
        printToken(out_token);
    }
    if (flag_default | flag_parse) {
        lex->reset(0);
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
