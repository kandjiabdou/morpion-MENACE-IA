#ifndef __MENACE_SYS_H__
#define __MENACE_SYS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <assert.h>

typedef struct Configuration{
    uint16_t numGrille;
    uint32_t billes[3][3];
    struct Configuration* suivant; // La configuration suivante stockée dans menace
    struct Configuration* configurationJouee; //Pile des configuration utilisées en cour de jeu
    uint16_t indiceTransformation; // Indexe de la rotation appliquée à une Configuration cherchée
    uint16_t indiceCaseJouee; // La case d'une configuration jouée en cour de jeu
}Configuration;

typedef struct Menace{
    // Système manace avec les boites d'alumettes
    // 593 Boite d'alumette conservées, en tenant compte des symétries
    // Haché en 3 critères :
    //  - qui a comencé la partie.
    //  - Le nombre pion de 'X' dans la grille: qui vaut 0, 1, 2 ou 3; 0 étant la premiere Configuration 
    //  - Nombre impaire ou paire : 5 paires, 5 impaires
    //  - La somme des chiffres du numéro de la grille modulo 5
    Configuration* boites[2][4][5][5];
    Configuration* teteListesConfigurationsJouees;
    uint8_t grilleDeJeu[3][3];
    uint16_t taille; // Nombre de configuration conservée
}Menace;

Menace* newMenace();
Configuration* newConfiguration(uint16_t grilleNum, uint8_t nbPionMenace);
uint8_t prochaine_grille(uint8_t grille[3][3], uint16_t* n1, uint16_t* n2);
void ajouterConfigurationJouee(Menace * m,  Configuration* c);
void appliquerLesBonusMalusConfigJouees(Menace * m, int16_t bonusMalus);
void ajouterConfigurationDansMenace(Menace* m, Configuration* conf, uint16_t difPion, uint16_t nbPionMenace);
uint16_t appliquer_transformation_base(uint8_t grille[3][3], uint8_t transformations[9]);
Configuration * trouverTransformationDansMenace(Menace* m, uint16_t num, uint16_t difPion, u_int16_t nbPionMenace);
Configuration * trouver_8_Transformations_DansMenace(Menace* m, uint8_t grille[3][3], uint8_t transformations[8][9], uint16_t difPion, uint16_t nbPionMenace );
void MenacePop(Menace* m, uint16_t nbPionMenace, uint16_t nbPion, uint16_t parite, uint16_t sommeChiffre);
void freeMenace(Menace* m);
uint16_t ChoisirAleatoirementCaseConfigSauvegardee(Configuration* c);
uint8_t getCaseChoisieDansGrilleJeu(Configuration* c, uint8_t transformations[8][9]);
uint8_t qui_a_gagne(uint8_t grille[3][3], uint16_t nb1, uint16_t nb2);
void initMenace(Menace * m, uint8_t transformations[8][9]);
void sauvegarderMenace(Menace * m);
void saveTete(Configuration * c, uint8_t dif, uint8_t nbPion, FILE * save_file);

void restaurerMenace(Menace * m, uint8_t transformations[8][9]);

uint8_t next_configuration(uint8_t grille[3][3], uint16_t* n1, uint16_t* n2);
uint16_t jouerUnePartie(Menace * m, uint8_t transformations[8][9], uint32_t utilisateur, uint16_t quiCommence);

uint16_t ChoixMenace(Menace* m, uint8_t grille[3][3], uint8_t transformations[8][9], uint16_t difPion, uint16_t nbPion1);
uint16_t ChoixUtilisateur(uint8_t grille[3][3]);
uint16_t choixAleatoire(Menace * m, uint16_t nbPion);
int32_t demander_nombre_partie();

int32_t utilisateurNewGame();

uint16_t hachage(uint16_t num);

char print_value(uint8_t value);
void display_grille_2d(uint8_t grille[3][3]);
void affiche_debut_jeu();

#endif