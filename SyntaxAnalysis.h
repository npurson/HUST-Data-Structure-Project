#ifdef __cplusplus
extern "C" {
#endif

#pragma once
#include "CodeFormat.h"

// 错误类型
typedef enum error {
    READ_ERR, Analysis_ERR, UNRECOGNIZED_STMT, INCLUDE_ERR, PARAMETER_ERR, EXPECT_TYPE, EXPECT_IDENTIFIER,
    EXPECT_LITERAL, MISS_SEMI, MISS_COMMA, MISS_LP, MISS_RP, MISS_LC, MISS_RC, MISS_OPERAND,
}error;
// 抽象语法树
typedef struct Ast {
    token  tk;                  // 语法类型
    char*  text;                // 自身值
    struct Ast* lchild;
    struct Ast* mchild;
    struct Ast* rchild;
}Ast;
// 错误信息线性表
typedef struct ErrorList {
    int    line[LIST_SIZE];     // 错误所在行号
    error  err[LIST_SIZE];      // 错误类型
    int    cur;
    struct ErrorList* next;
}ErrorList;
// 错误信息
extern const char* errInfo[];

// 返回语法分析内容
char** syntaxAnalysis(char* text);
// 解析外部定义序列
Ast* parseExDefList(TokenListCur* tkListCur);
// 解析外部定义
Ast* parseExDef(TokenListCur* tkListCur);
// 解析头文件包含语句
Ast* parseIncludeStmt(TokenListCur* tkListCur);
// 解析宏定义语句
Ast* parseDefStmt(TokenListCur* tkListCur);
// 解析函数定义语句
Ast* parseFunDef(TokenListCur* tkListCur);
// 解析函数形参列表
Ast* parseFunParameterList(TokenListCur* tkListCur);
// 解析函数形参
Ast* parseFunParameter(TokenListCur* tkListCur);
// 解析外部变量定义
Ast* parseExVarDef(TokenListCur* tkListCur);
// 解析变量定义
Ast* parseVarDef(TokenListCur* tkListCur);
// 解析变量序列
Ast* parseVarList(TokenListCur* tkListCur);
// 解析语句块
Ast* parseStmtBlock(TokenListCur* tkListCur);
// 解析语句
Ast* parseStmt(TokenListCur* tkListCur);
// 解析if语句
Ast* parseIfStmt(TokenListCur* tkListCur);
// 解析while语句
Ast* parseWhileStmt(TokenListCur* tkListCur);
// 解析表达式
Ast* parseExp(TokenListCur* tkListCur);
// 解析条件语句
Ast* parseCondition(TokenListCur* tkListCur);
// 根据语法单元链表创建抽象二叉树
Ast* createAst(TokenListCur* tkListCur);
// 打印抽象语法树
char* prtAst(Ast* T, int indent);
// 打印错误信息列表
char* prtErrorList(ErrorList* errList);
// 错误信息线性表尾插入元素
void pushErrorList(error err, TokenListCur tkListCur);
// 释放抽象语法树内存空间
void freeAst(Ast* T);
// 释放错误信息线性表内存空间
void freeErrorList(ErrorList* errList);
// 比较运算符优先级
char getPrecedence(Ast T, TokenListCur tkListCur);

#ifdef __cplusplus
}
#endif
