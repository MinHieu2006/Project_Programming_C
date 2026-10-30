# TD — Stockage de matrices en C

Bibliothèque modulaire de matrices d'entiers (allocation, lecture/écriture
de fichiers, addition, multiplication) et programme de démonstration
`demo_matrices`.

## Organisation en modules

| Module            | Rôle                                                        | Exercice |
|-------------------|-------------------------------------------------------------|----------|
| `matrice.h/.c`    | Type `Matrice`, `MATRICE_VIDE`, codes `MatErreur`, utilitaires | 1        |
| `matrice_alloc.h/.c` | Création (`mat_allouer`) et destruction (`mat_detruire`) | 1        |
| `matrice_io.h/.c` | Lecture (`mat_lire`), écriture (`mat_ecrire`), affichage    | 2        |
| `matrice_calcul.h/.c` | `mat_additionner`, `mat_multiplier`                     | 3        |
| `main.c`          | Programme `demo_matrices` (point d'entrée)                  | 4        |
| `test_ex*.c`      | Tests fournis (non modifiés)                                | —        |

Principes suivis (cf. cours *La modularisation en langage C*) :

- **Le `.h` est le contrat** : les en-têtes fournis ne sont pas modifiés ;
  chaque `.c` n'implémente que ce que son en-tête promet.
- **Masquage** : les fonctions internes (`lire_entier`, `verifier_parametres`,
  `tient_dans_int`, `charger`, `sauvegarder`) sont `static`.
- **Dépendances dirigées** : le calcul (`matrice_calcul`) ne connaît ni
  `stdio` ni les fichiers ; seules `matrice_io` et `main.c` font des
  entrées/sorties. `main.c` joue le rôle de couche application qui
  orchestre lecture → calcul → écriture.
- **Sortie propre** : une fonction qui échoue laisse `*res == MATRICE_VIDE`
  et ne fuit aucune mémoire ; `main.c` a un point de sortie unique
  (`nettoyage`) qui détruit chaque matrice exactement une fois.
- **Débordements** : les calculs se font en `long long` et sont contrôlés
  avant d'être rangés dans un `int` (`MAT_ERR_DEPASSEMENT`).

## Compilation

```sh
make            # compile et lance les 3 tests, puis compile demo_matrices
make test_ex1   # (ou test_ex2, test_ex3) un seul test
make demo_matrices
make clean
```

Options : `gcc -std=c11 -Wall -Wextra -Wpedantic -g` (aucun avertissement).
Les tests sont compilés avec `-DMAT_TEST_ALLOC` pour simuler des échecs
d'allocation.

## Utilisation

```sh
./demo_matrices <add|mul> <fichier_A> <fichier_B> <fichier_R>
```

Exemples :

```sh
./demo_matrices add donnees/A.txt donnees/B.txt R.txt   # R = A + B (code 0)
./demo_matrices mul donnees/A.txt donnees/C.txt R.txt   # R = A x C (code 0)
./demo_matrices add donnees/A.txt donnees/C.txt R.txt   # dimensions incompatibles (code 1)
./demo_matrices add donnees/A.txt donnees/invalide.txt R.txt  # fichier mal formé (code 1)
```

Format d'un fichier matrice :

```
<nb_lignes> <nb_colonnes>
<coefficients de la ligne 1 séparés par des espaces>
...
```

## Vérification

```sh
valgrind --leak-check=full ./test_ex1   # idem test_ex2, test_ex3, demo_matrices
```

Résultats : `test_ex1` 16/16, `test_ex2` 14/14, `test_ex3` 14/14,
aucune fuite ni erreur mémoire signalée par valgrind, scénarios
`demo_matrices` du workflow GitHub conformes.

## Questions de réflexion

### Exercice 1 — Allocation et destruction

- **Q3.1 (Pointeur pour `mat_detruire`)** : `mat_detruire` prend un pointeur `Matrice *` pour pouvoir modifier directement la variable de l'appelant. Si on passait par valeur, la fonction travaillerait sur une copie : la mémoire serait libérée, mais la variable d'origine ne serait pas remise à `MATRICE_VIDE` et contiendrait encore des adresses invalides.
- **Q3.2 (Nombre d'appels `calloc` et `free`)** : Pour une matrice $n \times p$, il faut $1 + n$ appels à `calloc` (1 pour le tableau de pointeurs et $n$ pour les lignes), et autant d'appels à `free` pour la destruction complète. Si la $k$-ième ligne échoue, il faut libérer les $k$ lignes déjà allouées plus le tableau de pointeurs, soit $k + 1$ libérations.
- **Q3.3 (Double destruction et copie)** : C'est sans danger car `mat_detruire` remet la matrice à `MATRICE_VIDE` à la fin (donc `coef` devient `NULL`). Un deuxième appel ne fait rien grâce au test `if (m->coef != NULL)`. En revanche, avec `Matrice b = a;`, `a` et `b` partagent les mêmes pointeurs. Détruire `a` puis `b` provoque un double `free` sur la même zone mémoire, ce qui est une erreur grave.
- **Q3.4 (Initialisation à `MATRICE_VIDE`)** : Pour garantir que la matrice de l'appelant est dans un état propre dès le départ. Si la fonction renvoie une erreur (par exemple des dimensions invalides), `*res` reste une matrice vide bien définie et non une variable contenant des valeurs aléatoires.

### Exercice 2 — Entrées-sorties

- **Q4.1 (Matrice locale dans `mat_lire`)** : Pour ne pas laisser une matrice incomplète dans `*res` si la lecture échoue en cours de route (fichier tronqué ou mal formé). En cas d'erreur, on détruit la matrice locale `tmp` et `*res` reste vide. On ne transfère le résultat dans `*res` que si toute la lecture a réussi.
- **Q4.2 (Format et principe de lecture)** : Non, la lecture ne vérifie pas les retours à la ligne car `fscanf` ignore les espaces et les sauts de ligne. Ce n'est pas gênant. Le principe « tolérant en lecture, strict en écriture » signifie qu'on accepte des fichiers avec des espacements variés pour être souple, mais qu'on enregistre toujours nos fichiers dans un format canonique et régulier.
- **Q4.3 (Deux matrices dans un même fichier)** : Comme `mat_lire` s'arrête juste après le dernier nombre lu, il suffit d'appeler une deuxième fois `mat_lire(f, &m2)` sur le même flux `f` pour lire la deuxième matrice, sans avoir besoin de rouvrir le fichier.
- **Q4.4 (Rôle de `fflush` et `fclose`)** : `mat_ecrire` appelle `fflush` pour forcer l'écriture immédiate des données sur le disque et vérifier qu'il n'y a pas d'erreur d'écriture (disque plein). Cela ne dispense pas de vérifier `fclose`, car la fermeture du fichier libère les ressources système et peut elle aussi échouer.
### Exercice 3 — Calcul matriciel

- **Q5.1 (Utilisation de `const`)** : `const Matrice *a` empêche de modifier directement les membres de la structure, mais pas les coefficients accessibles par `a->coef[i][j]`. Pour empêcher leur modification, il faudrait également rendre les entiers pointés constants.

- **Q5.2 (Résultat identique à une entrée)** : On refuse que `res` soit égal à `a` ou `b`, car la fonction commence par initialiser `*res` à `MATRICE_VIDE`. Cela ferait perdre les données de la matrice d'entrée et pourrait provoquer une fuite mémoire.

- **Q5.3 (Nombre de multiplications)** : Pour multiplier une matrice de taille \(n \times p\) par une matrice de taille \(p \times q\), il faut \(n \times p \times q\) multiplications. Pour deux matrices de taille \(1000 \times 1000\), cela représente \(1000^3 = 10^9\), soit un milliard de multiplications.

- **Q5.4 (Dépassement d'entiers)** : Calculer directement `int s = a + b` peut provoquer un dépassement d'entier signé avant même qu'on puisse le vérifier. On utilise donc `long long` pour effectuer le calcul, puis on vérifie que le résultat est compris entre `INT_MIN` et `INT_MAX` avant de le convertir en `int`.

### Exercice 4 — Démonstration complète

- **Q6.1 (Affichage des erreurs)** : `mat_lire` renvoie un code d'erreur sans afficher de message. C'est au programme principal de gérer son affichage. Cela permet de réutiliser la bibliothèque dans différents programmes sans imposer une manière de présenter les erreurs.

- **Q6.2 (Vérification de `fclose`)** : Dans `sauvegarder`, il faut vérifier `fclose`, car une erreur d'écriture peut encore apparaître pendant la fermeture du fichier. Dans `charger`, les erreurs de lecture sont déjà prises en charge par `mat_lire`.

- **Q6.3 (Libération des matrices)** : Si l'enregistrement échoue après le calcul, il faut libérer les trois matrices `a`, `b` et `r`. Le programme utilise `goto nettoyage` pour appeler `mat_detruire` sur chacune d'elles, ce qui évite les fuites mémoire.