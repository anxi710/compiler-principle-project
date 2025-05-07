#include <sstream>
#include "include/ast.hpp"

namespace parser::ast {

// static int cnt = 0;

// struct DotNodeDecl {
//     std::string name;
//     std::string label;

//     DotNodeDecl() = default;
//     explicit DotNodeDecl(const std::string& n, const std::string& l)
//         : name(n), label(l) {}
//     explicit DotNodeDecl(std::string&& n, std::string&& l)
//         : name(std::move(n)), label(std::move(l)) {}

//     std::string toString() {
//         return name + label;
//     }
// };

// static DotNodeDecl printNodeDecl(const std::string& node) {
//     std::ostringstream oss;

//     oss << node << cnt++ << "[label \"" << node << "\"]";

//     auto line {oss.str()};
//     auto pos = line.find_first_of('[');
//     return DotNodeDecl{line.substr(0, pos), line.substr(pos)};
// }

// std::string nodeDecls2Str(const std::initializer_list<DotNodeDecl>& node_decl_list) {
//     std::ostringstream oss;
//     for (auto node_decl : node_decl_list) {
//         oss << node_decl.toString() << std::endl;
//     }
//     return oss.str();
// }

// static 

// static auto Arg2Dot(const ArgPtr& arg) {
//     auto var_decl_body_node = printNodeDecl("VarDeclBody");
//     auto var_type_node = printNodeDecl("VarType");
// }

// static auto funcHeaderDecl2Dot(const FuncHeaderDeclPtr& func_header_decl) {
//     auto func_header_decl_node = printNodeDecl("FuncHeaderDecl");
//     auto fn_node = printNodeDecl("fn");
//     auto id_node = printNodeDecl("<ID>");
//     auto id_name_node = printNodeDecl(func_header_decl->name);

//     for (auto arg : func_header_decl->argv) {
//         auto [nd, ed] = Arg2Dot(arg);
//     }

//     std::string node_decl = nodeDecls2Str({
//         func_header_decl_node,
//         fn_node,
//         id_node,
//         id_name_node
//     });

//     return std::make_tuple<std::string, std::string>(node_decl, edge_decl)
// }

// static auto funcDecl2Dot(const FuncDeclPtr& func_decl) {
//     std::string node_decl {};
//     std::string edge_decl {};

//     std::ostringstream oss;
//     oss << "FuncDecl" << cnt++ << "[label \"FuncDecl\"]" << std::endl;

//     funcHeaderDecl2Dot();

//     return std::make_tuple()
// }

// void ast2Dot(std::ofstream& out, const ProgPtr& prog) {
//     out << "digraph AST {" << std::endl
//         << "    node [shape=ellipse, fontname=\"Courier\"]" << std::endl << std::endl
//         << "    // define node" << std::endl;

//     for (const auto& decl : prog->decls) {

//     }

//     out << "}" << std::endl;
// }

} // namespace parser::ast
