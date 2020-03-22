#include "LexicalAnalysis.h"

int line;
bool isString = false;
bool isInclude = false;
bool isLineComment = false;
bool isBlockComment = false;

// 关键字
const char* keyword[KEYWORD_NUM] = {
    "\0", "int", "double", "char", "float", "short", "long", "signed", "unsigned", "void",
    "if", "else", "while", "for", "return", "break", "continue"
};
// 运算符
const char* op[OPERATOR_NUM] = {
    "\0", "+", "+=", "++", "-", "-=", "--", "*", "*=", "/", "/=", "%", "%=", "<", "<=", ">", ">=",
    "=", "==", "!=", "&&", "||", "(", ")", "[", "]", "{", "}", ";", ",", "\"", "/*", "*/", "//"
};
// 运算符语法类型
const token opTk[OPERATOR_NUM] = {
    ERROR, ADD, ADD_AND_ASSIGN, INCREMENT, SUB, SUB_AND_ASSIGN, DECREMENT, MUL, MUL_AND_ASSIGN,
    DIV, DIV_AND_ASSIGN, MOD, MOD_AND_ASSIGN, LESS, LESS_OR_EQUAL, MORE, MORE_OR_EQUAL, ASSIGN,
    EQUAL, UNEQUAL, AND, OR, LP, RP, LS, RS, LC, RC, SEMI, COMMA, QUOTE, BLOCK_COMMENT_BEG,
    BLOCK_COMMENT_END, LINE_COMMENT,
};
// 语法类型释义
const char* tkInfo[TOKEN_NUM] = {
    "语法错误", "整型", "双精度浮点型", "字符型", "浮点型", "短整型", "长整型", "有符号整型", "无符号整型", "无类型", "IF", "ELSE", "WHILE", "FOR",
    "返回语句", "停止语句", "继续语句", "头文件包含", "定义", "标识符", "整型字面值", "浮点型字面值", "字符型字面值", "无符号类型字面值", "长整型字面值",
    "类型符", "因子", "加", "减", "乘", "除", "模", "自增", "自减", "小于", "大于", "小于等于", "大于等于", "等于", "不等于", "赋值", "加且赋值",
    "减且赋值", "乘且赋值", "除且赋值", "模且赋值", "逻辑与", "逻辑或", "左括号", "右括号", "左方括号", "右方括号", "左大括号", "右大括号", "分号",
    "逗号", "回车符", "引号", "字符串", "注释文本", "行注释", "块注释首", "块注释尾", "语句块", "错误语句", "外部定义序列", "外部变量定义", "变量定义",
    "函数声明", "函数定义", "头文件包含", "宏定义", "形参列表", "if语句", "if-else语句", "if子句", "else子句", "while语句", "while子句", "条件",
    "表达式", "库文件", "字面值", "用户文件", "返回值类型", "数组", "函数调用"
};

TokenList* readInput(const char* text)
{
    TokenList* tkList = (TokenList*)calloc(1, sizeof(TokenList));
    TokenList* tkListTail = tkList;
    if (!tkList) return NULL;
    char s[STMT_LENGTH] = { 0 };
    int offset = 0, readCnt;
    line = 1;
    isString = false;
    isInclude = false;
    isLineComment = false;
    isBlockComment = false;

    while (offset < strlen(text) && sscanf(text + offset, "%s%n", s, &readCnt) != EOF) {
        matchToken(&tkListTail, s);
        offset += readCnt;
        while (text[offset] && text[offset] == ' ') ++offset;       // 消除串尾空格
        if (text[offset] == '\n') {                                 // tkList添加回车
            matchToken(&tkListTail, "\n");
            ++line;
        }
    }
    return tkList;
}

void matchToken(TokenList** tkListTail, char* s)
{
    if (!s[0]) return;
    int i = 0, t;

    // 回车
    if (s[0] == '\n') {
        pushTokenList(tkListTail, ENTER, s, 1);
        // 行注释
        if (isLineComment) isLineComment = false;
        i = 1;
    }
    // 字符串
    else if (isString) {
        for (i = 1; s[i] && s[i] != '\"'; ++i);
        pushTokenList(tkListTail, STRING, s, i);
    }
    // 头文件包含
    else if (isInclude) {
        // 库文件
        if (s[0] == '<' && s[strlen(s) - 1] == '>')
            pushTokenList(tkListTail, LIB_FILE, s, i = strlen(s));
        // 用户文件
        else if (s[0] == '\"' && s[strlen(s) - 1] == '\"')
            pushTokenList(tkListTail, CUSTOM_FILE, s, i = strlen(s));
        // 语法错误
        else pushTokenList(tkListTail, ERROR, s, i = strlen(s));
        isInclude = false;
    }
    // 注释
    else if (isLineComment)
        pushTokenList(tkListTail, COMMENT_TEXT, s, i = strlen(s));
    // 预编译指令
    else if (s[0] == '#') {
        for (i = 1; isalpha(s[i]); ++i);                // 前i位是字母
        // 包含头文件
        if (!strncmp(s + 1, "include", i)) {
            pushTokenList(tkListTail, INCLUDE, s, i);
            isInclude = true;
        }
        // 宏定义
        else if (!strncmp(s + 1, "define", i))
            pushTokenList(tkListTail, DEFINE, s, i);
        // 语法错误
        else pushTokenList(tkListTail, ERROR, s, i);
    }
    // 标识符&关键字
    else if (isalpha(s[0])) {
        for (i = 0; isalnum(s[i]); ++i);                // 前i位是字母或数字
        // 数组
        if (s[i] == '[') {
            while (s[i] && s[i++] != ']');
            // 缺少右方括号
            if (!s[i]) pushTokenList(tkListTail, ERROR, s, i);
            else pushTokenList(tkListTail, IDENTIFIER, s, i);
        }
        else {
            for (t = KEYWORD_NUM - 1; t; --t)
                if (i == strlen(keyword[t]) && !strncmp(s, keyword[t], i)) break;
            // 关键字
            if (t) pushTokenList(tkListTail, t, s, i);  // t即为语法类型
            // 标识符
            else pushTokenList(tkListTail, IDENTIFIER, s, i);
        }
    }
    // 字面值
    else if (isdigit(s[0])) {
        bool isFloat = false;
        if (s[0] == '0')
            // 十六进制
            if (tolower(s[1]) == 'x') i = 2;
            // 八进制
            else i = 1;
        else i = 0;
        for (; s[i]; ++i) {
            if (s[i] == '.') isFloat = true;
            else if (!isdigit(s[i])) break;             // 前i位是数字或小数点
        }
        // 浮点型
        if (isFloat) pushTokenList(tkListTail, FLOAT_LITERAL, s, i);
        // 无符号整型
        else if (s[i] == 'U') pushTokenList(tkListTail, UNSIGNED_LITERAL, s, ++i);
        // 长整型
        else if (s[i] == 'L') pushTokenList(tkListTail, LONG_LITERAL, s, ++i);
        // 整型
        else pushTokenList(tkListTail, INT_LITERAL, s, i);
    }
    // 运算符
    else {
        for (t = OPERATOR_NUM - 1; t; --t)
            if (!strncmp(s, op[t], i = strlen(op[t]))) break;
        if (t) pushTokenList(tkListTail, opTk[t], s, i);
        else pushTokenList(tkListTail, ERROR, s, i = 1);
        switch (opTk[t]) {
        // 引号
        case QUOTE:
            isString = !isString;
            break;
        // 行注释首
        case LINE_COMMENT:
            isLineComment = true;
            break;
        // 块注释首
        case BLOCK_COMMENT_BEG:
            isBlockComment = true;
            break;
        // 块注释尾
        case BLOCK_COMMENT_END:
            isBlockComment = false;
            break;
        default:
            break;
        }
    }

    matchToken(tkListTail, s + i);                      // 处理余下字段
    return;
}

void pushTokenList(TokenList** tkListTail, token t, char* s, int i)
{
    // 语法单元链表为满
    if ((*tkListTail)->cur == LIST_SIZE) {
        (*tkListTail)->next = (TokenList*)calloc(1, sizeof(TokenList));
        *tkListTail = (*tkListTail)->next;
    }

    (*tkListTail)->token[(*tkListTail)->cur] = t;
    (*tkListTail)->line[(*tkListTail)->cur] = line;
    (*tkListTail)->text[(*tkListTail)->cur] = (char*)calloc(i + 1, sizeof(char));
    strncpy((*tkListTail)->text[(*tkListTail)->cur], s, i);
    (*tkListTail)->cur += 1;
}

void freeTokenList(TokenList* tkList)
{
    if (tkList->next) freeTokenList(tkList->next);
    for (int i = 0; i < tkList->cur; ++i) free(tkList->text[i]);
    free(tkList);
}
