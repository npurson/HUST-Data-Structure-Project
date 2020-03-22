#include "SyntaxAnalysis.h"

ErrorList* errList, * errListTail;
// 错误信息
const char* errInfo[] = {
    "读取输入遇到未知错误", "语法分析遇到未知错误", "无法识别的语句", "头文件包含错误", "函数形参错误",
    "预期输入：类型符", "预期输入：标识符", "预期输入：字面值", "缺失：分号", "缺失：括号",
    "缺失：左括号", "缺失：右括号", "缺失：左大括号", "缺失：右大括号", "缺失：操作数"
};

char** syntaxAnalysis(char* text)
{
    static char* analysis[3];
    memset(analysis, 0, sizeof(analysis));
    // 创建语法单元线性表
    TokenList* tkList = readInput(text);
    if (!tkList->cur) return analysis;
    TokenListCur tkListCur;
    tkListCur.pTkList = tkList;
    tkListCur.cur = 0;

    // 创建错误信息线性表
    errList = (ErrorList*)calloc(1, sizeof(ErrorList));
    errListTail = errList;

    analysis[0] = formatCode(tkListCur);
    Ast* T = parseExDefList(&tkListCur);
    analysis[1] = prtAst(T, 0);
    analysis[2] = prtErrorList(errList);

    freeTokenList(tkList);
    freeAst(T);
    freeErrorList(errList);
    return analysis;
}

Ast* parseExDefList(TokenListCur* tkListCur)
{
    if (!tkListCur->pTkList) return NULL;
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    T->tk = FACTOR;
    T->lchild = parseExDef(tkListCur);
    T->mchild = parseExDefList(tkListCur);
    return T;
}

Ast* parseExDef(TokenListCur* tkListCur)
{
    if (!tkListCur->pTkList) return NULL;
    Ast* T;
    switch (getAbstractType(*tkListCur)) {
    // 包含头文件
    case INCLUDE: return parseIncludeStmt(tkListCur);
    // 宏定义
    case DEFINE: return parseDefStmt(tkListCur);
    // 定义或声明
    case TYPE:
        // 函数定义或声明
        if (getNextCur(*tkListCur).pTkList
            && getType(getNextCur(*tkListCur)) == IDENTIFIER
            && getNextCur(getNextCur(*tkListCur)).pTkList
            && getType(getNextCur(getNextCur(*tkListCur))) == LP)
            return parseFunDef(tkListCur);
        // 外部变量定义
        else if (getNextCur(*tkListCur).pTkList
                 && getType(getNextCur(*tkListCur)) == IDENTIFIER)
            return parseExVarDef(tkListCur);
        // 缺少标识符
        else {
            pushErrorList(EXPECT_IDENTIFIER, *tkListCur);
            *tkListCur = getNextStmt(*tkListCur);
        }
        break;
    // 行注释
    case LINE_COMMENT:
        T = (Ast*)calloc(1, sizeof(Ast));
        T->tk = COMMENT_TEXT;
        for (*tkListCur = getNextCur(*tkListCur);
             tkListCur->pTkList && getType(*tkListCur) == COMMENT_TEXT;
             *tkListCur = getNextCur(*tkListCur));
        return T;
    // 块注释
    case BLOCK_COMMENT_BEG:
        T = (Ast*)calloc(1, sizeof(Ast));
        T->tk = COMMENT_TEXT;
        for (*tkListCur = getNextCur(*tkListCur);
             tkListCur->pTkList && getType(*tkListCur) != BLOCK_COMMENT_END;
             *tkListCur = getNextCur(*tkListCur));
        if (tkListCur->pTkList) *tkListCur = getNextCur(*tkListCur);
        return T;
    // 无法识别的语句
    default:
        pushErrorList(UNRECOGNIZED_STMT, *tkListCur);
        *tkListCur = getNextStmt(*tkListCur);
    }
    return NULL;
}

Ast* parseIncludeStmt(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    *tkListCur = getNextCur(*tkListCur);
    T->tk = INCLUDE_STMT;
    T->lchild = createAst(tkListCur);

    if (!T->lchild || (T->lchild->tk != LIB_FILE && T->lchild->tk != CUSTOM_FILE))
        pushErrorList(INCLUDE_ERR, *tkListCur);
    return T;
}

Ast* parseDefStmt(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    *tkListCur = getNextCur(*tkListCur);
    T->tk = DEF_STMT;
    T->lchild = createAst(tkListCur);
    T->mchild = createAst(tkListCur);

    // 缺少标识符
    if (!T->lchild || T->lchild->tk != IDENTIFIER)
        pushErrorList(EXPECT_IDENTIFIER, *tkListCur);
    // 缺少字面值
    if (!T->mchild || abstractType(T->mchild->tk) != LITERAL)
        pushErrorList(EXPECT_LITERAL, *tkListCur);
    return T;
}

Ast* parseFunDef(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    T->tk = FUN_DEC;
    // 返回值类型
    T->lchild = createAst(tkListCur);
    T->lchild->tk = RETURN_VAL_TYPE;
    // 函数名
    T->mchild = (Ast*)calloc(1, sizeof(Ast));
    T->mchild->tk = FACTOR;
    T->mchild->lchild = createAst(tkListCur);
    // 形参列表
    T->mchild->mchild = parseFunParameterList(tkListCur);

    // 函数定义
    if (tkListCur->pTkList && getType(*tkListCur) == LC) {
        T->tk = FUN_DEF;
        T->rchild = parseStmtBlock(tkListCur);
    }
    // 函数声明
    else if (tkListCur->pTkList && getType(*tkListCur) == SEMI)
        *tkListCur = getNextCur(*tkListCur);
    else pushErrorList(MISS_SEMI, *tkListCur);
    return T;
}

Ast* parseFunParameterList(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    *tkListCur = getNextCur(*tkListCur);
    T->tk = PARAMETER_LIST;
    T->lchild = parseFunParameter(tkListCur);
    if (!tkListCur->pTkList) pushErrorList(MISS_RP, *tkListCur);
    else *tkListCur = getNextCur(*tkListCur);
    return T;
}

Ast* parseFunParameter(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    T->tk = FACTOR;

    // 形参类型
    if (!tkListCur->pTkList || getType(*tkListCur) == RP) {
        pushErrorList(EXPECT_TYPE, *tkListCur);
        return T;
    }
    if (getAbstractType(*tkListCur) == TYPE) {
        T->lchild = createAst(tkListCur);
        if (T->lchild->tk == VOID) {
            if (tkListCur->pTkList && getType(*tkListCur) == RP) return T;
            else goto skip;
        }
    }
    else {
        pushErrorList(EXPECT_TYPE, *tkListCur);
        goto skip;
    }

    // 形参标识符
    if (!tkListCur->pTkList || getType(*tkListCur) == RP) {
        pushErrorList(EXPECT_IDENTIFIER, *tkListCur);
        return T;
    }
    if (getType(*tkListCur) == IDENTIFIER)
        T->mchild = createAst(tkListCur);
    else {
        pushErrorList(EXPECT_IDENTIFIER, *tkListCur);
        goto skip;
    }

    if (!tkListCur->pTkList || getType(*tkListCur) == RP);
    else if (tkListCur->pTkList && getType(*tkListCur) == COMMA) {
        *tkListCur = getNextCur(*tkListCur);
        T->rchild = parseFunParameter(tkListCur);
    }
    else {
        skip: pushErrorList(PARAMETER_ERR, *tkListCur);
        while (tkListCur->pTkList && getType(*tkListCur) != RP)
            *tkListCur = getNextCur(*tkListCur);
    }
    return T;
}

Ast* parseExVarDef(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    T->tk = EX_VAR_DEF;
    // 变量类型
    T->lchild = createAst(tkListCur);
    // 变量名
    T->mchild = parseVarList(tkListCur);
    return T;
}

Ast* parseVarDef(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    T->tk = VAR_DEF;
    // 变量类型
    T->lchild = createAst(tkListCur);
    if (!tkListCur->pTkList) {
        pushErrorList(EXPECT_IDENTIFIER, *tkListCur);
        return T;
    }
    // 变量名
    T->mchild = parseVarList(tkListCur);
    return T;
}

Ast* parseVarList(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    T->tk = FACTOR;

    T->lchild = createAst(tkListCur);
    if (!T->lchild || T->lchild->tk != IDENTIFIER)
        pushErrorList(EXPECT_IDENTIFIER, *tkListCur);

    if (tkListCur->pTkList && getType(*tkListCur) == COMMA) {
        *tkListCur = getNextCur(*tkListCur);
        T->mchild = parseVarList(tkListCur);
    }
    else if (tkListCur->pTkList && getType(*tkListCur) == SEMI)
        *tkListCur = getNextCur(*tkListCur);
    else {
        *tkListCur = getNextStmt(*tkListCur);
        pushErrorList(MISS_SEMI, *tkListCur);
    }
    return T;
}

Ast* parseStmtBlock(TokenListCur* tkListCur)
{
    if (!tkListCur->pTkList) return NULL;
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    T->tk = STMT_BLOCK;

    if (getType(*tkListCur) == LC)
        *tkListCur = getNextCur(*tkListCur);
    else pushErrorList(MISS_LC, *tkListCur);
    if (!tkListCur->pTkList) {
        pushErrorList(MISS_RC, *tkListCur);
        return T;
    }

    Ast* root = T, * parent = T;
    while (tkListCur->pTkList && getType(*tkListCur) != RC) {
        root->lchild = parseStmt(tkListCur);
        root->mchild = (Ast*)calloc(1, sizeof(Ast));
        root->mchild->tk = FACTOR;
        parent = root;
        root = root->mchild;
    }
    free(root);
    parent->mchild = NULL;

    if (tkListCur->pTkList)
        *tkListCur = getNextCur(*tkListCur);
    else pushErrorList(MISS_RC, *tkListCur);
    return T;
}

Ast* parseStmt(TokenListCur* tkListCur)
{
    Ast* T;
    switch (getAbstractType(*tkListCur)) {
    case TYPE:
        return parseVarDef(tkListCur);
    case IF:
        return parseIfStmt(tkListCur);
    case WHILE:
        return parseWhileStmt(tkListCur);
    case BREAK: case CONTINUE:
        if (!getNextCur(*tkListCur).pTkList || getType(getNextCur(*tkListCur)) != SEMI)
            pushErrorList(MISS_SEMI, *tkListCur);
        return createAst(tkListCur);
    case RETURN:
        T = createAst(tkListCur);
        if (tkListCur->pTkList && (getType(*tkListCur) == IDENTIFIER || getAbstractType(*tkListCur) == LITERAL))
            T->lchild = createAst(tkListCur);
        if (!tkListCur->pTkList || getType(*tkListCur) != SEMI)
            pushErrorList(MISS_SEMI, *tkListCur);
        return T;
    case LINE_COMMENT:
        T = (Ast*)calloc(1, sizeof(Ast));
        T->tk = COMMENT_TEXT;
        for (*tkListCur = getNextCur(*tkListCur);
             tkListCur->pTkList && getType(*tkListCur) == COMMENT_TEXT;
             *tkListCur = getNextCur(*tkListCur));
        return T;
    case BLOCK_COMMENT_BEG:
        T = (Ast*)calloc(1, sizeof(Ast));
        T->tk = COMMENT_TEXT;
        for (*tkListCur = getNextCur(*tkListCur);
             tkListCur->pTkList && getType(*tkListCur) != BLOCK_COMMENT_END;
             *tkListCur = getNextCur(*tkListCur));
        if (tkListCur->pTkList) *tkListCur = getNextCur(*tkListCur);
        return T;
    default:
        // 函数调用
        if (getType(*tkListCur) == IDENTIFIER
            && getNextCur(*tkListCur).pTkList
            && getType(getNextCur(*tkListCur)) == LP) {
            T = createAst(tkListCur);
            T->tk = FUN_CALL;
            while (tkListCur->pTkList && getType(*tkListCur) != RP)
                *tkListCur = getNextCur(*tkListCur);
            // 缺少右括号
            if (tkListCur->pTkList) *tkListCur = getNextCur(*tkListCur);
            else pushErrorList(MISS_RP, *tkListCur);
            // 缺少分号
            if (tkListCur->pTkList && getType(*tkListCur) == SEMI)
                *tkListCur = getNextCur(*tkListCur);
            else pushErrorList(MISS_SEMI, *tkListCur);
            return T;
        }
        return parseExp(tkListCur);
    }
}

Ast* parseIfStmt(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    *tkListCur = getNextCur(*tkListCur);
    T->tk = IF_STMT;
    // 条件
    if (tkListCur->pTkList && getType(*tkListCur) == LP)
        *tkListCur = getNextCur(*tkListCur);
    else pushErrorList(MISS_LP, *tkListCur);
    T->lchild = parseCondition(tkListCur);

    // if子句
    T->mchild = (Ast*)calloc(1, sizeof(Ast));
    T->mchild->tk = IF_SUB_STMT;
    T->mchild->lchild = parseStmtBlock(tkListCur);

    // else子句
    if (tkListCur->pTkList && getType(*tkListCur) == ELSE) {
        *tkListCur = getNextCur(*tkListCur);
        T->tk = IF_ELSE_STMT;
        T->rchild = (Ast*)calloc(1, sizeof(Ast));
        T->rchild->tk = ELSE_SUB_STMT;
        T->rchild->lchild = parseStmtBlock(tkListCur);
    }
    return T;
}

Ast* parseWhileStmt(TokenListCur* tkListCur)
{
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    *tkListCur = getNextCur(*tkListCur);
    T->tk = WHILE_STMT;
    // 条件
    if (tkListCur->pTkList && getType(*tkListCur) == LP)
        *tkListCur = getNextCur(*tkListCur);
    else pushErrorList(MISS_LP, *tkListCur);
    T->lchild = parseCondition(tkListCur);

    // while子句
    T->mchild = (Ast*)calloc(1, sizeof(Ast));
    T->mchild->tk = WHILE_SUB_STMT;
    T->mchild->lchild = parseStmtBlock(tkListCur);

    return T;
}

Ast* parseExp(TokenListCur* tkListCur)
{
    if (!tkListCur->pTkList) return NULL;
    Ast* op[STMT_LENGTH] = { 0 };                  // 运算符栈
    int opCur = 0;
    Ast* opn[STMT_LENGTH] = { 0 };                 // 操作数栈
    Ast* T0, * T1, * T2;
    int opnCur = 0;
    bool isError = false;

    while (tkListCur->pTkList && opCur + 1 && !isError) {
        if (getType(*tkListCur) == IDENTIFIER || getAbstractType(*tkListCur) == LITERAL)
            opn[opnCur++] = createAst(tkListCur);
        else if (getType(*tkListCur) == SEMI || getType(*tkListCur) == COMMA) break;
        else {
            if (!opCur) op[opCur++] = createAst(tkListCur);
            else {
                switch (getPrecedence(*op[opCur-1], *tkListCur)) {
                case '<':
                    op[opCur++] = createAst(tkListCur);
                    break;
                case '=':
                    if (opCur) op[--opCur] = NULL;
                    else {
                        pushErrorList(MISS_LP, *tkListCur);
                        isError = true;
                    }
                    *tkListCur = getNextCur(*tkListCur);
                    break;
                case '>':
                    if (opnCur) T2 = opn[--opnCur];
                    else isError = true;
                    if (opnCur) T1 = opn[--opnCur];
                    else {
                        pushErrorList(MISS_OPERAND, *tkListCur);
                        isError = true;
                    }
                    if (opCur) T0 = op[--opCur];
                    else isError = true;
                    T0->lchild = T1;
                    T0->mchild = T2;
                    opn[opnCur++] = T0;
                    break;
                case 'x':
                    isError = true;
                    break;
                }
            }
        }
    }

    if (tkListCur->pTkList) *tkListCur = getNextCur(*tkListCur);
    else pushErrorList(MISS_SEMI, *tkListCur);

    while (opCur) {
        if (opnCur) T2 = opn[--opnCur];
        else isError = true;
        if (opnCur) T1 = opn[--opnCur];
        else {
            pushErrorList(MISS_OPERAND, *tkListCur);
            isError = true;
        }
        if (opCur) T0 = op[--opCur];
        else isError = true;
        T0->lchild = T1;
        T0->mchild = T2;
        opn[opnCur++] = T0;
    }

    if (opnCur == 1 && !opCur && !isError) {
        Ast* T = (Ast*)calloc(1, sizeof(Ast));
        T->tk = EXPRESSION;
        T->lchild = opn[0];
        return T;
    }
    else return NULL;
}

Ast* parseCondition(TokenListCur* tkListCur)
{
    if (!tkListCur->pTkList) return NULL;
    Ast* op[STMT_LENGTH] = { 0 };                  // 运算符栈
    int opCur = 0;
    Ast* opn[STMT_LENGTH] = { 0 };                 // 操作数栈
    Ast* T0, * T1, * T2;
    int opnCur = 0;
    int parenthesesCnt = 0;
    bool isError = false;

    while (tkListCur->pTkList && opCur + 1 && !isError) {
        if (getType(*tkListCur) == IDENTIFIER || getAbstractType(*tkListCur) == LITERAL)
            opn[opnCur++] = createAst(tkListCur);
        else if (getType(*tkListCur) == RP && !parenthesesCnt) break;
        else {
            if (getType(*tkListCur) == LP) ++parenthesesCnt;
            if (!opCur) op[opCur++] = createAst(tkListCur);
            else {
                switch (getPrecedence(*op[opCur-1], *tkListCur)) {
                case '<':
                    op[opCur++] = createAst(tkListCur);
                    break;
                case '=':
                    if (opCur) op[--opCur] = NULL;
                    else {
                        pushErrorList(MISS_LP, *tkListCur);
                        isError = true;
                    }
                    *tkListCur = getNextCur(*tkListCur);
                    break;
                case '>':
                    if (opnCur) T2 = opn[--opnCur];
                    else isError = true;
                    if (opnCur) T1 = opn[--opnCur];
                    else {
                        pushErrorList(MISS_OPERAND, *tkListCur);
                        isError = true;
                    }
                    if (opCur) T0 = op[--opCur];
                    else isError = true;
                    T0->lchild = T1;
                    T0->mchild = T2;
                    opn[opnCur++] = T0;
                    break;
                case 'x':
                    isError = true;
                    break;
                }
            }
        }
    }

    if (tkListCur->pTkList) *tkListCur = getNextCur(*tkListCur);
    else pushErrorList(MISS_RP, *tkListCur);

    while (opCur) {
        if (opnCur) T2 = opn[--opnCur];
        else isError = true;
        if (opnCur) T1 = opn[--opnCur];
        else {
            pushErrorList(MISS_OPERAND, *tkListCur);
            isError = true;
        }
        if (opCur) T0 = op[--opCur];
        else isError = true;
        T0->lchild = T1;
        T0->mchild = T2;
        opn[opnCur++] = T0;
    }

    if (opnCur == 1 && !opCur && !isError) {
        Ast* T = (Ast*)calloc(1, sizeof(Ast));
        T->tk = CONDITION;
        T->lchild = opn[0];
        return T;
    }
    else return NULL;
}

Ast* createAst(TokenListCur* tkListCur)
{
    if (!tkListCur->pTkList) return NULL;
    Ast* T = (Ast*)calloc(1, sizeof(Ast));
    T->tk = getType(*tkListCur);
    T->text = getText(*tkListCur);
    *tkListCur = getNextCur(*tkListCur);
    return T;
}

char* prtAst(Ast* T, int indent)
{
    char* s = (char*)calloc(STR_LENGTH, sizeof(char));
    if (!T) return s;
    if (T->tk != FACTOR) {
        for (int i = 0; i < indent; ++i) strcat(s, "\t");
        strcat(s, tkInfo[T->tk]);
        if (isStmt(T->tk)) strcat(s, ":");
        else {
            strcat(s, "\t");
            strcat(s, T->text);
        }
        strcat(s, "\n");
        ++indent;
    }
    strcat(s, prtAst(T->lchild, indent));
    strcat(s, prtAst(T->mchild, indent));
    strcat(s, prtAst(T->rchild, indent));
    return s;
}

char* prtErrorList(ErrorList* errList)
{
    int cnt = 0;
    char* s = (char*)calloc(STR_LENGTH, sizeof(char));
    while (errList) {
        for (int i = 0; i < errList->cur; ++i, ++cnt) {
            char line[16] = { 0 };
            itoa(errList->line[i], line, 10);
            strcat(s, "🔥 ");
            strcat(s, line);
            strcat(s, ": ");
            strcat(s, errInfo[errList->err[i]]);
            strcat(s, "\n");
        }
        errList = errList->next;
    }
    char* h = (char*)calloc(STR_LENGTH, sizeof(char));
    char* n = (char*)calloc(4, sizeof(char));
    itoa(cnt, n, 10);
    strcat(h, "Error(s): ");
    strcat(h, n);
    strcat(h, "\n");
    strcat(h, s);
    free(s);
    free(n);
    return h;
}

void pushErrorList(error err, TokenListCur tkListCur)
{
    errListTail->err[errListTail->cur] = err;
    if (tkListCur.pTkList)
        errListTail->line[errListTail->cur] = tkListCur.pTkList->line[tkListCur.cur];
    else
        errListTail->line[errListTail->cur] = line;
    errListTail->cur += 1;
    if (errListTail->cur == LIST_SIZE) {
        errListTail->next = (ErrorList*)calloc(1, sizeof(ErrorList));
        errListTail = errListTail->next;
    }
}

void freeAst(Ast* T)
{
    if (!T) return;
    freeAst(T->lchild);
    freeAst(T->mchild);
    freeAst(T->rchild);
    free(T);
}

void freeErrorList(ErrorList* errList)
{
    if (errList->next) freeErrorList(errList->next);
    free(errList);
}

char getPrecedence(Ast T, TokenListCur tkListCur)
{
    token op1 = T.tk, op2 = getType(tkListCur);
    switch (op1) {
    case ADD: case SUB:
        switch (op2) {
        case ADD: case SUB: case RP: case LESS:
        case LESS_OR_EQUAL: case MORE_OR_EQUAL:
        case MORE: case EQUAL: case UNEQUAL:
            return '>';
        case MUL: case DIV: case LP:
            return '<';
        default:
            return 'x';
        }
    case MUL: case DIV:
        switch (op2) {
        case ADD: case SUB: case MUL: case DIV:
        case LESS_OR_EQUAL: case MORE_OR_EQUAL:
        case EQUAL: case UNEQUAL: case LESS:
        case RP: case MORE:
            return '>';
        case LP:
            return '<';
        default:
            return 'x';
        }
    case LP:
        switch (op2) {
        case ADD: case SUB: case MUL: case DIV: case LP:
            return '<';
        case RP:
            return '=';
        case LESS: case LESS_OR_EQUAL:
        case MORE: case MORE_OR_EQUAL:
        case EQUAL: case UNEQUAL:
            return '>';
        default:
            return 'x';
        }
    case RP:
        switch (op2) {
        case ADD: case SUB: case MUL: case DIV:
        case LP: case LESS: case LESS_OR_EQUAL:
        case RP: case MORE: case MORE_OR_EQUAL:
        case EQUAL: case UNEQUAL:
            return '>';
        default:
            return 'x';
        }
    case ADD_AND_ASSIGN: case SUB_AND_ASSIGN: case MUL_AND_ASSIGN:
    case DIV_AND_ASSIGN: case MOD_AND_ASSIGN: case ASSIGN:
        switch (op2) {
        case ADD: case SUB: case MUL: case DIV: case LP:
        case ASSIGN: case ADD_AND_ASSIGN: case SUB_AND_ASSIGN:
        case MUL_AND_ASSIGN: case DIV_AND_ASSIGN: case MOD_AND_ASSIGN:
        case LESS: case MORE:
        case LESS_OR_EQUAL: case MORE_OR_EQUAL:
        case EQUAL: case UNEQUAL:
            return '<';
        default:
            return 'x';
        }
    case LESS: case MORE: case LESS_OR_EQUAL: case MORE_OR_EQUAL:
        switch (op2) {
        case ADD: case SUB: case MUL: case DIV: case LP:
            return '<';
        case RP: case LESS: case MORE:
        case LESS_OR_EQUAL: case MORE_OR_EQUAL:
        case EQUAL: case UNEQUAL:
            return '>';
        default:
            return 'x';
        }
    case EQUAL: case UNEQUAL:
        switch (op2) {
        case ADD: case SUB: case MUL: case DIV: case LP: case LESS:
        case MORE: case LESS_OR_EQUAL: case MORE_OR_EQUAL:
            return '<';
        case RP: case EQUAL: case UNEQUAL:
            return '>';
        default:
            return 'x';
        }
    default:
        return 'x';
    }
}
