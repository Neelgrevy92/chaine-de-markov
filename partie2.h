//
// Created by neelg on 20/11/2025.
//

#ifndef PARTIE2_H
#define PARTIE2_H

#include "partie1.h"

typedef struct s_tarjan_vertex {
    int id;
    int num;
    int num_accessible;
    int dans_pile;
} t_tarjan_vertex;

typedef struct s_cellule_vertex {
    t_tarjan_vertex *sommet;
    struct s_cellule_vertex *suiv;
} t_cellule_vertex;


// La structure Classe e
typedef struct s_classe {
    char nom[10];           // Nom : "C1", "C2"
    t_cellule_vertex *head; // Liste des sommets
    struct s_classe *suiv;  // Pointeur vers le C suivant
} t_classe;


typedef struct s_partition {
    t_classe *head;  // Liste des classes
    int nb_classes;  // Compteur
} t_partition;

typedef struct s_element_pile {
    int id_sommet;
    struct s_element_pile *suiv;
} t_element_pile;

typedef struct s_pile {
    t_element_pile *head;
} t_pile;


// --- PROTOTYPES ---

// Fonctions utilitaires Pile
t_pile *creerPile();
void empiler(t_pile *p, int id);
int depiler(t_pile *p);
int estDansPile(t_pile *p, int id);

// Fonctions utilitaires Tarjan
t_tarjan_vertex *creerTabTarjan(listeAdjacence *g);
t_partition *creerPartition();
t_classe *ajouterClasse(t_partition *p, char *nom);
// Note : on passe un pointeur vers le sommet pour respecter la logique "stocke des t_tarjan_vertex"
void ajouterSommetClasse(t_classe *c, t_tarjan_vertex *sommet);

// Algorithme principal
t_partition *algoTarjan(listeAdjacence *g);

#endif //PARTIE2_H
