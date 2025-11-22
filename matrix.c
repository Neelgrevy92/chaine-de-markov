//
// Created by neelg on 20/11/2025.
//

#include "matrix.h"
#include <math.h>

//matrice pleine de 0
Matrice *creerMatrice(int n)
{
    Matrice *m = malloc(sizeof(Matrice));
    m->taille = n;
    m->data = malloc(n * sizeof(double *));
    for (int i=0; i<n; i++)
    {
        m->data[i] = calloc(n, sizeof(double));
    }
    return m;
}

//conversion liste adjacente -> matrice de proba

Matrice *creerMatriceAdjacence(listeAdjacence *g)
{
    // CORRECTION : Appel de 'creerMatrice' (allocation mémoire)
    Matrice *m = creerMatrice(g->taille);

    for (int i=0; i < g->taille; i++){
        t_arete *curr = g->tab[i]->head;
        while (curr != NULL)
        {
            int source = i;
            // Attention : Vérifie bien que tes sommets dans le fichier
            // commencent à 1. Sinon retire le "-1".
            int destination = curr->sommet_arrivee - 1;

            // Sécurité : Vérifier que destination est dans les bornes
            if (destination >= 0 && destination < g->taille) {
                m->data[source][destination] = (double)curr->proba;
            }

            curr = curr->suiv;
        }
    }
    return m;
}

/*
 * Recopie les valeurs d'une matrice dans une nouvelle
 */
Matrice *copierMatrice(Matrice *src) {
    Matrice *dest = creerMatrice(src->taille);
    for (int i = 0; i < src->taille; i++) {
        for (int j = 0; j < src->taille; j++) {
            dest->data[i][j] = src->data[i][j];
        }
    }
    return dest;
}

/*
 * Multiplication de deux matrices (O(n^3))
 */
Matrice *multiplierMatrices(Matrice *A, Matrice *B) {
    if (A->taille != B->taille) return NULL;
    int n = A->taille;
    Matrice *res = creerMatrice(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += A->data[i][k] * B->data[k][j];
            }
            res->data[i][j] = sum;
        }
    }
    return res;
}

/*
 * Calcule la somme des différences absolues entre deux matrices
 * diff(M, N) = Somme(|Mij - Nij|)
 */
double diffMatrices(Matrice *A, Matrice *B) {
    double diff = 0.0;
    int n = A->taille;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            diff += fabs(A->data[i][j] - B->data[i][j]);
        }
    }
    return diff;
}

//utils

void afficherMatrice(Matrice *m) {
    if (!m) return;
    for (int i = 0; i < m->taille; i++) {
        printf("| ");
        for (int j = 0; j < m->taille; j++) {
            printf("%.2f ", m->data[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}

void libererMatrice(Matrice *m) {
    if (!m) return;
    for (int i = 0; i < m->taille; i++) {
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}


//partie 2

/*
 * Fonction : subMatrix
 * --------------------
 * Crée une petite matrice carrée contenant uniquement les probabilités
 * de transition entre les membres d'une même classe.
 */
Matrice *subMatrix(Matrice *matrice, t_partition *part, int compo_index) {
    // 1. Trouver la classe cible dans la liste chaînée
    t_classe *currClass = part->head;
    for (int i = 0; i < compo_index; i++) {
        if (currClass == NULL) return NULL; // Index hors bornes
        currClass = currClass->suiv;
    }

    // 2. Compter le nombre de sommets dans cette classe
    int k = 0;
    t_cellule_vertex *node = currClass->head;
    while (node != NULL) {
        k++;
        node = node->suiv;
    }

    // 3. Créer un tableau de mapping pour savoir quels indices garder
    // Ex: Si la classe est {2, 5}, on veut garder les indices 1 et 4 (car 0-based)
    int *indices = malloc(sizeof(int) * k);
    node = currClass->head;
    for (int i = 0; i < k; i++) {
        // On stocke l'index matriciel (ID - 1)
        indices[i] = node->sommet->id - 1;
        node = node->suiv;
    }

    // 4. Créer la sous-matrice k x k
    Matrice *sub = creerMatrice(k);

    // 5. Remplir la sous-matrice
    // sub[i][j] correspond à la proba entre le i-ème sommet de la classe
    // et le j-ème sommet de la classe.
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            int originalRow = indices[i];
            int originalCol = indices[j];
            sub->data[i][j] = matrice->data[originalRow][originalCol];
        }
    }

    free(indices);
    return sub;
}



/* --- FONCTIONS BONUS : PÉRIODICITÉ --- */

/*
 * Calcule le Plus Grand Commun Diviseur d'un tableau d'entiers
 */
int gcd(int *vals, int nbvals) {
    if (nbvals == 0) return 0;
    int result = vals[0];
    for (int i = 1; i < nbvals; i++) {
        int a = result;
        int b = vals[i];
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        result = a;
    }
    return result;
}

/*
 * Calcule la période d'une classe (représentée par sa sous-matrice)
 * Adapte du code fourni dans le sujet.
 */
int getPeriod(Matrice *sub_matrix) {
    int n = sub_matrix->taille;
    // On ne peut pas avoir de période supérieure à la taille dans une composante connexe simple
    // Mais par sécurité on teste jusqu'à N

    int *periods = malloc(n * sizeof(int));
    int period_count = 0;

    // On copie la sous-matrice dans une matrice de puissance
    Matrice *power_matrix = copierMatrice(sub_matrix);

    for (int cpt = 1; cpt <= n; cpt++) {
        int diag_nonzero = 0;

        // Vérifie si un élément de la diagonale est > 0
        // (Si on peut revenir sur soi-même en 'cpt' étapes)
        for (int i = 0; i < n; i++) {
            if (power_matrix->data[i][i] > 0.00001f) { // Seuil epsilon
                diag_nonzero = 1;
                break; // Pas besoin de vérifier les autres
            }
        }

        if (diag_nonzero) {
            periods[period_count] = cpt;
            period_count++;
        }

        // Calcul de la puissance suivante :
        Matrice *next = multiplierMatrices(power_matrix, sub_matrix);

        // Mise à jour et nettoyage
        libererMatrice(power_matrix);
        power_matrix = next;
    }

    int p = gcd(periods, period_count);

    free(periods);
    libererMatrice(power_matrix);

    return p;
}