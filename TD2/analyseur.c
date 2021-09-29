#include <stdbool.h>
#include <stdio.h>
#include "calculatrice.h"

bool E(), Ep(), T(), Tp(), F();

int yylval;

typedef int token;

token next;

extern token yylex();

#define EXPECTED(message) (fprintf(stderr, "While parsing %s, expected " message ", got %s: %s\n", __func__, TOKEN_NAMES[next - LAST_ASCII - 1], next == EOL ? "\\n" : yytext), false)


bool verify(token tok)
{
	if (next != tok)
	{
		fprintf(stderr, "Unexpected token '%c'\n", next);
		return false;
	}
	next = yylex();
	return true;
}

bool S()
{
	return E() && (next == EOL || EXPECTED("EOL"));
}

bool E()
{
	switch (next)
	{
		case INT:
		case OPEN:
			return T() && Ep();
		default:
			return EXPECTED("INT or OPEN");
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
			return EXPECTED("PLUS, MINUS, CLOSE or EOL");
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
			return EXPECTED("INT or OPEN");
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
			return EXPECTED("PLUS, MINUS, CLOSE, EOL, MULT or DIV");
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
			return EXPECTED("INT or OPEN");
	}
}

void clear()
{
	if (next != EOL)
		while (yylex() != EOL);
}

int main()
{
	while (true)
	{
		printf("> ");
		next = yylex();
		if (next == EOL)
			continue;
		printf("Résultat analyse : %s\n", S() ? "SUCCÈS" : (clear(), "ÉCHEC"));
	}
}