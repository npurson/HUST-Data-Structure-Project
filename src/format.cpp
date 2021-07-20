#include "format.h"


char* formatCode(TokenListCur tkListCur)
{
    char* s = (char*)calloc(STR_LENGTH, sizeof(char));

    for (int indent = 0; tkListCur.pTkList; tkListCur = getNextToken(tkListCur)) {
        if (getNextToken(tkListCur).pTkList && getType(getNextToken(tkListCur)) == RC)
            --indent;
        switch (getType(tkListCur)) {
        case LC:
            ++indent;
        case RC: case SEMI:
            strcat(s, getText(tkListCur));
            if (getNextToken(tkListCur).pTkList && getType(getNextToken(tkListCur)) != ENTER)
                strcat(s, "\n");
            for (int i = indent; i; --i)
                strcat(s, "    ");
            break;
        case ENTER:
            strcat(s, getText(tkListCur));
            for (int i = indent; i; --i)
                strcat(s, "    ");
            break;
        default:
            strcat(s, getText(tkListCur));
            strcat(s, " ");
            break;
        }
    }
    return s;
}


TokenListCur getNextToken(TokenListCur tkListCur)
{
    if (tkListCur.cur + 1 < tkListCur.pTkList->cur)
        tkListCur.cur += 1;
    else {
        tkListCur.pTkList = tkListCur.pTkList->next;
        tkListCur.cur = 0;
    }
    return tkListCur;
}


TokenListCur getNextCur(TokenListCur tkListCur)
{
    tkListCur = getNextToken(tkListCur);
    if (tkListCur.pTkList && getType(tkListCur) == ENTER)
        tkListCur = getNextCur(tkListCur);
    return tkListCur;
}


TokenListCur getNextStmt(TokenListCur tkListCur)
{
    if (!tkListCur.pTkList) return tkListCur;
    while (getNextCur(tkListCur).pTkList && getType(tkListCur = getNextCur(tkListCur)) != SEMI);
    return getNextCur(tkListCur);
}


token getType(TokenListCur tkListCur)
{
    return tkListCur.pTkList->token[tkListCur.cur];
}


token getAbstractType(TokenListCur tkListCur)
{
    switch (getType(tkListCur)) {
    case INT: case DOUBLE: case CHAR: case FLOAT: case SHORT:
    case LONG: case SIGNED: case UNSIGNED: case VOID:
        return TYPE;
    case INT_LITERAL: case FLOAT_LITERAL: case CHAR_LITERAL:
    case UNSIGNED_LITERAL: case LONG_LITERAL:
        return LITERAL;
    default:
        return getType(tkListCur);
    }
}


token abstractType(token tk)
{
    switch (tk) {
    case INT: case DOUBLE: case CHAR: case FLOAT: case SHORT:
    case LONG: case SIGNED: case UNSIGNED: case VOID:
        return TYPE;
    case INT_LITERAL: case FLOAT_LITERAL: case CHAR_LITERAL:
    case UNSIGNED_LITERAL: case LONG_LITERAL:
        return LITERAL;
    default:
        return tk;
    }
}


char* getText(TokenListCur tkListCur)
{
    return tkListCur.pTkList->text[tkListCur.cur];
}


bool isStmt(token t)
{
    switch (t) {
    case RETURN: case BREAK: case CONTINUE: case PARAMETER_LIST: case STMT_BLOCK: case ERROR_STMT:
    case EX_VAR_DEF: case VAR_DEF: case FUN_DEC: case FUN_DEF: case INCLUDE_STMT: case DEF_STMT:
    case COMMENT_TEXT: case IF_STMT: case IF_ELSE_STMT: case IF_SUB_STMT: case ELSE_SUB_STMT:
    case WHILE_STMT: case WHILE_SUB_STMT: case CONDITION: case EXPRESSION:
        return true;
    default:
        return false;
    }
}
