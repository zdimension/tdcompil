%{
    #include "calc.c"
%}

%%
" " {}
\#(.*)$ { }
^\!(.*)$ {system(yytext+1);return(EOL);}
quit {yyterminate();}
0x[0-9A-Fa-f]+ {sscanf(yytext+2, "%x", &yylval.val);return(NUMBER);}
0[0-7]+ {sscanf(yytext+1, "%o", &yylval.val);return(NUMBER);}
[0-9]+ {sscanf(yytext, "%d", &yylval.val);return(NUMBER);}
[A-Za-z_0-9]+ {yylval.var=strndup(yytext,yyleng);return(IDENT);}
\+ return(PLUS);
\- return(MINUS);
\* return(MULT);
\/ return(DIV);
\( return(OPEN);
\) return(CLOSE);
= return(EQUAL);
\n return(EOL);
%%

int yywrap() { return 1; }
