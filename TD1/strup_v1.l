%%
\"(\\([abfnrtv\"]|[0-9]{3}|x[0-9a-f]+)|[^"]|(\\\n))*\" { 
    char c; 
    int escape = 0;
    while ((c = *yytext++)) {
        if (!escape && c >= 'a' && c <= 'z') c -= 32;
        putchar(c);
        escape = c == '\\';
    }
};
. ECHO;
%%
int main()  { return yylex(); }
int yywrap() { return 1; }