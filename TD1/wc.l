%{
    int numchars = 0;
    int numwords = 0;
    int numlines = 0;
%}

%%
[\n]      { numchars++; numlines++; }
[\r]      { numchars++; }
[^ \t\n]+ { numwords++; numchars += yyleng; }
.         { numchars++; }
%%

int main()  
{
    yylex();
    printf("%7d %7d %7d\n", numlines, numwords, numchars);
    return 0;
}
int yywrap() { return 1; }