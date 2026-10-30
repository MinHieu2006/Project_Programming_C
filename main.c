#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrice.h"
#include "matrice_alloc.h"
#include "matrice_io.h"
#include "matrice_calcul.h"

static MatErreur charger(const char *chemin, Matrice *m)
{
    FILE *f = fopen(chemin, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir \"%s\" : %s\n",
                chemin, strerror(errno));
        *m = MATRICE_VIDE;
        return MAT_ERR_LECTURE;
    }
    MatErreur err = mat_lire(f, m);
    fclose(f);
    if (err != MAT_OK)
        fprintf(stderr, "Erreur : lecture de \"%s\" : %s\n",
                chemin, mat_message_erreur(err));
    return err;
}

static MatErreur sauvegarder(const char *chemin, const Matrice *m)
{
    FILE *f = fopen(chemin, "w");
    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible de créer \"%s\" : %s\n",
                chemin, strerror(errno));
        return MAT_ERR_ECRITURE;
    }
    MatErreur err = mat_ecrire(f, m);
    if (fclose(f) == EOF && err == MAT_OK)
        err = MAT_ERR_ECRITURE;
    if (err != MAT_OK)
        fprintf(stderr, "Erreur : écriture de \"%s\" : %s\n",
                chemin, mat_message_erreur(err));
    return err;
}

int main(int argc, char *argv[])
{
    if (argc != 5
        || (strcmp(argv[1], "add") != 0 && strcmp(argv[1], "mul") != 0)) {
        fprintf(stderr, "Usage : %s <add|mul> <fichier_A> <fichier_B> "
                        "<fichier_R>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int addition = (strcmp(argv[1], "add") == 0);
    Matrice a = MATRICE_VIDE, b = MATRICE_VIDE, r = MATRICE_VIDE;
    int statut = EXIT_FAILURE;

    if (charger(argv[2], &a) != MAT_OK || charger(argv[3], &b) != MAT_OK)
        goto nettoyage;

    mat_afficher("A", &a);
    mat_afficher("B", &b);

    MatErreur err = addition ? mat_additionner(&a, &b, &r)
                             : mat_multiplier(&a, &b, &r);
    if (err != MAT_OK) {
        fprintf(stderr, "Erreur de calcul (%d x %d et %d x %d) : %s\n",
                a.nb_lignes, a.nb_colonnes, b.nb_lignes, b.nb_colonnes,
                mat_message_erreur(err));
        goto nettoyage;
    }

    mat_afficher(addition ? "R = A + B" : "R = A x B", &r);
    if (sauvegarder(argv[4], &r) != MAT_OK)
        goto nettoyage;

    printf("Résultat enregistré dans \"%s\".\n", argv[4]);
    statut = EXIT_SUCCESS;

nettoyage:
    mat_detruire(&r);
    mat_detruire(&b);
    mat_detruire(&a);
    return statut;
}
