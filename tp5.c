#include <stdio.h>
#include <string.h>

char non_terminaux[] = "EeTtFfP"; 
char terminaux[] = "+*()ab$"; 

//emplacement d un caractere dans une chaine
int emplacement(char c, char *s){
    int i;
    for (i = 0; s[i] != '\0'; i++)
        if (s[i] == c) return i;
    return -1;
}

/* Premier et Suivant de chaque non-terminal (meme ordre que "EeTtFfP") */
char *premier[] = { "(ab", "+",   "(ab",  "(ab",  "(ab",    "*",      "(ab" };
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
                printf("ERREUR : caractere '%c' inconnu\n", a); 
                return 0; 
            }

            r = M[ligne][colonne];

            if (r == 0) {                       /* MODE PANIQUE */
                nb_erreurs++;
                printf("ERREUR : case vide -> on saute des caracteres\n");
                /* on elimine les lettres jusqu'a en trouver une de Premier(X) ou de Suivant(X) */
                while (a != '$' && emplacement(a, premier[ligne]) == -1
                                && emplacement(a, suivant[ligne]) == -1) {
                    ptr++;
                    a = mot[ptr];
                }
                if (emplacement(a, premier[ligne]) == -1)
                    depiler();                  /* a est dans Suivant(X) : on abandonne X */
                continue;                       /* sinon on garde X et on recommence */
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
            printf("ERREUR : caracteres en trop a la fin\n");
            return 0;
        }

        else{
            if (X == a){
                printf("\n");
                depiler();
                ptr++;
            }
            else{                               /* MODE PANIQUE : terminal attendu absent */
                nb_erreurs++;
                printf("ERREUR : on attendait '%c' -> on le considere comme ajoute\n", X);
                depiler();
            }
        }

    }
}

int main(void){
    char ligne[200], mot[200];
    int i, n;

    printf("Ecrire une expression reguliere sur {a,b} :\n");

    while(fgets(ligne, sizeof ligne, stdin) != NULL){
        n = 0;
        for (i = 0; ligne[i] != '\0'; i++)
            if (ligne[i] != ' ' && ligne[i] != '\t' && ligne[i] != '\n' && ligne[i] != '\r')
                mot[n++] = ligne[i];
        if (n == 0) continue;
        mot[n] = '\0';
        printf("\nAnalyse de : %s\n", mot);       /* on affiche le mot avant d'ajouter le $ */
        mot[n] = '$';                               /* le mot est termine par $ */
        mot[n + 1] = '\0';

        if (analyse(mot)) printf("=> le mot est accepte\n\n");
        else printf("=> le mot est refuse\n\n");
    }
    return 0;
}