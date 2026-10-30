# TD — Stockage de matrices en C
## Réponses aux questions Q5 et Q6

### Q5.1 — Portée de `const`
Non. `const Matrice *a` empêche de modifier les membres directs de la structure à travers `a` (par exemple `a->nb_lignes = 3` ou `a->coef = autre_adresse`), mais n'interdit pas `a->coef[0][0] = 7`. Le membre `coef` est de type `int **` : les entiers vers lesquels pointent les pointeurs de lignes ne deviennent pas constants du seul fait que la structure est constante. Pour rendre les coefficients non modifiables via l'interface, il faut une représentation ou une interface de lecture en `const int` (par exemple, pour une vue de lecture seule, un tableau de pointeurs `const int *const *` construit convenablement). Changer simplement le paramètre en `const Matrice *` ne suffit pas.

### Q5.2 — Pourquoi interdire `mat_additionner(&a, &b, &a)` ?
La fonction commence par mettre la matrice de sortie `*res` à l'état `MATRICE_VIDE`, avant de calculer. Si `res` désignait aussi `a`, elle écraserait ainsi les informations de `a` (dont son pointeur de mémoire) : les données d'entrée seraient perdues et la mémoire initialement allouée risquerait de fuir. Le contrôle d'alias empêche ce cas. Pour autoriser une addition « sur place », il faudrait une autre implémentation qui préserve l'entrée et maîtrise sa propriété mémoire.

### Q5.3 — Nombre de multiplications
Le résultat du produit de matrices n × p et p × q comporte n × q coefficients. Chaque coefficient nécessite p multiplications. Le total est donc n × p × q. Pour deux matrices 1000 × 1000 : 1000³ = 1 000 000 000 multiplications.

### Q5.4 — Détection des dépassements
Calculer d'abord `int s = a + b` peut provoquer un débordement signé, ce qui entraîne un comportement indéfini en C : on ne peut pas ensuite s'appuyer sur `s < a`. En outre, ce test ne couvre pas correctement tous les cas, notamment les additions de nombres négatifs. Il faut effectuer le calcul dans un type plus large (`long long`) avant de vérifier `INT_MIN <= s <= INT_MAX`, puis convertir vers `int` seulement si le résultat tient.

### Q6.1 — Qui doit afficher les erreurs ?
`mat_lire` appartient à une bibliothèque réutilisable : elle signale l'échec par un code `MatErreur` sans imposer comment le présenter. `main.c` et ses fonctions auxiliaires connaissent le contexte de l'application et peuvent afficher un message sur `stderr`. Une application graphique pourrait ainsi afficher une boîte de dialogue, sans devoir modifier la bibliothèque.

### Q6.2 — Pourquoi contrôler `fclose` en écriture ?
En écriture, des données peuvent encore être en attente dans un tampon ; `fclose` peut révéler un échec final d'écriture. Il est donc indispensable de vérifier son retour dans `sauvegarder`. En lecture, les erreurs de lecture sont normalement remontées par `mat_lire` ; fermer le fichier n'est pas une étape de validation de données. Le code fourni ne teste donc pas `fclose` dans `charger`.

### Q6.3 — Combien de matrices détruire après échec d'enregistrement ?
Trois : `a`, `b` et `r`. Les deux entrées ont été chargées et le calcul a réussi ; toutes trois possèdent alors de la mémoire. Même si `sauvegarder` échoue, `goto nettoyage` mène à `mat_detruire(&r)`, `mat_detruire(&b)` et `mat_detruire(&a)`. Elles sont donc toutes libérées, et le programme retourne `EXIT_FAILURE`.

## Fichiers réalisés
- `matrice_calcul.c` : implémentation complète des opérations.
- `main.c` : implémentation complète du programme de démonstration.

Les fichiers compilent séparément avec `-std=c11 -Wall -Wextra -Wpedantic`. Les tests d'intégration exigent que les modules précédents (`matrice.c`, `matrice_alloc.c`, `matrice_io.c`) aient aussi été terminés.
