#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define TOKEN_NUM     87
#define KEYWORD_NUM   17
#define OPERATOR_NUM  34
#define LIST_SIZE     16
#define TEXT_LENGTH   32
#define STMT_LENGTH   64
#define STR_LENGTH    50000

// 语法类型
typedef enum token {
    ERROR, INT, DOUBLE, CHAR, FLOAT, SHORT, LONG, SIGNED, UNSIGNED, VOID, IF, ELSE, WHILE, FOR, RETURN, BREAK, CONTINUE,
    INCLUDE, DEFINE, IDENTIFIER, INT_LITERAL, FLOAT_LITERAL, CHAR_LITERAL, UNSIGNED_LITERAL, LONG_LITERAL, TYPE, FACTOR,
    ADD, SUB, MUL, DIV, MOD, INCREMENT, DECREMENT, LESS, MORE, LESS_OR_EQUAL, MORE_OR_EQUAL, EQUAL, UNEQUAL, ASSIGN,
    ADD_AND_ASSIGN, SUB_AND_ASSIGN, MUL_AND_ASSIGN, DIV_AND_ASSIGN, MOD_AND_ASSIGN, AND, OR, LP, RP, LS, RS, LC, RC,
    SEMI, COMMA, ENTER, QUOTE, STRING, COMMENT_TEXT, LINE_COMMENT, BLOCK_COMMENT_BEG, BLOCK_COMMENT_END, STMT_BLOCK,
    ERROR_STMT, EX_DEF_LIST, EX_VAR_DEF, VAR_DEF, FUN_DEC, FUN_DEF, INCLUDE_STMT, DEF_STMT, PARAMETER_LIST, IF_STMT,
    IF_ELSE_STMT, IF_SUB_STMT, ELSE_SUB_STMT, WHILE_STMT, WHILE_SUB_STMT, CONDITION, EXPRESSION, LIB_FILE, LITERAL,
    CUSTOM_FILE, RETURN_VAL_TYPE, ARRAY, FUN_CALL
}token;
// 语法单元线性表
typedef struct TokenList {
    token  token[LIST_SIZE]; // 语法类型
    char*  text[LIST_SIZE];  // 单元自身值
    char   line[LIST_SIZE];  // 所在行
    int    cur;              // 顺序表索引
    struct TokenList* next;
}TokenList;
// 关键字
extern const char* keyword[KEYWORD_NUM];
// 运算符
extern const char* op[OPERATOR_NUM];
// 运算符语法类型
extern const token opTk[OPERATOR_NUM];
// 语法类型释义
extern const char* tkInfo[TOKEN_NUM];
// 当前行
extern int line;

// 读取输入文本，创建语法单元线性表
TokenList* readInput(const char* text);
// 为输入字符串匹配语法类型
void matchToken(TokenList** tkListTail, char* s);
// 语法单元线性表尾插入元素
void pushTokenList(TokenList** tkListTail, token t, char* s, int i);
// 释放语法单元线性表内存空间
void freeTokenList(TokenList* tkList);
