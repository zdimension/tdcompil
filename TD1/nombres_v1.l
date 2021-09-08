%%
[-+]?(0|[1-9][0-9]*) { printf("NOMBRE"); };
. ECHO;
%%
int main()  { return yylex(); }
int yywrap() { return 1; }