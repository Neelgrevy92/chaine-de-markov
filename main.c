#include <stdio.h>
#include <stdlib.h>
#include "partie1.h"
#include "partie2.h" // Indispensable pour la partie 2
#include "matrix.h"
int main(void)
{
    // Nom du fichier de test (doit être dans le dossier du projet ou cmake-build-debug)
    const char *filename = "graphe.txt";

    printf("========================================\n");
    printf("      PROJET MARKOV - TEST GLOBAL       \n");
    printf("========================================\n\n");

    /* ---------------------------------------------------------
     * PARTIE 1 : Lecture, Vérification et Dessin
     * --------------------------------------------------------- */
    printf("--- [PARTIE 1] CHARGEMENT ET VERIFICATION ---\n");

    // 1. Lecture du graphe
    printf("Lecture du fichier '%s'...\n", filename);
    listeAdjacence *g = readGraph(filename);
    printf("-> Graphe chargé avec %d sommets.\n\n", g->taille);

    // 2. Affichage console brut
    afficherListeAdjacence(g);

    // 3. Vérification Markov
    printf("\n--- Verification Markov ---\n");
    if (estMarkov(g)) {
        printf("[OK] Le graphe respecte la propriété de Markov (somme probas ~ 1).\n");
    } else {
        printf("[ATTENTION] Ce n'est PAS un graphe de Markov valide.\n");
    }

    // 4. Génération du fichier Mermaid
    printf("\n--- Generation Visuelle ---\n");
    genererMermaid(g, "graphe.mmd");
    printf("-> Fichier 'graphe.mmd' généré. Copiez son contenu sur mermaid.live\n");


    printf("\n\n========================================\n");
    printf("       PARTIE 2 : ALGO DE TARJAN        \n");
    printf("========================================\n");

    // CORRECTION ICI : On récupère la partition !
    t_partition *partition = algoTarjan(g);


    /* ---------------------------------------------------------
     * PARTIE 3 : Calculs Matriciels et Analyse
     * --------------------------------------------------------- */
    printf("\n\n========================================\n");
    printf("   PARTIE 3 - CALCULS & ANALYSES        \n");
    printf("========================================\n");

    // 1. Matrice complète (Déjà fait avant, on le refait proprement)
    printf("Lecture du graphe pour analyse matricielle...\n");
    // Attention : on utilise 'g' (le graphe principal) ou 'gMeteo' selon ce que tu veux tester.
    // Pour tester le bonus Périodicité, utilise 'g' si c'est ton graphe de test périodique,
    // ou 'gMeteo' pour le sujet.
    Matrice *GrandM = creerMatriceAdjacence(g);

    // 2. Analyse par classe (Sous-matrices)
    printf("\n--- Analyse des sous-matrices par classe ---\n");

    t_classe *curr = partition->head;
    int index = 0;

    while (curr != NULL) {
        printf("\n>>> Analyse de la classe %s : { ", curr->nom);
        t_cellule_vertex *n = curr->head;
        while(n) { printf("%d ", n->sommet->id); n = n->suiv; }
        printf("}\n");

        // A. Extraction
        Matrice *subM = subMatrix(GrandM, partition, index);

        // B. Calcul Période
        int periode = getPeriod(subM);
        printf("    -> Periode : %d\n", periode);

        if (periode > 1) {
            printf("    -> [PERIODIQUE] Pas de limite unique (cycle de %d).\n", periode);
        }
        else {
            printf("    -> [APERIODIQUE] Calcul de la distribution stationnaire...\n");

            Matrice *M_curr = copierMatrice(subM);
            Matrice *M_next = NULL;
            double diff = 1.0;
            int iter = 0;

            while (diff > 0.001 && iter < 1000) {
                M_next = multiplierMatrices(M_curr, subM);
                diff = diffMatrices(M_next, M_curr);
                libererMatrice(M_curr);
                M_curr = M_next;
                iter++;
            }

            if (iter < 1000) {
                printf("    -> Distribution stationnaire : [ ");
                // Affiche la ligne 0
                for(int k=0; k<M_curr->taille; k++) printf("%.2f ", M_curr->data[0][k]);
                printf("]\n");
            } else {
                printf("    -> Pas de convergence (probablement transitoire ou instable).\n");
            }
            libererMatrice(M_curr);
        }

        libererMatrice(subM);
        curr = curr->suiv;
        index++;
    }

    libererMatrice(GrandM);
    // Optionnel : libérer partition (non implémenté ici pour gagner du temps)

    printf("\n========================================\n");
    printf("           FIN DU PROJET                \n");
    printf("========================================\n");
    return 0;
}