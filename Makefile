# Makefile -- TD Stockage de matrices en C
#
# Cibles principales :
#   make test_ex1      -> compile et lance le test de l'exercice 1
#   make test_ex2      -> compile et lance le test de l'exercice 2
#   make test_ex3      -> compile et lance le test de l'exercice 3
#   make demo_matrices -> compile le programme final
#   make all           -> compile tout
#   make clean         -> supprime les fichiers générés

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -g

# Objets de la bibliothèque commune (sans MAT_TEST_ALLOC)
LIB_SRC = matrice.c matrice_alloc.c matrice_io.c matrice_calcul.c
LIB_OBJ = matrice.o matrice_alloc.o matrice_io.o matrice_calcul.o

.PHONY: all clean test_ex1 test_ex2 test_ex3

all: test_ex1 test_ex2 test_ex3 demo_matrices

# ---------------------------------------------------------------
# Exercice 1 : test_ex1  (compile avec -DMAT_TEST_ALLOC)
# ---------------------------------------------------------------
test_ex1: test_ex1.c matrice.c matrice_alloc.c
	$(CC) $(CFLAGS) -DMAT_TEST_ALLOC \
	    test_ex1.c matrice.c matrice_alloc.c -o test_ex1
	./test_ex1

# ---------------------------------------------------------------
# Exercice 2 : test_ex2  (compile avec -DMAT_TEST_ALLOC)
# ---------------------------------------------------------------
test_ex2: test_ex2.c matrice.c matrice_alloc.c matrice_io.c
	$(CC) $(CFLAGS) -DMAT_TEST_ALLOC \
	    test_ex2.c matrice.c matrice_alloc.c matrice_io.c -o test_ex2
	./test_ex2

# ---------------------------------------------------------------
# Exercice 3 : test_ex3  (compile avec -DMAT_TEST_ALLOC)
# ---------------------------------------------------------------
test_ex3: test_ex3.c matrice.c matrice_alloc.c matrice_io.c matrice_calcul.c
	$(CC) $(CFLAGS) -DMAT_TEST_ALLOC \
	    test_ex3.c matrice.c matrice_alloc.c matrice_io.c matrice_calcul.c \
	    -o test_ex3
	./test_ex3

# ---------------------------------------------------------------
# Exercice 4 : demo_matrices  (sans -DMAT_TEST_ALLOC)
# ---------------------------------------------------------------
demo_matrices: main.c $(LIB_SRC)
	$(CC) $(CFLAGS) \
	    main.c $(LIB_SRC) -o demo_matrices

# ---------------------------------------------------------------
# Nettoyage
# ---------------------------------------------------------------
clean:
	rm -f *.o test_ex1 test_ex2 test_ex3 demo_matrices
