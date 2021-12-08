REPONSES A LA QUESTION 3 (Qualificatif const sur les variables Toy)
==============================================

Consignes:
  - Essayez de ne pas trop modifier la stucture de ce fichier
  - Vous pouvez copier/coller les parties *significatives* de vos
    modifications (ne copiez des fichiers entiers!!!)
  - le code c'est bien, mais s'il est expliqué c'est mieux.


Sous-question 1: Principe de la fonctionnement de l'extension
------------------------------------

*Expliquer ici le principe des modifications apportées au compilateur
pour introduire cette extension.*

On rajoute le mot-clé `const` qui peut être placé à la tête d'une déclaration de variables,
et qui rend donc constante chaque variable de la déclaration.

Ainsi, dans la syntaxe, la production `var_decl` est scindée en `var_decl` et `var_decl_inner`,
`var_decl` gérant la logique du spécificateur `const`.

Au moment de l'analyse d'une expression de modification (incrément, décrément, affectation), on 
vérifie si l'opérande modifiée est un identificateur, et si le cas échéant ce dernier se réfère à une
variable constante. Dans ce cas, une erreur est générée.

Aucun changement au niveau de la production n'est nécessaire car on ne transmet pas l'information `const` 
dans le code C (tout est vérifié par le compilateur Toy).



Sous-question 2: Fichier(s) modifiés
------------------------------------



### lexical.l: (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```lex
"const"			return KCONST;
```





### syntax.y:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```bison
%token KCONST

[...]

%type   <ast>   var_decl_inner

[...]

var_decl_inner:       var_decl_inner ',' var init_var
                                        { add_variable_to_decl($1, $3, $4); $$ = $1; }
        |       type var init_var       { $$ = make_var_decl($2, $1, $3); }
        ;

var_decl:	    KCONST var_decl_inner   { $$ = $2; set_var_decl_const($2); }
		|		var_decl_inner			{ $$ = $1; }
		;
```





### ast.c/ast.h:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```c
// .h

struct s_var_decl {
  [...]
  bool is_const;               ///< true if the declaration is constant
};

#define VARDECL_CONST(p)       (((struct s_var_decl*) (p))->is_const)

void set_var_decl_const(ast_node *decl);

// .c

void set_var_decl_const(ast_node *decl) {
    VARDECL_CONST(decl) = true;
}

ast_node *make_var_decl(ast_node *var, ast_node *type, ast_node* expr) {
    [...]
    p->is_const = false;
}
```





### analysis.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```c
void analysis_var_decl(ast_node *node) {
  [...]
  
  FORLIST(p, VARDECL_VARS(node)) {
    [...]
    // If var decl is constant, then all variables in it are constant
    if (VARDECL_CONST(node)) {
        IDENT_IS_CONST(var) = true;
    }
  }
}
void analysis_identifier(ast_node *node){
  [...]
  else if (AST_KIND(var) == k_identifier) {
    [...]
    // If variable is constant, then this identifier is too
    IDENT_IS_CONST(node) = IDENT_IS_CONST(var);
  }
}

void analysis_expression(ast_node *node) {
  [...]

              case preincr:
              case postincr:
                if (AST_KIND(op1) == k_identifier && IDENT_IS_CONST(op1))
                  error_msg(node, "trying to increment or decrement const variable '%s'", IDENT_VAL(op1));
              case uarith:
  [...]
            switch(EXPRESSION_KIND(node)) {
              case assign:
                if (AST_KIND(op1) == k_identifier && IDENT_IS_CONST(op1))
                  error_msg(node, "assignment to const variable '%s'", IDENT_VAL(op1));
                AST_TYPE(node)= AST_TYPE(op2);
                break;
```





### prodcode.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________



PAS DE MODIFICATION



### Modifications dans le runtime? (indiquer 'PAS DE MODIFICATION' si non modifié)
_______________________________________________________________________________



PAS DE MODIFICATION




### Autres modifications? (indiquer 'PAS DE MODIFICATIONS' dans le cas contraire)
_______________________________________________________________________________


PAS DE MODIFICATION



Sous-question 3: Difficultés rencontrées
----------------------------------------

*Si votre extension ne marche pas, ou pas complètement, indiquer ce
qui d’après vous en est la cause.*

