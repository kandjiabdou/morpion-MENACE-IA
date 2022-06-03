#include "menace_sys.h"

#define CONTINUE 0
#define FINISHED 1

Configuration* newConfiguration(uint16_t grilleNum, uint8_t nbPion){
    Configuration* cfg = malloc(sizeof(Configuration));
    assert(cfg != NULL);
	cfg->numGrille = grilleNum;
    uint16_t val;
    // Initialise le nombre de bille corespondant au case libre
    // Formule nombre de bille = 8-2nombre pions 'X'
    for (int16_t i = 8; i >= 0; i--){
        // Cette méthode décompose un entier en grille 3x3
        if(grilleNum!=0){
            val = grilleNum%3;
            grilleNum/=3;
        }else val = 0;
        cfg->billes[i/3][i%3] = val == 0 ? 8-2*nbPion : 0;
    }
    cfg->indiceTransformation = 0;
    cfg->indiceCaseJouee = 0;
    cfg->suivant = NULL;
    cfg->configurationJouee = NULL;
	return cfg;
}

Configuration* newConfigurationRestored(uint16_t grilleNum, uint32_t billes[3][3]){
    Configuration* cfg = malloc(sizeof(Configuration));
    assert(cfg != NULL);
	cfg->numGrille = grilleNum;
    for (int16_t i = 0; i < 3; i++)
    for (int16_t j = 0; j < 3; j++){
        cfg->billes[i][j] = billes[i][j];
    }
    cfg->indiceTransformation = 0;
    cfg->indiceCaseJouee = 0;
    cfg->suivant = NULL;
    cfg->configurationJouee = NULL;
	return cfg;
}

uint8_t next_configuration(uint8_t grille[3][3], uint16_t* n1, uint16_t* n2){
    // Toute les configurations possibles
    uint8_t i, j;
    (*n1)=0; (*n2)=0;
    grille[0][0]++;
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            if(grille[i][j]==3){
                grille[i][j]=0;
                if(j<2) grille[i][j+1]++;
                else if(i<2) grille[i+1][0]++;
                else return FINISHED;
            }else if (grille[i][j] == 1) (*n1)++;
            else if (grille[i][j] == 2) (*n2)++;
        }
    }

    return CONTINUE;
}
char print_value(uint8_t value){
    switch(value){
        case (0):
            return ' ';
        case (1):
            return 'X';
        case (2):
            return 'O';
        default:
            assert(0);
    }
}
void display_grille_2d(uint8_t grille[3][3]){
    printf("    ____________\n");
    printf("    |1  |2  |3  |\n");
    printf("    | %c | %c | %c |\n", print_value(grille[0][0]), print_value(grille[0][1]), print_value(grille[0][2]));
    printf("    _____________\n");
    printf("    |4  |5  |6  |\n");
    printf("    | %c | %c | %c |\n", print_value(grille[1][0]), print_value(grille[1][1]), print_value(grille[1][2]));
    printf("    _____________\n");
    printf("    |7  |8  |9  |\n");
    printf("    | %c | %c | %c |\n", print_value(grille[2][0]), print_value(grille[2][1]), print_value(grille[2][2]));
    printf("    _____________\n");
}

Menace* newMenace(){
	Menace* m = malloc(sizeof(Menace));
	assert(m != NULL);
    for (uint8_t i = 0; i < 4; i++)
    for (uint8_t j = 0; j < 5; j++)
    for (uint8_t k = 0; k < 5; k++){
        m->boites[0][i][j][k]=NULL;
        m->boites[1][i][j][k]=NULL;
    }
    m->teteListesConfigurationsJouees = NULL;
    m->taille = 0;
	return m;
}

void ajouterConfigurationJouee(Menace * m,  Configuration* c){
    c->configurationJouee = m->teteListesConfigurationsJouees;
    m->teteListesConfigurationsJouees = c;
}

void appliquerLesBonusMalusConfigJouees(Menace * m, int16_t bonusMalus){
    Configuration* cj;
    while (m->teteListesConfigurationsJouees != NULL){
        cj = m->teteListesConfigurationsJouees;
        m->teteListesConfigurationsJouees = cj->configurationJouee;
        if(cj->configurationJouee != NULL) cj->configurationJouee = NULL;
        cj->billes[cj->indiceCaseJouee/3][cj->indiceCaseJouee%3] += bonusMalus;
    }
    
}

uint16_t hachage(uint16_t num){
    // Retourne la somme des chiffres du numéro de la grille modulo 5
    uint16_t somme = 0;
    while( num > 0){
        somme+= num%10;
        num/=10;
    }
    return somme%5;
}

void ajouterConfigurationDansMenace(Menace* m, Configuration* conf, uint16_t difPion, uint16_t nbPionMenace){
    // Ajoute dans menace une configuration dont le nombre de pions 'X'=n
    uint8_t h1 = (conf->numGrille%10)/2;
    uint8_t h2 = hachage(conf->numGrille);
    conf->suivant = m->boites[difPion][nbPionMenace][h1][h2];
    m->boites[difPion][nbPionMenace][h1][h2] = conf;
    m->taille++;
}
void MenacePop(Menace* m, uint16_t difPion, uint16_t nbPionMenace, uint16_t parite, uint16_t sommeChiffre){
    // Supprime une configuration dans menace et libère la mémoire
    Configuration* c = m->boites[difPion][nbPionMenace][parite][sommeChiffre];
    m->boites[difPion][nbPionMenace][parite][sommeChiffre] = c->suivant;
    m->taille--;
    free(c);
}

void freeMenace(Menace* m){
    // Libère la mémoire
    for (uint8_t i = 0; i < 4; i++)
    for (uint8_t j = 0; j < 5; j++)
    for (uint8_t k = 0; k < 5; k++) {
        while (m->boites[0][i][j][k] != NULL) MenacePop(m, 0, i, j, k);
        while (m->boites[1][i][j][k] != NULL) MenacePop(m, 1, i, j, k);
    }
    free(m);
}

void affiche_debut_jeu(){
    printf("###################################################\n");
    printf("############    MENACE GAME - MORPION  ############\n");
    printf("###################################################\n");
}

uint8_t qui_a_gagne(uint8_t grille[3][3], uint16_t nb1, uint16_t nb2){
    // S'il y a un gagnant; retourne le numéro : 1 ou 2
    // Sinon retourne 0 
    if(nb1>= 3 || nb2 >= 3){
        if(grille[0][0]==grille[0][1] && grille[0][0]==grille[0][2] && grille[0][0] !=0) return grille[0][0];
        if(grille[1][0]==grille[1][1] && grille[1][0]==grille[1][2] && grille[1][0] !=0) return grille[1][0];
        if(grille[2][0]==grille[2][1] && grille[2][0]==grille[2][2] && grille[2][0] !=0) return grille[2][0];

        if(grille[0][0]==grille[1][0] && grille[0][0]==grille[2][0] && grille[2][0] !=0) return grille[0][0];
        if(grille[0][1]==grille[1][1] && grille[0][1]==grille[2][1] && grille[2][1] !=0) return grille[0][1];
        if(grille[0][2]==grille[1][2] && grille[0][2]==grille[2][2] && grille[2][2] !=0) return grille[0][2];

        if(grille[0][0]==grille[1][1] && grille[0][0]==grille[2][2] && grille[2][2] !=0) return grille[0][0];

        if(grille[0][2]==grille[1][1] && grille[0][2]==grille[2][0] && grille[2][0] !=0) return grille[0][2];
    }
    return 0;
}

uint8_t prochaine_grille(uint8_t grille[3][3], uint16_t* n1, uint16_t* n2){
    // Toute les configurations possibles
    uint8_t i, j;
    (*n1)=0; (*n2)=0;
    grille[0][0]++;
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            if(grille[i][j]==3){
                grille[i][j]=0;
                if(j<2) grille[i][j+1]++;
                else if(i<2) grille[i+1][0]++;
                else return FINISHED;
            }else if (grille[i][j] == 1) (*n1)++;
            else if (grille[i][j] == 2) (*n2)++;
        }
    }

    return CONTINUE;
}

uint16_t appliquer_transformation_base(uint8_t grille[3][3], uint8_t transformations[9]){
    // Transforme la grille par une symétrie transformations[i]
    // Sans modifier la grille et retourne le numéro de la grille transformée
    uint32_t nb = 0;
    uint16_t it;
    uint16_t jt;
    for (int16_t i = 0; i < 3; i++)
    for (int16_t j = 0; j < 3; j++){
        it = transformations[i*3+j]/3;
        jt = transformations[i*3+j]%3;
        nb += (uint32_t) pow(3, 8-(i*3+j))* grille[it][jt];
    }
    return nb;
}

Configuration * trouverTransformationDansMenace(Menace* m, uint16_t num, uint16_t difPion, u_int16_t nbPionMenace){
    // Cherche une configuration dans menace à l'aide de son numéro de grille
    Configuration* c = m->boites[difPion][nbPionMenace][(num%10)/2][hachage(num)];
    while (c != NULL){
        if(c->numGrille == num) return c;
        c = c->suivant;
	}
    return NULL;
}

Configuration * trouver_8_Transformations_DansMenace(Menace* m, uint8_t grille[3][3], uint8_t transformations[8][9], uint16_t difPion, uint16_t nbPion ){
    uint16_t numConfiguration = appliquer_transformation_base(grille, transformations[0]);
    Configuration * c = trouverTransformationDansMenace(m,numConfiguration, difPion, nbPion);
    uint16_t indiceTransformation = 1;
    while (c==NULL && indiceTransformation<8){
        numConfiguration = appliquer_transformation_base(grille, transformations[indiceTransformation]);
        c = trouverTransformationDansMenace(m, numConfiguration, difPion, nbPion);
        indiceTransformation++;
    }
    c->indiceTransformation = indiceTransformation-1;
    return c;
}

uint16_t ChoisirAleatoirementCaseConfigSauvegardee(Configuration* c){
    uint32_t tailleElement = 0;
    for (int16_t i = 0; i < 3; i++)
    for (int16_t j = 0; j < 3; j++){
        if(c->billes[i][j] !=0) {
            tailleElement += c->billes[i][j];
        }
    }
    if(tailleElement == 0){
        printf("->>> ! MENACE ne peut pas jouer à défaut de bille <----\n");
        return 100;
    }

    uint16_t random_value = rand() % tailleElement;
    uint32_t cumul = 0;
    uint16_t i;
    for (i=0; i < 9 && cumul<= random_value; i++){
        if(c->billes[i/3][i%3] !=0) {
            cumul += c->billes[i/3][i%3];
        }
    }
    c->indiceCaseJouee = i-1;
    return i-1;
}

uint8_t getCaseChoisieDansGrilleJeu(Configuration* c, uint8_t transformations[8][9]){
    return transformations[c->indiceTransformation][c->indiceCaseJouee];
}

uint16_t ChoixMenace(Menace* m, uint8_t grille[3][3], uint8_t transformations[8][9], uint16_t difPion,  uint16_t nbPionMenace){
    Configuration * c = trouver_8_Transformations_DansMenace(m, grille, transformations, difPion, nbPionMenace);
    ajouterConfigurationJouee(m, c);
    uint16_t isChoiceCorrect = ChoisirAleatoirementCaseConfigSauvegardee(c);
    if(isChoiceCorrect == 100) return 100;
    uint16_t caseChoisie = getCaseChoisieDansGrilleJeu(c, transformations);
    if(nbPionMenace == 0 && difPion == 0) 
    caseChoisie = caseChoisie == 4 ? 4 : ( caseChoisie == 1 ? ((rand()%4)*2 + 1) : ( (rand()%2)*2 + (rand()%2)*6 ) ) ;
    return caseChoisie;
}
uint16_t ChoixUtilisateur(uint8_t grille[3][3]){
    printf("\n # Choisi une de ces positions :\n #");
    for (int16_t i = 0; i < 3; i++)
    for (int16_t j = 0; j < 3; j++){
        if(grille[i][j] == 0){
            printf(" - %d",i*3+j +1 );
        }
    }
    printf(".\n");
    int32_t caseChoisie;
    do{
        printf(" # Entrer votre choix : ");
        scanf("%d", &caseChoisie);
    }while ( caseChoisie>9 || caseChoisie<1 || grille[(caseChoisie-1)/3][(caseChoisie-1)%3] != 0 );
    return (uint16_t) caseChoisie-1;
}

uint16_t choixAleatoire(Menace * m, uint16_t nbPion){
    uint32_t tailleElement = 9 - (nbPion*2 + 1);
    uint16_t random_value = rand() % tailleElement;
    uint32_t cumul = 0;
    uint16_t i;
    for (i=0; i < 9 && cumul<= random_value; i++){
        if(m->grilleDeJeu[i/3][i%3] ==0) {
            cumul += 1;
        }
    }
    return i-1;
}
int32_t utilisateurNewGame(){
    printf("################   MENU DU JEU  ###################\n");
    printf(" ## Nouvelle partie\n");
    printf(" ## Tapez 1 : pour jouer contre MENACE\n");
    printf(" ## Tapez 2 : MENACE vs MENACE\n");
    printf(" ## Tapez 3 : Quiter et sauvegarder MENACE\n");
    int32_t utilisateur;
    do{
        printf(" >> Entrer votre choix : ");
        scanf("%d", &utilisateur);
        if(utilisateur != 1 && utilisateur != 2 && utilisateur != 3)
            printf(" # Votre choix n'est pas bon !\n");
    }while (utilisateur != 1 && utilisateur != 2 && utilisateur != 3);
    return utilisateur;
}

int32_t demander_nombre_partie(){
    printf(" ## Menace vs Menace \n");
    printf(" ## Donnez le nombre de partie que menace va jouer \n");
    int32_t nbpartie;
    do{
        printf(" >> Entrer votre choix : ");
        scanf("%d", &nbpartie);
        if(nbpartie <= 0)
            printf(" # Votre choix n'est pas bon !\n");
    }while (nbpartie <= 0);
    return nbpartie;
}

void initMenace(Menace * m, uint8_t transformations[8][9] ){
    uint8_t grille[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    uint16_t n1,n2;
    uint16_t num;
    ajouterConfigurationDansMenace(m, newConfiguration(0,0), 0, 0);
    while(next_configuration(grille, &n1, &n2)==CONTINUE){
        if((n2 - n1 == 1 || n2 - n1 == 0) && n1 <=3 ){
            if(qui_a_gagne(grille, n1, n2) == 0 ){
                num = appliquer_transformation_base(grille, transformations[0]);
                uint16_t transfo = 1;
                while (trouverTransformationDansMenace(m,num, n2 - n1, n1) == NULL && transfo<8){
                    num = appliquer_transformation_base(grille, transformations[transfo]);
                    transfo++;
                }
                if(transfo==8) ajouterConfigurationDansMenace(m, newConfiguration(num,n1), n2 - n1, n1);
            }  
        }
    }
    Configuration * c = trouverTransformationDansMenace(m, 0, 0, 0);
    for (int16_t i = 0; i < 9; i++)
        if(i != 0 && i != 1 && i != 4)
            c->billes[i/3][i%3] = 0;
}
void saveTete(Configuration * c, uint8_t dif, uint8_t nb, FILE * save_file){
	while (c != NULL){
        fprintf(save_file, "%d,%d,%d:", dif, nb, c->numGrille);
        for (int16_t i = 0; i <8 ; i++){
            fprintf(save_file, "%d,", c->billes[i/3][i%3]);
        }
        fprintf(save_file, "%d\n", c->billes[2][2]);
		c = c->suivant;
	}
}
void sauvegarderMenace(Menace * m){
    FILE * save = fopen("sauvegarde.txt","wb");
    for (uint8_t nb = 0; nb < 4; nb++)
    for (uint8_t j = 0; j < 5; j++)
    for (uint8_t k = 0; k < 5; k++){
        saveTete(m->boites[0][nb][j][k], 0, nb, save);
        saveTete(m->boites[1][nb][j][k], 1, nb, save);
    }
    fclose(save);
}
void restaurerMenace(Menace * m, uint8_t transformations[8][9] ){
    FILE * restore = fopen("sauvegarde.txt","rb");
	if( restore == NULL ){
        printf("Il n'exte pas de fichier de sauvegarde\n");
        printf("Initialisatoin de Menace\n");
        initMenace(m, transformations);
    }else{
        printf(" ## Il exte un fichier de sauvegarde\n");
        printf(" ## Voulez vous continuer avec le dernier sauvegarde ?\n");
        printf(" ## Tapez 1 : OUI\n");
        printf(" ## Tapez 2 : NON (par défaut ) \n");
        printf(" >> Entrer votre choix : ");
        int16_t reponse;
        scanf("%hd", &reponse);
        if(reponse == 1){
            printf("Restauration des données ...\n");
            uint16_t num;
            uint8_t diff, nbPionMenace;
            uint32_t billes[3][3];
            //scan the pionter : configuration
            while(fscanf(restore, "%hhd,%hhd,%hd:", &diff, &nbPionMenace, &num) == 3){
                fscanf(restore, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n", &billes[0][0], &billes[0][1], &billes[0][2], &billes[1][0], &billes[1][1], &billes[1][2], &billes[2][0], &billes[2][1], &billes[2][2]);
                Configuration * c = newConfigurationRestored(num, billes);
                ajouterConfigurationDansMenace(m, c,diff, nbPionMenace);
            }
            printf("FIN Restauration des données !\n"); 
        }else{
            printf("RéInitialisatoin de Menace\n");
            initMenace(m, transformations);
            remove("sauvegarde.txt");
        }
        fclose(restore); 
    }
    
}
uint16_t jouerUnePartie(Menace * m, uint8_t transformations[8][9], uint32_t utilisateur, uint16_t quiCommence){    
    uint16_t nbPion1, nbPion2, quiAGagne;
    for (int16_t i = 0; i < 3; i++) for (int16_t j = 0; j < 3; j++) m->grilleDeJeu[i][j] = 0;
    nbPion1 = 0; nbPion2 = 0; quiAGagne = 0;
    uint16_t quiJoue = quiCommence;
    uint16_t caseChoisie;
    if(utilisateur == 1) display_grille_2d(m->grilleDeJeu);
    while (quiAGagne == 0 && (nbPion1 + nbPion2)<8 ){
        if(quiJoue == 1){// 1 : menace
            caseChoisie = ChoixMenace(m, m->grilleDeJeu, transformations, nbPion2 - nbPion1, nbPion1);
            if(caseChoisie == 100){
                freeMenace(m);
                exit(EXIT_FAILURE);
            };
            nbPion1++;
        }else{// 2 : user ou Randome
            
            caseChoisie = utilisateur==1 ? ChoixUtilisateur(m->grilleDeJeu) : choixAleatoire(m, nbPion2);
            nbPion2++;
        }
        m->grilleDeJeu[caseChoisie/3][caseChoisie%3] = quiJoue;
        quiJoue = quiJoue%2 + 1; // 1 ou 2
        quiAGagne = qui_a_gagne(m->grilleDeJeu, nbPion1, nbPion2);
        if(utilisateur == 1) {
            quiJoue==2? printf("------- C'est votre tour ---------\n") : printf("------- Ma a joué la case %d ---------\n", caseChoisie);
            display_grille_2d(m->grilleDeJeu);
        }
    }

    if(quiAGagne == 0){
        uint16_t i = 0;
        while (m->grilleDeJeu[i/3][i%3] != 0){
            i++;
        }
        m->grilleDeJeu[i/3][i%3] = quiJoue;
        quiAGagne = qui_a_gagne(m->grilleDeJeu, nbPion1, nbPion2);
        if(utilisateur == 1) {
            printf(" La case restante %d est jouée : %d\n", i, quiJoue);
            display_grille_2d(m->grilleDeJeu);
        }
    }
    if(utilisateur == 1){
        if(quiAGagne == 0) printf("\n ----------------------- Partie EGALE !\n\n");
        else if(quiAGagne == 1) printf("\n -------------------- MENACE a gagné !\n\n");
        else printf("\n ------------------------ Bravo vous avez gagné !\n\n");
    }

    int16_t bonusMalus = quiAGagne == 1 ? 3 : (quiAGagne == 2 ? -1 : 1);
    appliquerLesBonusMalusConfigJouees(m, bonusMalus);
    return quiAGagne;
}