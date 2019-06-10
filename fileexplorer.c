#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

/*Fichiers d'en-t�te pour les fonctions relatives aux fichiers.*/
#include <dirent.h>
#include <sys/stat.h>

/*Fichiers d'en-t�te pour les fonctions relatives aux Threads.*/
#include <unistd.h>
#include <pthread.h>

/*Fichiers d'en-t�te pour les fonctions relatives � la console.*/
#include <windows.h>

/* Fichiers d'en-t�te pour les appelles de fonctions */
#include "def.h"
#include "fileexplorer.h"
#include "phase_2.h"

/**Variables globales**/

/*La variables runExplorer est utilis� pour la boucle principale*/
bool runExplorer = true;

/*La console est divis� en deux parties (Gauche / Droite )
l'utilisateur � l'aide des fl�che peut passer de gauche � droite
lorsque l'explorateur de fichier est selectionn� explorerSelect = true sinon false*/
bool explorerSelect = true;

/*A l'aide des fl�ches haut/bas l'utilisateur peut naviguer entres les fichiers et les dossier.
selectLine correspond � la ligne selectionn�, l'indice est utilis� pour savoir
quel �lement l'utilisateur � choisie lorsqu'il clique sur "ENTER" mais �galement, quels �lements
doit �tre surlign� */
int selectLine = 0;

/*maxLine est une variable utilis� pour indiquer, le nombre d'�lement afficher sur la console.*/
int maxLine = 0;

/*Si le nombre de fichiers/dossiers dans le chemin d'acc�s sp�cifi� d�passe le nombre de ligne de la console
l'utilisateur peut descendre avec les fl�ches pour "scroller" et afficher les autres fichiers,
scroll corresponds au d�calage. */
int scroll = 0;

/*La variables pressedEnter est modifi� lorsque l'utilisateur appuis sur Enter*/
bool pressedEnter = false;

/*la chaine de caract�re workingDir est utilis� pour stocker la position dans laquel se situe l'explorateur*/
char workingDir[1000];

/*returnPath est la chaine de caract�re ou se situe le chemin d'acc�s de l'�l�ment s�l�ctionn� et valid�*/
char returnPath[1000];

/*Cette structure est un objet correspondant � la fen�tre d'int�raction, demandant de valid� ou non la selection*/
struct Validpop validpop;

/*variables similaire � selectLine elles corresponds � l'�lement s�l�ctionn� sur la droite de la console*/
int selectPos = 0;

void explore(bool isProxyNeeded)
{
    /*La variables pos correspond au nombre total de Thread d'analyse existant
    Nous avons limit� le nombre d'analyse � 5 donc 0 < pos < 5*/
    int pos = 0;
    /*La structure Data corresponds � l'ensemble des donn�es n�c�ssaires, pour l'analyse.
    chacun des indices du tableaux data correponds aux donn�es d'un des fichiers � analyser.*/
    struct Data data[5];

    /*Cette fonction copie le chemin d'acces dans lequel est lanc� l'executable dans workingDir*/
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
            /*Si la fenetre de la validation est active on "imprime" sur l'�cran la fenetre */
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

        /*Cette fonction actualise les modifications � l'�cran*/
        screen();


        /*La fonction int kbhit(void); indique si une touche du clavier a �t� frapp�e et que,
        par cons�quent, un caract�re est en attente d'�tre lu. Son appel renvoie 0 si aucun
        caract�re n'est en attente et une valeur non nulle autrement. La lecture proprement
        dite est r�alis�e � l'aide de la fonction int getch(void);*/
        if (_kbhit())
        {
            int value = getch();

            if(!validpop.Active)
            {

                    switch(value) {
                case 72: //fl�che du haut

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
                case 80: //fl�che du bas

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
                case 77: //fl�che droite

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
                case 75: //fl�che gauche

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

                            while(sz < 7 && sz < (data)[selectPos].nbResult) //Deux condition, la premi�re
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
                    /**Ici l'utilisateur � valid� la s�l�ction
                    ainsi nous allons lancer l'analyse du fichier selectionn�**/

                    int rc; //Code d'erreur relatif au threads
                    int MaxL=0; //Ceci est une variables temporaire.
                    pthread_t thread; //On d�clare un thread.

                    /*Rappel data est un tableau de structure Data, elle sert a stoquer les informations
                    relative � chaque scan, elle est de dimension 5, 1 pour chaque scan (maximum 5)*/
                    (data)[pos].array = phase_1(returnPath,&MaxL,pos); //on recup�re une liste de chaine de caractere, provenant de la phase 1
                    (data)[pos].isProxyNeeded = isProxyNeeded; //On pr�cise si un proxy est n�c�ssaire
                    (data)[pos].MaxL = &MaxL; //On utilise notre variables temporaire pour la copie
                    (data)[pos].pos = pos; //On informe quels est la position du thread actuel
                    (data)[pos].finish = false; //On l'initialise
                    strcpy((data)[pos].filename,validpop.filePath); //On copie le NOM DU FICHIER

                    /*Lors de la cr�ation d'un thread, il est impossible d'envoy� plusieurs arguments,
                    on ne peut envoyer que une adresse ind�finit */
                    if( (rc=pthread_create( &thread, NULL, &phase_2, &(data[pos]) )) )
                    {
                        system("CLS");
                        printf("Une erreur est survenu lors de l'execution du programme: %d\n Merci de redemarrer le programme.", rc);
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        /**On lance le thread en detach� c'est � dire, qu'il va s'executer en "parralle" du code actuel
                        Information compl�mentaire :
                        => http://man7.org/linux/man-pages/man3/pthread_detach.3.html
                        => https://stackoverflow.com/a/10598449/10160890 **/
                        pthread_detach(thread);
                        pos++;
                    }

                    //Une fois le nouveau thread servant pour l'analyse
                    //cr��e on continue, et on r�initialise l'explorateur.
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
    struct dirent *dp; //Cette variables sert pour chaque �l�ment
    DIR *dir = opendir(basePath); //On ouvre une directory

    if (!dir) //On v�rifie qu'elle existe
        return;

    print((*dir).dd_name,2,0,112); //On imprime en haut a gauche le chemin d'acc�s


    /*on affiche tout en haut de l'explorateur une ligne d'indice -1 qui correspond � remonter*/
    //Si la l'utilisateur � selectionner cette �l�ment, et qu'il est entrain d'utiliser l'explorateur
    if(selectLine == -1 && explorerSelect)
    {
    print("../",3,2,158); //On la surligne
    }
    else if(scroll == 0)
    {
    print("../",3,2,241); //On l'affiche normalement
    }




    /**Cette boucle va se r�aliser, autant de fois qu'il y a d'�l�ment dans le chemin donn�**/
    while ((dp = readdir(dir)) != NULL)
    {
        if(line>=scroll)
        {

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            /**Ici on "fabrique" le chemin d'acc�s de chaque fichier
             on r�cup�re le chemin de dossier dans lequel on est, on
             ajoute un / puis le nom de l'�lement lu**/
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);


            /** Pour faire la diff�rences entre un fichier et un dossier,
             on essaye d'ouvrir l'�l�ment, si c'est un dossier, on aura
             un r�sultat, sinon on aura 0 **/
            if(opendir(path))
            { //ALORS C'EST UN DOSSIER

                /*Si l'utilisateur � appuy� sur enter ET
                 que la ligne actuel et la ligne selectionn�
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
                    //Si la ligne selectionn� est la bonne ligne ET que l'utilisateur utilise l'explorateur.
                    if(selectLine == line && explorerSelect)
                    {
                        print(append_(dp->d_name,'/'),3,3+line-scroll,158); //On la surligne
                    }
                    else
                    {
                        print(append_(dp->d_name,'/'),3,3+line-scroll,241); //On l'affiche normalement
                    }

                line++; //On incr�mente le nombre de ligne
                }
            }
            else
            {//SINON C'EST UN FICHIER

                /*Si l'utilisateur � appuy� sur enter ET
                 que la ligne actuel et la ligne selectionn�
                 ET que la fenetre de validation n'est pas ouverte
                 alors ouvrir FENETRE DE VALIDATION*/
                if(pressedEnter && selectLine == line && !validpop.Active)
                {
                    //Si le nombre d'�l�ment scanner est < 5
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


                /*Si la ligne ne d�passe pas la console*/
                if(line < HEIGHT - 4 + scroll)
                {

                    //Si la ligne selectionn� est la bonne ligne ET que l'utilisateur utilise l'explorateur.
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



                line++; //On incr�mente le nombre de ligne
                }
            }


        }
    }
    else
    {
        line++;
    }

    }

    maxLine = line; //On note le nombre de ligne affich�

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
    return strdup(buf); //La fonction strdup() renvoie un pointeur sur une nouvelle cha�ne de caract�res qui est dupliqu�e depuis s
}
