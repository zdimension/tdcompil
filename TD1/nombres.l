%{
    int value = 0;
    int base = 10;
    const char* const NAMES[] = {"Octal", "Decimal", "Hexadecimal"};
    const char* name = NAMES[1];
%}

%%
0x  {base=16;name=NAMES[2];}
0   {base=8;name=NAMES[0];}
[0-9a-fA-F] {
    value*=base;
    char c = *yytext;
    if (c >= 'a' && c <= 'f') c -= 'a' - 10;
    else if (c >= 'A' && c <= 'F') c -= 'A' - 10;
    else c -= '0';
    value += c;
}
\n {printf("%s: %d", name, value);value=0;base=10;name=NAMES[1];ECHO;}
. ECHO;
%%

int main() { return yylex(); }
int yywrap() { return 1; }