# The Toy compiler 

Toy est un mini langage de programmation, développé pour le cours de compilation de Polytech Nice Sophia. Il s'inspire du langage Ptiloo qui avait été défini à l'origine par Jacques Farré.

<!-- TODO: plus de ref ici --> 

Organisation du source du compilateur:

- `src`: les fichiers sources du compilateur
- `lib`: une petite bibliothèque C (indépendante du compilateur)
   implémentant des tables de hash et des listes d'objets quelconques.
- `runtime`: le répertoire contenant le header à insérer pour compiler
   un programme toy une fois traduis en C et le support d'exécution
   qui doit être lié à ce programme lors de la construction de
   l'exécutable.
- `tests`: un répertoire contenant des programmes de test (justes ET faux)
- `doc`: le répertoire de la doc Doxygen


------ 
Erick Gallesio

