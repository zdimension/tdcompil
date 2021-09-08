%x IN_STR

%%
<IN_STR>\\([abfnrtv\"]|[0-9]{3}|x[0-9a-f]+)|(\\\n) ECHO;
<IN_STR>[a-z] putchar(*yytext-32);
<INITIAL>\" {BEGIN IN_STR; ECHO;}
<IN_STR>\" {BEGIN INITIAL; ECHO;}
. ECHO;
%%

int main()  { return yylex(); }
int yywrap() { return 1; }