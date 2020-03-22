#pragma once
#include "LexicalAnalysis.h"

// 语法单元线性表索引
typedef struct TokenListCur {
    TokenList* pTkList;
    int cur;
}TokenListCur;

// 格式化处理源代码
char* formatCode(TokenListCur tkListCur);
// 获取下一语法单元的语法单元线性表索引
TokenListCur getNextToken(TokenListCur tkListCur);
// 获取下一语法单元的语法单元线性表索引（跳过回车）
TokenListCur getNextCur(TokenListCur tkListCur);
// 获取下一语句的语法单元线性表索引
TokenListCur getNextStmt(TokenListCur tkListCur);
// 获取语法单元线性表索引指向元素的语法类型
token getType(TokenListCur tkListCur);
// 获取语法单元线性表索引指向元素的抽象语法类型
token getAbstractType(TokenListCur tkListCur);
// 获取语法类型的抽象语法类型
token abstractType(token tk);
// 获取语法单元线性表索引指向元素的自身值
char* getText(TokenListCur tkListCur);
// 判断语法类型是否为语句
bool isStmt(token t);
