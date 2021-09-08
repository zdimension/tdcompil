 /* La commande cat en lex */
%%

%%
int main()  { return yylex(); }
int yywrap() { return 1; }