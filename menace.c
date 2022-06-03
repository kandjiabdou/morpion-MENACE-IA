#include <stdlib.h>
#include <stdio.h>
#include "menace_sys.h"

int main(void){
    srand(time(NULL));
    uint8_t transformations[8][9]={{
        0,1,2,
        3,4,5,
        6,7,8}, // ID
        {
        2,5,8,
        1,4,7,
        0,3,6}, // ROT_90
        {
        8,7,6,
        5,4,3,
        2,1,0}, // ROT_180
        {
        6,3,0,
        7,4,1,
        8,5,2}, // ROT_270
        {
        2,1,0,
        5,4,3,
        8,7,6}, // MIROIR_VERT
        {
        6,7,8,
        3,4,5,
        0,1,2}, // MIROIR_HORIZ
        {
        0,3,6,
        1,4,7,
        2,5,8}, // MIROIR_VERT + ROT_90
        {
        8,5,2,
        7,4,1,
        6,3,0} // MIROIR_HORIZ + ROT_90
    };
    uint16_t stats[4] = {0};
    //stats[0]: nombre de parties égalisées
    //stats[1]: nombre de parties gagnées
    //stats[2]: nombre de parties perdues
    //stats[3]: nombre de parties jouées
    affiche_debut_jeu();
    Menace* m = newMenace();
    restaurerMenace(m, transformations);
    uint16_t resultat;
    int32_t user = utilisateurNewGame();
    while (user != 3){
        uint16_t quiCommence = rand()% 2 + 1 ;
        if(user == 1){// User vs Menace
            printf(" ## Vous jouez 'O' et Menace joue 'X' \n");
            quiCommence == 1 ? printf(" ## MENACE commence la partie \n") : printf(" ## Vous commencez la partie \n");
            resultat = jouerUnePartie(m, transformations, user, quiCommence);
            stats[3]++; stats[resultat]++;
        }else{// Menace vs Menace
            uint32_t nbPartie = demander_nombre_partie();
            stats[3] += nbPartie; 
            printf("\n-- Menace est entrain de jouer ...\n");
            for(uint32_t nbPartieJouee = 0; nbPartieJouee< nbPartie; nbPartieJouee++){
                resultat = jouerUnePartie(m, transformations, user, 1);
                stats[resultat]++;
            }
            printf("\n\n ## Apres %d partie, Manace a : \n",stats[3]);
            printf("-- Gagné : %d, soit %d%%\n",stats[1], (stats[1]*100/stats[3]));
            printf("-- Perdu : %d\n",stats[2]);
            printf("-- Egalisé : %d\n",stats[0]);
            printf("-- Soit un bilan positif de %d%%\n\n\n\n", ((stats[1]+stats[0])*100/stats[3]));
        }
        user = utilisateurNewGame();
    }

    sauvegarderMenace(m);

    freeMenace(m);
    printf(" ################################### \n");
    printf(" ##   Vous avez quitté le jeu     ## \n");
    printf(" ## MENACE a été bien sauvegardé  ## \n");
    printf(" ################################### \n");
    return EXIT_SUCCESS;
}
// gcc -g menace-sys.c menace.c -o menace -Wall -lm
// valgrind --vgdb=yes --tool=memcheck --leak-check=full --show-leak-kinds=all --show-reachable=yes ./menace