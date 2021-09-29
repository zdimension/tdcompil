%{
    #include <stdio.h>
    #include "calculatrice.h"
    #include "analyseur.c"
%}

%%
" " {}
\#(.*)$ { }
^\!(.*)$ {system(yytext+1);return(EOL);}
quit {exit(0);}
0x[0-9A-Fa-f]+ {sscanf(yytext+2, "%x", &yylval);return(INT);}
0[0-7]+ {sscanf(yytext+1, "%o", &yylval);return(INT);}
[0-9]+ {sscanf(yytext, "%d", &yylval);return(INT);}
\+ return(PLUS);
\- return(MINUS);
\* return(MULT);
\/ return(DIV);
\( return(OPEN);
\) return(CLOSE);
= return(EQUAL);
\n return(EOL);
. {fprintf(stderr, "Invalid token: %s\n", yytext); return(INVALID);}
%%

int yywrap() { return 1; }
