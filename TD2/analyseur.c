#include <stdbool.h>
#include <stdio.h>
#include "calculatrice.h"

bool E();

bool Ep();

bool T();

bool Tp();

bool F();

int yylval;

typedef int token;

token next;

extern token yylex();

#define EXPECTED(message) return (fprintf(stderr, message, TOKEN_NAMES[next - LAST_ASCII - 1]), false)


bool verify(token tok)
{
    if (next != tok)
    {
        fprintf(stderr, "Invalid token '%c'\n", next);
        return false;
    }
    next = yylex();
    return true;
}

bool E()
{
    switch (next)
    {
        case INT:
        case OPEN:
            return T() && Ep();
        default:
            EXPECTED("Expected INT or OPEN, got %s\n");
    }
}

bool Ep()
{
    switch (next)
    {
        case PLUS:
            if (verify(PLUS) && T())
            {
                printf("PLUS\n");
                return Ep();
            }
            else
            {
                return false;
            }
        case MINUS:
            if (verify(MINUS) && T())
            {
                printf("MINUS\n");
                return Ep();
            }
            else
            {
                return false;
            }
        case EOL:
            printf("PRINT\n");
            return true;
        case CLOSE:
            return true;
        default:
            EXPECTED("Expected PLUS, MINUS, CLOSE or EOL, got %s\n");
    }
}

bool T()
{
    switch (next)
    {
        case INT:
        case OPEN:
            return F() && Tp();
        default:
            EXPECTED("Expected INT or OPEN, got %s\n");
    }
}

bool Tp()
{
    switch (next)
    {
        case EOL:
            return true;
        case PLUS:
        case MINUS:
        case CLOSE:
            return true;
        case MULT:
            if (verify(MULT) && F() && Tp())
            {
                printf("MULT\n");
                return true;
            }
            return false;
        case DIV:
            if (verify(DIV) && F() && Tp())
            {
                printf("DIV\n");
                return true;
            }
            return false;
        default:
            EXPECTED("Expected PLUS, MINUS, CLOSE, EOL, MULT or DIV, got %s\n");
    }
}

bool F()
{
    switch (next)
    {
        case INT:
            printf("PUSH %d\n", yylval);
            return verify(INT);
        case OPEN:
            return verify(OPEN) && E() && verify(CLOSE);
        default:
            EXPECTED("Expected INT or OPEN, got %s\n");
    }
}

int main()
{
    while (true)
    {
        printf("> ");
        next = yylex();
        if (next == EOL)
            continue;
        printf("Résultat analyse : %s\n", E() ? "SUCCÈS" : "ÉCHEC");
    }
}