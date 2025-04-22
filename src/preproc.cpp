#include "include/preproc.hpp"

namespace compiler::preproc {

/**
 * @brief  删除输入字符流中的注释
 * @param  text 输入字符串（将被原地修改）
 * @return void
 */
void removeAnnotations(std::string& text) {
    std::string result;
    size_t i = 0;
    int depth = 0;
    while (i < text.length()) {
        if (depth == 0 && text[i] == '/' && i + 1 < text.length()) {
            if (text[i + 1] == '/') {
                // 单行注释，跳过直到换行
                i += 2;
                while (i < text.length() && text[i] != '\n') 
                    i++;
            } else if (text[i + 1] == '*') {
                // 块注释开始
                depth++;
                i += 2;
            } else {
                result += text[i++];
            }
        } else if (depth > 0) {
            if (text[i] == '/' && i + 1 < text.length() && text[i + 1] == '*') {
                // 嵌套块注释
                depth++;
                i += 2;
            } else if (text[i] == '*' && i + 1 < text.length() && text[i + 1] == '/') {
                // 块注释结束
                depth--;
                i += 2;
            } else {
                i++;
            }
        } else {
            result += text[i++];
        }
    }
    text = result;
}

} // namespace compiler::preproc
