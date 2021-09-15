%{
    int value = 0;
%}

%x OCT
%x HEX
%x DEC

%%
<OCT>[0-7] { value *= 8; value += *yytext - '0'; }
<OCT>\n { printf("Octal: %d\n", value); value = 0; BEGIN INITIAL; }
<DEC>[0-9] { value *= 10; value += *yytext - '0'; }
<DEC>\n { printf("Décimal: %d\n", value); value = 0; }
<HEX>[0-9a-fA-F] {
    value *= 16;
    char c = *yytext;
    if (c >= 'a' && c <= 'f') c -= 'a' - 10;
    else if (c >= 'A' && c <= 'F') c -= 'A' - 10;
    else c -= '0';
    value += c;
}
<HEX>\n { printf("Hexa: %d\n", value); value = 0; BEGIN INITIAL; }
<INITIAL>0 { BEGIN OCT; }
<INITIAL>0x { BEGIN HEX; }
<INITIAL>[0-9] { value = *yytext - '0'; BEGIN DEC; }
. ECHO;
%%

int main() { return yylex(); }
int yywrap() { return 1; }