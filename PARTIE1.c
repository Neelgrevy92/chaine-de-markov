#include "PARTIE1.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


//venant de utils.c du professeur
static char *getID(int i)
{
    static char buffer[10];
    char temp[10];
    int index = 0;

    i--; // Adjust to 0-based index
    while (i >= 0)
    {
        temp[index++] = 'A' + (i % 26);
        i = (i / 26) - 1;
    }

    // Reverse the string to get the correct order
    for (int j = 0; j < index; j++)
    {
        buffer[j] = temp[index - j - 1];
    }
    buffer[index] = '\0';

    return buffer;
}


/*
 * Fonction : creerArete
 * ---------------------
 * Crée une arête (Arete) qui relie le sommet courant à un sommet d'arrivée
 * avec une probabilité associée.
 *
 * sommet_arrivee : numéro du sommet de destination
 * proba          : probabilité de transition
 * suiv           : pointeur vers l'arête suivante (utilisé si on veut chaîner directement)
 *
 * Retour : un pointeur vers la nouvelle arête allouée dynamiquement
 */
t_arete * creerArete(int sommet_arrivee, float proba, t_arete * suiv) {
    t_arete *nvlArete = malloc(sizeof(t_arete));
    if (nvlArete == NULL) {
        perror("L'allocation mémoire a échouée. \n");
        exit(EXIT_FAILURE);
    }
    nvlArete->sommet_arrivee = sommet_arrivee;
    nvlArete->proba = proba;
    nvlArete->suiv = NULL; // Toujours terminer la liste par NULL
    return nvlArete;
}

/*
 * Fonction : creerSommet
 * ----------------------
 * Crée un sommet vide (sans arêtes pour l'instant).
 *
 * Retour : un pointeur vers le sommet nouvellement créé
 */
t_sommet * creerSommet() {
    t_sommet * nvSommet = malloc(sizeof(t_sommet)); // allocation mémoire
    if (nvSommet == NULL) {
        perror("L'allocation mémoire a échouée. \n");
        exit(EXIT_FAILURE);
    }
    nvSommet->head = NULL; // aucune arête au départ
    return nvSommet;
}

/*
 * Fonction : ajouterArete
 * -----------------------
 * Ajoute une arête à la liste des arêtes d'un sommet.
 * Si le sommet n'a pas encore d'arêtes, la nouvelle arête devient la tête.
 * Sinon, elle est ajoutée à la fin de la liste chaînée.
 */
void ajouterArete(t_arete *arete, t_sommet *sommet) {
    if (sommet->head == NULL) {
        sommet->head = arete;
        arete->suiv = NULL;
        return;
    }

    t_arete * curr = sommet->head;
    while (curr->suiv != NULL) {
        curr = curr->suiv;
    }
    curr->suiv = arete;
    arete->suiv = NULL;
}

/*
 * Fonction : affihcerSommet
 * -------------------------
 * Affiche toutes les arêtes d'un sommet sous forme :
 * [head @] -> (sommet_arrivee, probabilité) -> ... -> NULL
 */
void afficherSommet(t_sommet *sommet) {
    t_arete * curr = sommet->head;
    printf("[head] -> ");
    while (curr != NULL) {
        printf("(vers %d, p=%.2f) -> ", curr->sommet_arrivee, curr->proba);
        curr = curr->suiv;
    }
    printf("NULL\n");
}

/*
 * Fonction : creerListeAdjacence
 * -------------------------------
 * Crée une liste d'adjacence vide pour un graphe de 'taille' sommets.
 *
 * Retour : un pointeur vers la liste d'adjacence
 */
listeAdjacence * creerListeAdjacence(int taille) {
    listeAdjacence * adjList = malloc(sizeof(listeAdjacence));
    if (adjList == NULL) {
        perror("L'allocation mémoire a échouée. \n");
        exit(EXIT_FAILURE);
    }

    adjList->taille = taille;
    adjList->tab = malloc(sizeof(t_sommet *) * taille);
    if (adjList->tab == NULL) {
        free(adjList);
        perror("Échec allocation mémoire pour les listes");
        exit(EXIT_FAILURE);
    }

    // Initialiser chaque sommet avec une liste vide
    for (int i = 0; i < taille; i++) {
        adjList->tab[i] = creerSommet();
    }

    return adjList;
}

/*
 * Fonction : afficherListeAdjacence
 * ---------------------------------
 * Parcourt tous les sommets de la liste d'adjacence et affiche leurs arêtes.
 */
void afficherListeAdjacence(listeAdjacence *adjList) {
    if (adjList == NULL) {
        printf("La liste d’adjacence est vide.\n");
        return;
    }

    for (int i = 0; i < adjList->taille; i++) {
        printf("Liste pour le sommet %d : [head @] -> ", i + 1);
        t_arete * curr = adjList->tab[i]->head;
        while (curr != NULL) {
            printf("(%d, %.2f)", curr->sommet_arrivee, curr->proba);
            if (curr->suiv != NULL)
                printf(" @-> ");
            curr = curr->suiv;
        }
        printf("\n");
    }
}

/*
 * Fonction : readGraph
 * --------------------
 * Lit un fichier texte contenant un graphe et crée la liste d'adjacence correspondante.
 * Format du fichier :
 *   première ligne : nombre de sommets
 *   lignes suivantes : depart arrivee probabilité
 */
listeAdjacence * readGraph(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (file == NULL) {
        perror("Le fichier ne s'est pas ouvert à la lecture.");
        exit(EXIT_FAILURE);
    }

    int nbvert, depart, arrivee;
    float proba;

    // Lecture du nombre de sommets
    if (fscanf(file, "%d", &nbvert) != 1) {
        perror("Le nombre de sommets n'a pas pu être lu. ");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Créer la liste d'adjacence vide
    listeAdjacence *graph = creerListeAdjacence(nbvert);

    // Lecture des arêtes
    while (fscanf(file, "%d %d %f", &depart, &arrivee, &proba) == 3) {
        t_arete *new = creerArete(arrivee, proba, NULL);
        ajouterArete(new, graph->tab[depart - 1]); // -1 car sommets numérotés à partir de 1
    }

    fclose(file);
    return graph;
}

/*
 * Fonction : probaTotale
 * ----------------------
 * Calcule la somme des probabilités des arêtes sortantes d'un sommet.
 */
float probaTotale(t_sommet *sommet) {
    if (sommet == NULL) {
        perror("Le sommet fourni en paramètres est vide. \n");
        exit(EXIT_FAILURE);
    }

    t_arete * curr = sommet->head;
    float probaTotale = 0.0;
    while (curr != NULL) {
        probaTotale += curr->proba;
        curr = curr->suiv;
    }
    return probaTotale;
}

/*
 * Fonction : estMarkov
 * --------------------
 * Vérifie si le graphe est markovien :
 *  pour chaque sommet, la somme des probabilités de ses arêtes sortantes doit être ≈ 1.
 */
int estMarkov(listeAdjacence * adjlist) {
    if (adjlist == NULL) {
        perror("La liste d'adjacence fournie en paramètre est nulle. \n");
        exit(EXIT_FAILURE);
    }

    const float EPSILON = 0.001f;

    for (int i = 0; i < adjlist->taille; i++) {
        float somme = probaTotale(adjlist->tab[i]);
        printf("La somme des probabilites du sommet %d est %.3f. \n", i + 1, somme);
        if (somme < 1.0f - EPSILON || somme > 1.0f + EPSILON) {
            // sommet non markovien
            return 0;
        }
    }

    return 1; // tous les sommets sont markoviens
}

/*
 * Fonction : ecrireGraphe
 * -----------------------
 * Écrit un graphe représenté par une liste d'adjacence dans un fichier texte
 * dans le même format que celui utilisé pour readGraph.
 */
void ecrireGraphe(listeAdjacence *g, const char *filename) {
    if (g == NULL) {
        perror("La liste d'adjacence fournie est nulle.\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(filename, "w"); // ouverture en écriture
    if (file == NULL) {
        perror("Impossible d'ouvrir le fichier pour écriture.\n");
        exit(EXIT_FAILURE);
    }

    // Écrire le nombre de sommets
    fprintf(file, "%d\n", g->taille);

    // Parcourir chaque sommet et écrire ses arêtes
    for (int i = 0; i < g->taille; i++) {
        t_arete *curr = g->tab[i]->head;
        while (curr != NULL) {
            fprintf(file, "%d %d %.2f\n", i + 1, curr->sommet_arrivee, curr->proba);
            curr = curr->suiv;
        }
    }

    fclose(file);
}


/*
 * Fonction : genererMermaid
 * -------------------------
 * Étape 3 du projet : Génère un fichier texte compatible avec Mermaid
 * pour visualiser le graphe.
*/
void genererMermaid(listeAdjacence *g, const char *filename) {
    if (g == NULL) return;

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur ouverture fichier Mermaid");
        exit(EXIT_FAILURE);
    }


    //nos entetes
    fprintf(file, "---\n");
    fprintf(file, "config:\n");
    fprintf(file, "  layout: elk\n");
    fprintf(file, "  theme: neo\n");
    fprintf(file, "  look: neo\n");
    fprintf(file, "---\n");
    fprintf(file, "flowchart LR\n");

    // 2. Déclaration des sommets : A((1))
    // Pas de conflit ici car un seul appel à getID par ligne
    for (int i = 0; i < g->taille; i++) {
        fprintf(file, "%s((%d))\n", getID(i + 1), i + 1);
    }

    // 3. Déclaration des arêtes
    // Astuce : on découpe le fprintf pour éviter le conflit de buffer
    for (int i = 0; i < g->taille; i++) {
        t_arete *curr = g->tab[i]->head;
        while (curr != NULL) {
            // Étape A : On écrit d'abord la source "A"
            fprintf(file, "%s", getID(i + 1));

            // Étape B : On écrit la flèche et la proba "-->|0.50|"
            fprintf(file, "-->|%.2f|", curr->proba);

            // Étape C : On écrit la destination "B" + saut de ligne
            // Ici, getID réécrit dans le buffer, mais c'est pas grave car
            // l'étape A est déjà finie et écrite dans le fichier !
            fprintf(file, "%s\n", getID(curr->sommet_arrivee));

            curr = curr->suiv;
        }
    }

    fclose(file);
    printf("Fichier Mermaid '%s' généré.\n", filename);
}