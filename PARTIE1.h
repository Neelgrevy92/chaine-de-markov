#ifndef PARTIE1_H
#define PARTIE1_H

#include <stdio.h>
#include <stdlib.h>

// Structure Arete
typedef struct s_arete{
    int sommet_arrivee;
    float proba;           // Harmonisé en 'float' pour correspondre au .c
    struct s_arete * suiv;
} t_arete;

// Structure Sommet
typedef struct s_sommet{
    t_arete * head;
} t_sommet;

// Structure Liste d'Adjacence (Graphe)
typedef struct ListeAdjacence {
    t_sommet ** tab;
    int taille;
} listeAdjacence;

// Prototypes
t_arete * creerArete(int sommet_arrivee, float proba, t_arete * suiv);
t_sommet * creerSommet();
void ajouterArete(t_arete * arete, t_sommet * sommet);

// Correction de la typo: "afficher" au lieu de "affihcer"
void afficherSommet(t_sommet * sommet); 

listeAdjacence * creerListeAdjacence(int taille);
void afficherListeAdjacence(listeAdjacence *adjList);
listeAdjacence * readGraph(const char *filename);

// Ajout des prototypes manquants
float probaTotale(t_sommet *sommet); 
int estMarkov(listeAdjacence * adjlist);
void ecrireGraphe(listeAdjacence *g, const char *filename);


void genererMermaid(listeAdjacence *g, const char *filename);

#endif //PARTIE1_H