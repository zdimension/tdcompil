REPONSES A LA QUESTION 4 (SStructure de contrôle foreach)
=================================

Consignes:
  - Essayez de ne pas trop modifier la stucture de ce fichier
  - Vous pouvez copier/coller les parties *significatives* de vos
    modifications (ne copiez des fichiers entiers!!!)
  - le code c'est bien, mais s'il est expliqué c'est mieux.



Sous-question 1: Principe de fonctionnement
-------------------------------------------

*Expliquer ici le principe des modifications apportées au compilateur
pour introduire cette extension.*

Les parties lexicale et syntaxique étant déjà réalisées, ici on ne modifie que l'analyseur
sémantique et la production.

Pour l'analyse, il convient de vérifier que les éléments de la "liste" du `foreach` sont
tous compatibles (en affectation) avec la variable de boucle.

Pour la production, on génère simplement une déclaration de tableau et une boucle `for`
implémentant le comportement attendu.


Sous-question 2: Fichier(s) modifiés
------------------------------------



### lexical.l: (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________



PAS DE MODIFICATIONS



### syntax.y:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


PAS DE MODIFICATIONS




### ast.c/ast.h:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


PAS DE MODIFICATIONS




### analysis.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```c
void analysis_foreach_statement(ast_node *node) {
    struct s_foreach_statement *n = (struct s_foreach_statement *) node;
    enter_scope();
    // declare foreach variable
    declare_simple_variable(n->var, n->type);
    // check list values are compatible
    FORLIST(p, n->lst)
    {
        analysis(list_item_data(p));
        if (!compatible_types(n->type, list_item_data(p)))
            error_msg(list_item_data(p), "type of list item incompatible with loop variable");
    }
    current_loop += 1;
    // loop body
    analysis(n->body_stat);
    current_loop -= 1;
    leave_scope();
}
```





### prodcode.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```c
void produce_code_foreach_statement(ast_node *node) {
   struct s_foreach_statement *n = (struct s_foreach_statement *) node;

    emit("{\n"); indent(+1);
    // array initialization
    code(n->type); emit(" _lst[] = { ");
    int comma = 0;
    FORLIST(p, n->lst) {
        if (comma++) emit(", ");
        code_expr_cast(list_item_data(p));
    }
    emit(" };\n");
    
    // for header
    indent(0); emit("for (int _idx=0; _idx < %d; _idx++) {\n", comma);
    indent(+1);
    
    // loop variable
    code(n->type); emit(" "); code(n->var); emit(" = _lst[_idx];\n");
    
    indent(0); code(n->body_stat);
    
    indent(-1);
    emit("}\n");
    indent(-1); emit("}\n");
}
```





### Modifications dans le runtime? (indiquer 'PAS DE MODIFICATION' si non modifié)
_______________________________________________________________________________

PAS DE MODIFICATIONS






### Autres modifications? (indiquer 'PAS DE MODIFICATIONS' dans le cas contraire)
_______________________________________________________________________________

PAS DE MODIFICATIONS




Sous-question 3: Difficultés rencontrées
----------------------------------------

*Si votre extension ne marche pas, ou pas complètement, indiquer ce
qui d’après vous en est la cause.*
