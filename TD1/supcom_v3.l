%{ 
    int in_comm = 0; 
    int depth = 0;
    int in_str = 0; 

    void yyerror(const char* msg) {
        fprintf(stderr, "At line %d: %s\n", yylineno, msg);
    }
%}

%%
\\\" if(!in_comm)ECHO;
\/\* {in_comm=1;depth++;putchar(' ');}
\*\/ {if(!depth--){yyerror("Unexpected */ (not in comment)");yyterminate();}if(!depth)in_comm=0;}
\/\/ if(!in_comm){if(!in_str){in_comm = 2;putchar(' ');} else ECHO; }
\n {yylineno++; if(in_comm == 2){in_comm = 0;} ECHO;}
\" if(!in_comm){in_str = !in_str; ECHO;}
<<EOF>> {if(in_comm){yyerror("Unexpected EOF in comment");}yyterminate();}
. if(!in_comm){ECHO;}
%%

int main()  { return yylex(); }
int yywrap() { return 1; }