%%
[a-z] { printf("%c", yytext[0] - 32); };
. ECHO;
%%
int main()  { return yylex(); }
int yywrap() { return 1; }