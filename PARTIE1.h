//
// Created by nkosi on 07/11/2025.
//

#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdio.h>
#include <stdlib.h>

typedef struct s_arete{
    int sommet_arrivee;
    double proba;
    struct s_arete * suiv;
}t_arete;

typedef struct s_sommet{
    t_arete * head;
}t_sommet;

typedef struct ListeAdjacence {
    t_sommet ** tab;
    int taille;
}listeAdjacence;

t_arete * creerArete(int sommet_arrivee, float proba, t_arete * suiv);
t_sommet * creerSommet();
void ajouterArete(t_arete * arete, t_sommet * sommet);
void afficherSommet(t_sommet * sommet);
listeAdjacence * creerListeAdjacence(int taille);
void afficherListeAdjacence(listeAdjacence *adjList);
listeAdjacence * readGraph(const char *filename);
int probaCorrecte(t_sommet *sommet);
int estMarkov(listeAdjacence * adjlist);
static char *getID(int i);

void ecrireGraphe(listeAdjacence *g, const char *filename);

#endif //STRUCTS_H
