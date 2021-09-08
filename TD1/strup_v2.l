%{ 
    int in_str = 0; 
%}

%%
\\([abfnrtv\"]|[0-9]{3}|x[0-9a-f]+)|(\\\n) ECHO;
[a-z] putchar(*yytext-32*in_str);
\" {in_str = !in_str; ECHO;}
. ECHO;
%%

int main()  { return yylex(); }
int yywrap() { return 1; }