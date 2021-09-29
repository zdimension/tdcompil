#ifndef CALC_H
#define CALC_H

#define TOKENS(F) \
    F(INT) \
    F(PLUS) \
    F(MINUS) \
    F(MULT) \
    F(DIV) \
    F(OPEN) \
    F(CLOSE) \
    F(EQUAL) \
    F(EOL) \
    F(INVALID)

#define ENUM_ITEM(X) X,
enum
{
    LAST_ASCII = 257,
    TOKENS(ENUM_ITEM)
};

#define ENUM_NAME(X) #X,
const char* TOKEN_NAMES[] =
{
    TOKENS(ENUM_NAME)
};

#endif