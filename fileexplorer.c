#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

/*Fichiers d'en-tête pour les fonctions relatives aux fichiers.*/
#include <dirent.h>
#include <sys/stat.h>

/*Fichiers d'en-tête pour les fonctions relatives aux Threads.*/
#include <unistd.h>
#include <pthread.h>

/*Fichiers d'en-tête pour les fonctions relatives à la console.*/
#include <windows.h>

/* Fichiers d'en-tête pour les appelles de fonctions */
#include "def.h"
#include "fileexplorer.h"
#include "phase_2.h"

/**Variables globales**/

/*La variables runExplorer est utilisé pour la boucle principale*/
bool runExplorer = true;

/*La console est divisé en deux parties (Gauche / Droite )
l'utilisateur à l'aide des flèche peut passer de gauche à droite
lorsque l'explorateur de fichier est selectionné explorerSelect = true sinon false*/
bool explorerSelect = true;

/*A l'aide des flèches haut/bas l'utilisateur peut naviguer entres les fichiers et les dossier.
selectLine correspond à la ligne selectionné, l'indice est utilisé pour savoir
quel élement l'utilisateur à choisie lorsqu'il clique sur "ENTER" mais également, quels élements
doit être surligné */
int selectLine = 0;

/*maxLine est une variable utilisé pour indiquer, le nombre d'élement afficher sur la console.*/
int maxLine = 0;

/*Si le nombre de fichiers/dossiers dans le chemin d'accès spécifié dépasse le nombre de ligne de la console
l'utilisateur peut descendre avec les flèches pour "scroller" et afficher les autres fichiers,
scroll corresponds au décalage. */
int scroll = 0;

/*La variables pressedEnter est modifié lorsque l'utilisateur appuis sur Enter*/
bool pressedEnter = false;

/*la chaine de caractère workingDir est utilisé pour stocker la position dans laquel se situe l'explorateur*/
char workingDir[1000];

/*returnPath est la chaine de caractère ou se situe le chemin d'accès de l'élément séléctionné et validé*/
char returnPath[1000];

/*Cette structure est un objet correspondant à la fenêtre d'intéraction, demandant de validé ou non la selection*/
struct Validpop validpop;

/*variables similaire à selectLine elles corresponds à l'élement séléctionné sur la droite de la console*/
int selectPos = 0;

void explore(bool isProxyNeeded)
{
    /*La variables pos correspond au nombre total de Thread d'analyse existant
    Nous avons limité le nombre d'analyse à 5 donc 0 < pos < 5*/
    int pos = 0;
    /*La structure Data corresponds à l'ensemble des données nécéssaires, pour l'analyse.
    chacun des indices du tableaux data correponds aux données d'un des fichiers à analyser.*/
    struct Data data[5];

    /*Cette fonction copie le chemin d'acces dans lequel est lancé l'executable dans workingDir*/
    getcwd(workingDir, sizeof(workingDir));

    /*Cette fonction initialise la console en l'effacant*/
    resetScreen();
    initBorders();

    validpop.select = 0;


    /*Boucle principal, elle permet d'actualiser en permanence la console*/
    while(runExplorer)
    {

        if(validpop.Active)
        {
            /*Si la fenetre de la validation est active on "imprime" sur l'écran la fenetre */
            popup(validpop.filePath,validpop.select);
        }
        else
        {
            resetLeft(); //On efface la partie de gauche
            BordersTop(); //On imprime les bordures
            tree(workingDir,pos); //On imprime les fichiers/dossiers
        }

        if(!explorerSelect)
        {
            selectThreadPos(selectPos);

            if(pos > 0 && (data)[selectPos].finish)
            {
                print("Appuyer sur 'Entrer' pour selectionner",WIDTH/2+2,HEIGHT/2,112);
            }
            else if(pos >0)
            {
                print("Analyse en cours...",WIDTH/2+2,HEIGHT/2,112);
            }
        }
        else
        {
            print("Appuyer sur 'Entrer' pour selectionner",2,HEIGHT-1,112);
        }

        /*Cette fonction actualise les modifications à l'écran*/
        screen();


        /*La fonction int kbhit(void); indique si une touche du clavier a été frappée et que,
        par conséquent, un caractère est en attente d'être lu. Son appel renvoie 0 si aucun
        caractère n'est en attente et une valeur non nulle autrement. La lecture proprement
        dite est réalisée à l'aide de la fonction int getch(void);*/
        if (_kbhit())
        {
            int value = getch();

            if(!validpop.Active)
            {

                    switch(value) {
                case 72: //flèche du haut

                        if(explorerSelect)
                        {
                            if(scroll == selectLine && scroll>0)
                            {
                                scroll--;
                                selectLine--;
                            }
                            else if(selectLine != -1)
                            {
                            selectLine--;
                            }
                        }
                        else //Right side
                        {
                            resetBottomRight();

                            if(selectPos == 0 && pos >0)
                            {
                                selectPos = pos - 1;
                            }
                            else if(pos >0)
                            {
                                selectPos--;
                            }
                            initBorders();
                        }

                    break;
                case 80: //flèche du bas

                        if(explorerSelect) //left side
                        {
                            if(selectLine == HEIGHT - 6 + scroll && selectLine<(maxLine-1))
                            {
                                scroll++;
                                selectLine++;
                            }
                            else if(selectLine<(maxLine-1))
                            {
                               selectLine++;
                            }
                        }
                        else //right side
                        {
                            resetBottomRight();

                            if(selectPos == pos -1 && pos >0)
                            {
                                selectPos = 0;
                            }
                            else if(pos >0)
                            {
                                selectPos++;
                            }
                            initBorders();
                        }

                    break;
                case 13: //Bouton "enter"
                        pressedEnter = true;

                        if(!explorerSelect && pos > 0)
                        {
                            if((data)[selectPos].finish)
                            {
                                printResult((data)[selectPos]);
                                print("Appuyer sur 'o' pour ouvrir les liens",WIDTH/2+2,HEIGHT-1,112);
                            }
                            else
                            {
                                resetBottomRight();
                               print("Analyse en cours...",WIDTH/2+4,HEIGHT/2+4,122);
                            }
                            pressedEnter = false;
                        }
                        else if(selectLine == -1 && scroll ==0)
                        {
                            strcat(workingDir,"/..");
                            pressedEnter = false;
                        }

                    break;
                case 77: //flèche droite

                        if(pos != 0)
                        {

                            explorerSelect = !explorerSelect;

                            if(explorerSelect)
                            {
                                selectThreadPos(-1);
                            }

                             initBorders();
                             resetBottomRight();

                        }
                    break;
                case 75: //flèche gauche

                        if(pos != 0)
                        {

                            explorerSelect = !explorerSelect;
                            if(explorerSelect)
                            {
                                selectThreadPos(-1);
                            }

                             initBorders();
                             resetBottomRight();

                        }

                    break;
                case 111: //bouton 'o' pour open

                        if((data)[selectPos].finish)
                        {
                          int sz = 0;

                            while(sz < 7 && sz < (data)[selectPos].nbResult) //Deux condition, la première
                            {
                                ShellExecute(0, 0, (data)[selectPos].result[sz].url, 0, 0 , SW_SHOW );
                                sz++;
                            }
                        }


                    break;
                case 27: //Touche "echap"
                        runExplorer = false;
                        system("CLS");
                        printf("Program end");
                        exit(0);
                    break;
                    }



            }
            else /* Cas ou la fenetre de validation est active */
            {

                //Pressed right / left arrow switch
                if(value ==77 || value ==75)
                {
                   if(validpop.select == 1)
                    {
                        validpop.select =0;
                    }
                    else
                    {
                        validpop.select = 1;
                    }
                }

                //Pressed Escape or "No"
                if(value == 27 || (validpop.select==1 && value ==13))
                {
                    resetLeft();
                    validpop.Active = false;
                    validpop.filePath[0] = '\0';
                }

                //Pressed enter on "Yes"
                if(validpop.select==0 && value ==13)
                {
                    /**Ici l'utilisateur à validé la séléction
                    ainsi nous allons lancer l'analyse du fichier selectionné**/

                    int rc; //Code d'erreur relatif au threads
                    int MaxL=0; //Ceci est une variables temporaire.
                    pthread_t thread; //On déclare un thread.

                    /*Rappel data est un tableau de structure Data, elle sert a stoquer les informations
                    relative à chaque scan, elle est de dimension 5, 1 pour chaque scan (maximum 5)*/
                    (data)[pos].array = phase_1(returnPath,&MaxL,pos); //on recupère une liste de chaine de caractere, provenant de la phase 1
                    (data)[pos].isProxyNeeded = isProxyNeeded; //On précise si un proxy est nécéssaire
                    (data)[pos].MaxL = &MaxL; //On utilise notre variables temporaire pour la copie
                    (data)[pos].pos = pos; //On informe quels est la position du thread actuel
                    (data)[pos].finish = false; //On l'initialise
                    strcpy((data)[pos].filename,validpop.filePath); //On copie le NOM DU FICHIER

                    /*Lors de la création d'un thread, il est impossible d'envoyé plusieurs arguments,
                    on ne peut envoyer que une adresse indéfinit */
                    if( (rc=pthread_create( &thread, NULL, &phase_2, &(data[pos]) )) )
                    {
                        system("CLS");
                        printf("Une erreur est survenu lors de l'execution du programme: %d\n Merci de redemarrer le programme.", rc);
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        /**On lance le thread en detaché c'est à dire, qu'il va s'executer en "parralle" du code actuel
                        Information complémentaire :
                        => http://man7.org/linux/man-pages/man3/pthread_detach.3.html
                        => https://stackoverflow.com/a/10598449/10160890 **/
                        pthread_detach(thread);
                        pos++;
                    }

                    //Une fois le nouveau thread servant pour l'analyse
                    //créée on continue, et on réinitialise l'explorateur.
                    resetLeft();
                    validpop.Active = false;
                    validpop.filePath[0] = '\0';


                }






            }
        }


    }


    return;

}




void tree(char *basePath, int pos)
{

    int i,line = 0;
    char path[1000]; //Ceci est une variables temporaire pour faire des copies etc...
    struct dirent *dp; //Cette variables sert pour chaque élément
    DIR *dir = opendir(basePath); //On ouvre une directory

    if (!dir) //On vérifie qu'elle existe
        return;

    print((*dir).dd_name,2,0,112); //On imprime en haut a gauche le chemin d'accès


    /*on affiche tout en haut de l'explorateur une ligne d'indice -1 qui correspond à remonter*/
    //Si la l'utilisateur à selectionner cette élément, et qu'il est entrain d'utiliser l'explorateur
    if(selectLine == -1 && explorerSelect)
    {
    print("../",3,2,158); //On la surligne
    }
    else if(scroll == 0)
    {
    print("../",3,2,241); //On l'affiche normalement
    }




    /**Cette boucle va se réaliser, autant de fois qu'il y a d'élément dans le chemin donné**/
    while ((dp = readdir(dir)) != NULL)
    {
        if(line>=scroll)
        {

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            /**Ici on "fabrique" le chemin d'accès de chaque fichier
             on récupère le chemin de dossier dans lequel on est, on
             ajoute un / puis le nom de l'élement lu**/
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);


            /** Pour faire la différences entre un fichier et un dossier,
             on essaye d'ouvrir l'élément, si c'est un dossier, on aura
             un résultat, sinon on aura 0 **/
            if(opendir(path))
            { //ALORS C'EST UN DOSSIER

                /*Si l'utilisateur à appuyé sur enter ET
                 que la ligne actuel et la ligne selectionné
                 alors ouvrir le DOSSIER*/
                if(pressedEnter && selectLine == line)
                {
                    workingDir[0] = '\0';
                    selectLine = 0;
                    strcat(workingDir,path);
                    pressedEnter = false;
                    scroll = 0;
                    closedir(dir);
                    return;
                }



                if(line < HEIGHT - 4 + scroll)
                {
                    //Si la ligne selectionné est la bonne ligne ET que l'utilisateur utilise l'explorateur.
                    if(selectLine == line && explorerSelect)
                    {
                        print(append_(dp->d_name,'/'),3,3+line-scroll,158); //On la surligne
                    }
                    else
                    {
                        print(append_(dp->d_name,'/'),3,3+line-scroll,241); //On l'affiche normalement
                    }

                line++; //On incrémente le nombre de ligne
                }
            }
            else
            {//SINON C'EST UN FICHIER

                /*Si l'utilisateur à appuyé sur enter ET
                 que la ligne actuel et la ligne selectionné
                 ET que la fenetre de validation n'est pas ouverte
                 alors ouvrir FENETRE DE VALIDATION*/
                if(pressedEnter && selectLine == line && !validpop.Active)
                {
                    //Si le nombre d'élément scanner est < 5
                    if(pos < 5)
                    {
                        strcpy(returnPath,path);

                        pressedEnter = false;

                        strcpy(validpop.filePath,dp->d_name); //on met dans la fenetre le nom du fichier

                        validpop.Active = true; //On ouvre la fenetre
                    }
                    else
                    {
                        pressedEnter = false;
                    }
                }


                /*Si la ligne ne dépasse pas la console*/
                if(line < HEIGHT - 4 + scroll)
                {

                    //Si la ligne selectionné est la bonne ligne ET que l'utilisateur utilise l'explorateur.
                    if(selectLine == line && explorerSelect)
                    {
                        print(dp->d_name,3,3+line-scroll,158); //On la surligne
                    }
                    else
                    {
                        print(dp->d_name,3,3+line-scroll,246); //On l'affiche normalement
                    }


                int i = fsize(path); //On regarde quel taille fait un fichier
                char str[20];

                if(i > 100000000000)
                {
                    i = i/100000000000;
                    sprintf(str, "%d To", i);
                }
                else if(i > 100000000)
                {
                    i = i/100000000;
                    sprintf(str, "%d Go", i);
                }
                else if(i > 1000000)
                {
                    i = i/1000000;
                    sprintf(str, "%d Mo", i);
                }
                else if(i>1000)
                {
                    i = i/1000;
                    sprintf(str, "%d Ko", i);
                }
                else
                {
                    sprintf(str, "%d O", i);
                }


                    print(str,WIDTH/2 - 2 - strlen(str),3+line-scroll,248); //On imprime sur la console



                line++; //On incrémente le nombre de ligne
                }
            }


        }
    }
    else
    {
        line++;
    }

    }

    maxLine = line; //On note le nombre de ligne affiché

    closedir(dir);//On ferme le chemin d'acces
}

/* Explication
=> https://stackoverflow.com/a/238609/10160890 */
off_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1;
}



char* append_(const char *s, char c)
{
    int len = strlen(s);
    char buf[len+2];
    strcpy(buf, s);
    buf[len] = c;
    buf[len + 1] = 0;
    return strdup(buf); //La fonction strdup() renvoie un pointeur sur une nouvelle chaîne de caractères qui est dupliquée depuis s
}
