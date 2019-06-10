#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"


char ** decompo (FILE* fichier,int *MaxL,int pos)
{
        int c;
        int line = 0;

        /*Variables declared as static inside a function are statically allocated,
         thus keep their memory cell throughout all program execution, while having the
         same scope of visibility as automatic local variables (auto and register),
         meaning remain local to the function. Hence whatever values the function puts
         into its static local variables during one call will still be present when the
         function is called again.*/
        static char* tab[5][MAX_LINE]; //static = super important

        int nbMot=0;
        int returnLine = 0;

        /*Tout d'abord on initialise le tableau à la bonne position avec du vide
          pour être sur de ne pas être embeter */
        for(int init =0; init < MAX_LINE; init++)
        {
            tab[pos][init] = "";
        }

        while ((c = getc(fichier)) != EOF)
        {
            //Tableau ASCII : http://www.gecif.net/qcm/information/ascii_decimal.pdf

            if (c==46 || c==33 || c==58 || c==59 || c==63 || (nbMot>8 && c==44) || (nbMot>10 && c==32))
            {
                tab[pos][line] = append_(tab[pos][line],c);
                line++;
                returnLine = 1;
                nbMot=0;
            }
            else if (c==32)//Si c'est un espace
            {

                if(!returnLine)
                {
                    tab[pos][line] = append_(tab[pos][line],c);
                }
                else
                {
                    returnLine = 0;
                }
                nbMot++;
            }
            else if ((c!=32 || nbMot!=0) && c!=10)
            {
                tab[pos][line] = append_(tab[pos][line],c);
                returnLine = 0;

            }

        }

        //Pour debug et afficher le découpage
        /*printf("\n-------FICHIER-DEBUT-------\n");

        for(int j = 0; j <= line; j++)
        {
                printf("\n%d: %s",j,tab[pos][j]);
        }

        printf("\n-------FICHIER-FIN-------");*/


        *MaxL = line;

        //On retourne un tableau de char*
        return tab[pos];

}

/*Cette fonction est appellé une fois que l'utilisateur à validé la selection*/
/*Le premier argument *path est le chemin d'acces du fichier*/
/*MaxL est l'adresse de la variable MaxL qui servira a receuillir le nombre de lignes*/
char ** phase_1(char *path,int *MaxL,int pos)
{
    FILE* fichier = NULL;
    fichier = fopen(path, "r");
    char ** buffer[5] ; //On a ici 5 tableau a deux dimension
    if(fichier == NULL)
    {
        system("CLS");
        printf("No file in the directory");
        exit(EXIT_FAILURE);
        return;
    }


    if (fichier)
    {
        buffer[pos] =decompo(fichier,MaxL,pos);
    }
    fclose(fichier);


    return buffer[pos];

}



