REPONSES A LA QUESTION 1 (Correction de bug
================================================

Consignes:
  - Essayez de ne pas trop modifier la structure de ce fichier
  - Vous pouvez copier/coller les parties *significatives* de vos
    modifications (ne copiez des fichiers entiers!!!)
  - le code c'est bien, mais s'il est expliqué c'est mieux.


Sous-question 1: Explications sommaires sur les modifications apportées
-----------------------------------------------------------------------

*Expliquer ici le principe des modifications apportées au compilateur
pour permettre la correction du bug*

L'analyse des opérations unaires (++, --, -) vérifiait jusque là que l'opérande était de
type `int`, j'ai modifié pour accepter `float` et modifier le type de retour en 
conséquence.





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


PAS DE MODIFICATION




### analysis.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```c
if (AST_TYPE(op1) != int_type && AST_TYPE(op1) != float_type)
  error_msg(node, "operand of unary %s must be numeric (int or float)", name);
AST_TYPE(node) = AST_TYPE(op1); // type(result) == type(operand)
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