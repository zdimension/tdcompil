REPONSES A LA QUESTION 4 (Traces automatiques)
==============================================

Consignes:
  - Essayez de ne pas trop modifier la stucture de ce fichier
  - Vous pouvez copier/coller les parties *significatives* de vos
    modifications (ne copiez des fichiers entiers!!!)
  - le code c'est bien, mais s'il est expliqué c'est mieux.



Sous-question 1: Principe de fonctionnement
-------------------------------------------

*Expliquer ici le principe des modifications apportées au compilateur
pour introduire cette extension.*

Il s'agit ici principalement de changements dans la production du code, pour ajouter
les appels des macros de débogage. Pour les corps des fonctions, on englobe dans un 
nouveau bloc qui contient les appels des macros `(ENTER|LEAVE)_(VOID|FUNC)`. Pour les `return`, on remplace juste la
ligne par un appel à la macro `RETURN(_VALUE)?`.

Pour le cas du `return`, on ajoute au nœud au moment de l'analye sémantique une référence
vers la fonction parent, afin de pouvoir récupérer au moment de la production le type
de retour de la fonction (qui peut être différent du type de l'expression passée 
au return, en cas de cast implicite).


Sous-question 2: Fichier(s) modifiés
------------------------------------



### lexical.l: (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

PAS DE MODIFICATION





### syntax.y:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


PAS DE MODIFICATION




### ast.c/ast.h:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```c
struct s_return_statement {
  [...]
  ast_node *parent_function;    ///< the parent function
};

ast_node *make_return_statement(ast_node *expr)
{
  [...]
  p->parent_function = NULL;
  return (ast_node *) p;
}
```





### analysis.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


```c
void analysis_return_statement(ast_node *node) {
  [...]
  n->parent_function = current_function; // store containing function for later use
```




### prodcode.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```c
void produce_code_function(ast_node *node) {
  [...]
  if (n->body)  {
      if (trace_mode) {
          emit(" {\n");
          indent(+1);
          if (AST_TYPE(node) == void_type)
              emit("ENTER_VOID;");
          else
              emit("ENTER_FUNC(%s);", toy_type_to_string(AST_TYPE(node)));
          emit("\n");
      }
      indent(0);
      code(n->body);
      if (trace_mode) {
          indent(0);
          if (AST_TYPE(node) == void_type)
              emit("LEAVE_VOID;");
          else
              emit("LEAVE_FUNC(%s);", toy_type_to_string(AST_TYPE(node)));
          indent(-1);
          emit("\n}\n");
      }
  }
      
void produce_code_return_statement(ast_node *node) {
  struct s_return_statement *n = (struct s_return_statement *) node;
  if (trace_mode)
  {
      if (n->expr) {
          emit("RETURN_VALUE(%s, ", toy_type_to_string(AST_TYPE(n->parent_function)));
          code_expr_cast(n->expr);
          emit(")");
      } else {
          emit("RETURN");
      }
  } else {
      emit("return");
      if (n->expr) {
          emit(" ");
          code_expr_cast(n->expr);
      }
  }
  emit(";\n");
}
```





### Modifications dans le runtime? (indiquer 'PAS DE MODIFICATION' si non modifié)
_______________________________________________________________________________

```c
// "Enter procedure" -> "Enter function"
#define ENTER_FUNC(_type)   _toy_##_type _res;                               \
                            printf("%*s>> Enter function '%s'\n",    \
                                   _trace_level++, " ", __func__)
```






### Autres modifications? (indiquer 'PAS DE MODIFICATIONS' dans le cas contraire)
_______________________________________________________________________________


PAS DE MODIFICATION



Sous-question 3: Difficultés rencontrées
----------------------------------------

*Si votre extension ne marche pas, ou pas complètement, indiquer ce
qui d’après vous en est la cause.*

Je me suis rendu compte après environ vingt minutes d'écriture de code de préprocesseur
que des macros étaient fournies pour gérer les traces. L'importance de lire un sujet en 
entier avant de commencer à travailler.