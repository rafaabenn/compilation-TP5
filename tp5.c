#include <stdio.h>
#include <string.h>

char non_terminaux[] = "EeTtFfP"; 
char terminaux[] = "+*()ab$"; 

int emplacement(char c, char *s){
    int i;
    for (i = 0; s[i] != '\0'; i++)
        if (s[i] == c) return i;
    return -1;
}

char *suivant[] = { ")$",  ")$",  "+)$",  "+)$",  "(ab+)$", "(ab+)$", "*(ab+)$" };
int nb_erreurs;

int est_non_terminal(char x){
    return emplacement(x, non_terminaux) != -1;
}

char *partie_droite[] = { "", "Te", "+Te", "", "Ft", "Ft", "", "Pf", "*f", "", "(E)", "a", "b" };

char *regle[] = { "",
    "E -> T E'", "E' -> + T E'", "E' -> eps",
    "T -> F T'", "T' -> F T'",   "T' -> eps",
    "F -> P F'", "F' -> * F'",   "F' -> eps",
    "P -> ( E )", "P -> a",      "P -> b" };

// 0 => case vide
int M[7][7] = {
/*            +   *   (   )   a   b   $   */
/* E  */   {  0,  0,  1,  0,  1,  1,  0 },
/* E' */   {  2,  0,  0,  3,  0,  0,  3 },
/* T  */   {  0,  0,  4,  0,  4,  4,  0 },
/* T' */   {  6,  0,  5,  6,  5,  5,  6 },
/* F  */   {  0,  0,  7,  0,  7,  7,  0 },
/* F' */   {  9,  8,  9,  9,  9,  9,  9 },
/* P  */   {  0,  0, 10,  0, 11, 12,  0 }
};

//la pile
char pile[200];
int sommet = -1;

void empiler(char c) { 
    sommet++; 
    pile[sommet] = c;
}

void depiler(void){
    sommet--;
}

///
void afficher_pile(void){
    int i;
    for (i = 0; i <= sommet; i++){
        if      (pile[i] == 'e') printf("E' ");
        else if (pile[i] == 't') printf("T' ");
        else if (pile[i] == 'f') printf("F' ");
        else                     printf("%c ", pile[i]);
    }
}


//algo d analyse
int analyse(char *mot){
    int ptr = 0;
    int r, i, ligne, colonne;
    char X, a;

    nb_erreurs = 0;
    sommet = -1;
    empiler('$'); //initialiser
    empiler('E');

    printf("PILE | ENTREE | SORTIE\n");
    
    while(1){
        X = pile[sommet];
        a = mot[ptr];

        afficher_pile();
        printf("| %s | ", mot + ptr);

        if (est_non_terminal(X)){
            ligne = emplacement(X, non_terminaux);
            colonne = emplacement(a, terminaux);

            if (colonne == -1) { 
                printf("ERREUR : caractere inconnu\n"); 
                return 0; 
            }

            r = M[ligne][colonne];

            if (r == 0) {                      
                nb_erreurs++;
                printf("ERREUR\n");
                while (a != '$' && emplacement(a, suivant[ligne]) == -1) {
                    ptr++;                      
                    a = mot[ptr];
                }
                depiler();                      
                continue;
            }

            printf("%s\n", regle[r]);
            depiler();
            
            for (i = strlen(partie_droite[r]) - 1; i >= 0; i--)
                empiler(partie_droite[r][i]);
        }

        else if (X =='$'){
            if (a == '$'){
                if (nb_erreurs == 0) { printf("ACCEPTER\n"); return 1; }
                printf("FIN : %d erreur(s)\n", nb_erreurs);
                return 0;
            }
            printf("ERREUR\n");
            return 0;
        }

        else{
            if (X == a){
                printf("\n");
                depiler();
                ptr++;
            }
            else{                               
                nb_erreurs++;
                printf("ERREUR\n");
                depiler();
            }
        }

    }
}

int main(int argc, char *argv[]){
    char ligne[200], mot[200];
    int i, n;
    char *nom_fichier = (argc > 1) ? argv[1] : "test.txt";
    FILE *f = fopen(nom_fichier, "r");

    if (f == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", nom_fichier);
        return 1;
    }
    printf("Analyse des exemples du fichier %s :\n", nom_fichier);

    while(fgets(ligne, sizeof ligne, f) != NULL){
        n = 0;
        for (i = 0; ligne[i] != '\0'; i++)
            if (ligne[i] != ' ' && ligne[i] != '\t' && ligne[i] != '\n' && ligne[i] != '\r')
                mot[n++] = ligne[i];
        if (n == 0) continue;
        mot[n] = '\0';
        printf("\nAnalyse de : %s\n", mot);       
        mot[n] = '$';                              
        mot[n + 1] = '\0';

        if (analyse(mot)) printf("=> le mot est accepte\n\n");
        else printf("=> le mot est refuse\n\n");
    }
    fclose(f);
    return 0;
}