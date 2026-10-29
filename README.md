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
