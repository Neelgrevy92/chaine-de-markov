#ifndef MATRIX_H
#define MATRIX_H

#include "partie1.h"
#include "partie2.h"


// Structure de Matrice
typedef struct {
    int taille;     // n x n
    double **data;  // Tableau 2D de doubles
} Matrice;

// Prototypes
Matrice *creerMatrice(int n);
Matrice *creerMatriceAdjacence(listeAdjacence *g);
Matrice *copierMatrice(Matrice *src);
Matrice *multiplierMatrices(Matrice *A, Matrice *B);
double diffMatrices(Matrice *A, Matrice *B);

// --- ÉTAPE 2 : SOUS-MATRICES ---
/*
 * Extrait la sous-matrice correspondant à la classe n° compo_index
 * dans la partition.
 */
Matrice *subMatrix(Matrice *matrice, t_partition *part, int compo_index);

// --- ÉTAPE 3 (BONUS) : PÉRIODICITÉ ---
int gcd(int *vals, int nbvals);
int getPeriod(Matrice *sub_matrix);


// Fonctions util

void afficherMatrice(Matrice *m);
void libererMatrice(Matrice *m);

#endif //MATRIX_H