REPONSES A LA QUESTION 2 (Nombres avec séparateurs)
==============================================

Consignes:
  - Essayez de ne pas trop modifier la stucture de ce fichier
  - Vous pouvez copier/coller les parties *significatives* de vos
    modifications (ne copiez des fichiers entiers!!!)
  - le code c'est bien, mais s'il est expliqué c'est mieux.


Sous-question 1: Principales modifications du compilateur
---------------------------------------------------------

*Expliquer ici le principe des modifications apportées au compilateur
pour introduire cette extension.*

Dans l'analyseur lexical, j'ai rajouté une regex matchant des nombres entiers pouvant
contenir des `_`. Cette regex est formée de sorte à n'accepter que les `_` en milieu de 
nombre, et de ne pas accepter plusieurs `_` consécutifs.

Il faut ensuite une logique spécifique pour gérer la conversion d'un tel jeton en nombre,
en copiant yytext dans un buffer intermédiaire, en enlevant les `_` au passage.


Sous-question 2: Fichier(s) modifiés
------------------------------------


### lexical.l: (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

```c
decint_sep        [0-9]+(_[0-9]+)*

[...]

{decint_sep}             {
                          // on alloue un buffer suffisamment grand
                          // pas besoin de +1 après longueur car le résultat est de toute façon
                          // au moins un caractère moins long que l'entrée, vu qu'il contient un _
                          char* buf = malloc(yyleng);
                          char* p = buf;
                          // on copie en ignorant les _
                          for (char* q = yytext; *q; q++) {
                            if (*q != '_') {
                              *p++ = *q;
                            }
                          }
                          // en n'oubliant pas de terminer la chaîne
                          *p = 0;
                          yylval.int_value = strtol(buf, NULL, 0);
                          free(buf);
                          return INTEGER;
                        }

```





### syntax.y:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


PAS DE MODIFICATION




### ast.c/ast.h:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


PAS DE MODIFICATION




### analysis.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________



PAS DE MODIFICATION



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
