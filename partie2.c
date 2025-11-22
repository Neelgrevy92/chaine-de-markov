#include "partie2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==============================================================================
 * GESTION DE LA PILE
 * ============================================================================== */

/*
 * Crée une pile vide.
 */
t_pile *creerPile() {
    t_pile *p = malloc(sizeof(t_pile));
    if (p == NULL) {
        perror("Erreur allocation pile");
        exit(EXIT_FAILURE);
    }
    p->head = NULL;
    return p;
}

/*
 * Ajoute un identifiant de sommet au sommet de la pile.
 */
void empiler(t_pile *p, int id) {
    t_element_pile *newEl = malloc(sizeof(t_element_pile));
    if (newEl == NULL) {
        perror("Erreur allocation element pile");
        exit(EXIT_FAILURE);
    }
    newEl->id_sommet = id;
    newEl->suiv = p->head;
    p->head = newEl;
}

/*
 * Retire l'élément au sommet de la pile et retourne son identifiant.
 * Retourne -1 si la pile est vide.
 */
int depiler(t_pile *p) {
    if (p->head == NULL) return -1;
    
    t_element_pile *tmp = p->head;
    int val = tmp->id_sommet;
    
    p->head = tmp->suiv;
    free(tmp);
    
    return val;
}

/*
 * Vérifie si un identifiant est présent dans la pile.,
 * mais cette fonction reste utile pour le debug ou la conformité structurelle).
 */
int estDansPile(t_pile *p, int id) {
    t_element_pile *curr = p->head;
    while (curr != NULL) {
        if (curr->id_sommet == id) return 1;
        curr = curr->suiv;
    }
    return 0;
}


/* ==============================================================================
 * GESTION DES STRUCTURES TARJAN
 * ============================================================================== */

/*
 * Initialise le tableau de structures t_tarjan_vertex.
 * Etape 2.1 du sujet [cite: 100-103].
 */
t_tarjan_vertex *creerTabTarjan(listeAdjacence *g) {
    t_tarjan_vertex *tab = malloc(sizeof(t_tarjan_vertex) * g->taille);
    if (tab == NULL) {
        perror("Erreur allocation tableau Tarjan");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < g->taille; i++) {
        tab[i].id = i + 1;        // IDs commencent à 1
        tab[i].num = -1;          // -1 signifie "non visité"
        tab[i].num_accessible = -1;
        tab[i].dans_pile = 0;     // Faux
    }
    return tab;
}

/*
 * Crée une partition vide.
 */
t_partition *creerPartition() {
    t_partition *p = malloc(sizeof(t_partition));
    if (p == NULL) exit(EXIT_FAILURE);
    p->head = NULL;
    p->nb_classes = 0;
    return p;
}

/*
 * Ajoute une nouvelle classe (vide) à la partition.
 */
t_classe *ajouterClasse(t_partition *p, char *nom) {
    t_classe *nouv = malloc(sizeof(t_classe));
    if (nouv == NULL) exit(EXIT_FAILURE);
    
    strcpy(nouv->nom, nom);
    nouv->head = NULL;
    
    // Insertion en tête de liste des classes
    nouv->suiv = p->head;
    p->head = nouv;
    
    p->nb_classes++;
    return nouv;
}

/*
 * Ajoute un sommet existant à une classe.
 */
void ajouterSommetClasse(t_classe *c, t_tarjan_vertex *sommet) {
    t_cellule_vertex *n = malloc(sizeof(t_cellule_vertex));
    if (n == NULL) exit(EXIT_FAILURE);
    
    n->sommet = sommet;
    n->suiv = c->head;
    c->head = n;
}

/*
 * Affiche la partition finale selon le format demandé page 5 [cite: 144-156].
 */
void afficherPartition(t_partition *p) {
    printf("\n--- RÉSULTAT TARJAN (PARTITION) ---\n");
    t_classe *currClass = p->head;
    
    while (currClass != NULL) {
        printf("Composante %s :\n{", currClass->nom);
        
        t_cellule_vertex *currNode = currClass->head;
        while (currNode != NULL) {
            // On affiche l'ID stocké dans le sommet pointé
            printf("%d", currNode->sommet->id);
            
            if (currNode->suiv != NULL) printf(", ");
            currNode = currNode->suiv;
        }
        printf("}\n");
        
        currClass = currClass->suiv;
    }
}


/* ==============================================================================
 * ALGORITHME DE TARJAN
 * ============================================================================== */

/*
 * Fonction récursive interne.
 * Etape 3.1.
 * * u : indice du sommet courant dans le tableau (0 à taille-1)
 * count : pointeur vers le compteur global de visite
 */
static void parcours(int u, listeAdjacence *g, t_tarjan_vertex *tab, t_pile *pile, t_partition *partition, int *count) {
    
    // 1. Initialisation du sommet courant (numérotation)
    tab[u].num = *count;
    tab[u].num_accessible = *count;
    (*count)++;
    
    empiler(pile, u);
    tab[u].dans_pile = 1;

    // 2. Exploration des voisins
    t_arete *voisin = g->tab[u]->head;
    while (voisin != NULL) {
        int v_idx = voisin->sommet_arrivee - 1; // Conversion ID / Index

        if (tab[v_idx].num == -1) {
            // Voisin non visité : appel récursif
            parcours(v_idx, g, tab, pile, partition, count);
            
            // Au retour, mise à jour du num_accessible
            if (tab[v_idx].num_accessible < tab[u].num_accessible) {
                tab[u].num_accessible = tab[v_idx].num_accessible;
            }
        } 
        else if (tab[v_idx].dans_pile) {
            // Voisin déjà visité et dans la pile (arc retour)
            if (tab[v_idx].num < tab[u].num_accessible) {
                tab[u].num_accessible = tab[v_idx].num;
            }
        }
        
        voisin = voisin->suiv;
    }

    // 3. Identification CFC
    // Si num == num_accessible, u est la racine d'une CFC
    if (tab[u].num == tab[u].num_accessible) {
        
        // Création d'une nouvelle classe
        char nomClasse[10];
        sprintf(nomClasse, "C%d", partition->nb_classes + 1);
        t_classe *nouvelleClasse = ajouterClasse(partition, nomClasse);
        
        int v_idx;
        do {
            v_idx = depiler(pile);
            tab[v_idx].dans_pile = 0;
            
            // Ajout du sommet dépilé à la classe courante
            // On passe l'adresse du sommet dans le tableau principal
            ajouterSommetClasse(nouvelleClasse, &tab[v_idx]);
            
        } while (u != v_idx); // On continue tant qu'on n'a pas dépilé la racine u
    }
}

/*
 * Fonction principale qui lance l'algorithme sur tout le graphe.
 */
t_partition *algoTarjan(listeAdjacence *g) {
    if (g == NULL) return NULL;

    // A. Initialisations
    int count = 0;
    t_tarjan_vertex *tab = creerTabTarjan(g);
    t_pile *pile = creerPile();
    t_partition *partition = creerPartition();

    // B. Boucle principale
    for (int i = 0; i < g->taille; i++) {
        if (tab[i].num == -1) {
            parcours(i, g, tab, pile, partition, &count);
        }
    }

    // C. Affichage
    afficherPartition(partition);

    // D. Nettoyage mémoire
    // on ne libere pas tab car partition contient des pointeurs vers tab

    while(!estDansPile(pile, -99) && pile->head != NULL) depiler(pile);
    free(pile);
    return partition;
}