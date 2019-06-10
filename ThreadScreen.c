#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h> /* for HANDLE type, and console functions */
#include <string.h>

#include <unistd.h>  //Header file for Sleep().
#include <pthread.h> //Header file for Threads.

#include "ThreadScreen.h"

//Variables
HANDLE wHnd; /* write (output) handle */

static struct block map[WIDTH][HEIGHT];

int run = 1;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int  counter = 0;



/*Cette fonctions permet d'afficher sur la console,
les données stoqué dans le tableau a deux dimension map
Informations complémentaire :
=> https://stackoverflow.com/questions/38343232/outputs-in-console-with-writeconsoleoutputa
=> https://docs.microsoft.com/en-us/windows/console/writeconsoleoutput
=> http://cecilsunkure.blogspot.com/2011/11/windows-console-game-setting-up-window.html */
void screen()
{


        int x, y;

        SetConsoleTitle("Detecteur de Plagiat");

        /* On définit les coordonnées des coins de la console
        => https://docs.microsoft.com/en-us/windows/console/small-rect-str */
        SMALL_RECT windowSize = {0, 0, WIDTH - 1, HEIGHT - 1};
        SMALL_RECT consoleWriteArea = {0, 0, WIDTH - 1, HEIGHT - 1};

        /* Les structures COORD sont utilisés pour spécifié des positions sur l'écran.
        => https://docs.microsoft.com/en-us/windows/console/coord-str*/
        COORD bufferSize = {WIDTH, HEIGHT};
        COORD characterBufferSize = {WIDTH, HEIGHT};
        COORD characterPosition = {0, 0};

        /* On définit une structure CHAR_INFO qui contiendra toutes les données pour chaques caractères */
        CHAR_INFO consoleBuffer[WIDTH * HEIGHT];

        /* On récupère la sortie */
        wHnd = GetStdHandle(STD_OUTPUT_HANDLE);

        /* On définit la taille de la fenetre */
        SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

        /* On définit la taille de la fenetre virtuel (tempon) */
        SetConsoleScreenBufferSize(wHnd, bufferSize);

        for (y = 0; y < HEIGHT; y++)
        {
            for (x = 0; x < WIDTH; x++)
            {
                    int color = map[x][y].color;
                    consoleBuffer[x + WIDTH * y].Char.AsciiChar = map[x][y].character;
                    consoleBuffer[x + WIDTH * y].Attributes = color;
            }
        }

        /* On copie les données situé dans consoleBuffer pour les envoyées dans une zone rectangulaire de taille spécifié par
        characterBufferSize et characterPosition situé dans la zone de la console indiqué par consoleWriteArea*/
        WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);



}

/*Cette fonction remplis le tableau map de caractère █ de couleur blanche*/
void resetScreen()
{

    for(int x = 0 ; x < WIDTH; x++ )
      {

        for(int y=0; y < HEIGHT; y++)
        {
                map[x][y].color = 255;
                map[x][y].character = 219;
        }

      }


}

/* Cette fonction affiche une fenetre sur la partie gauche de l'écran*/
void popup(char * msg,int select)
{

    //Impression du font bleu de la fenetre
    for(int x = WIDTH/2-strlen(msg)/2-3 - WIDTH/4; x < WIDTH/2+strlen(msg)/2+3- WIDTH/4; x++ )
      {

        for(int y= HEIGHT/2-5; y < HEIGHT/2+5; y++)
        {
                map[x][y].character = 219; //█
                map[x][y].color = 51; //bleu
        }

      }


        print("Scanner ?",WIDTH/2-strlen(msg)/2- WIDTH/4,HEIGHT/2-3,48); //Impression de text
        print(msg,WIDTH/2-strlen(msg)/2- WIDTH/4,HEIGHT/2-2,48); //Impression du nom du fichier

        //L'utilisateur peut choisir entre oui/non
      if(select == 0)
      {
        print("Oui",WIDTH/2-5- WIDTH/4,HEIGHT/2 +1,32);
        print("Non",WIDTH/2+2- WIDTH/4,HEIGHT/2 +1,48);
      }
      else
      {
        print("Oui",WIDTH/2-5- WIDTH/4,HEIGHT/2 +1,48);
        print("Non",WIDTH/2+2- WIDTH/4,HEIGHT/2 +1,192);
      }



}

void BordersTop()
{
    for(int x = 0 ; x < WIDTH; x++ )
    {
        map[x][0].color = 119;
        map[x][0].character = 219;
    }
}


/*Ici on colorie les bordures*/
void initBorders()
{

    for(int x = 0 ; x < WIDTH; x++ )
    {
        map[x][0].color = 119;
        map[x][0].character = 219;

        map[x][HEIGHT-1].color = 119;
        map[x][HEIGHT-1].character = 219;
    }


    for(int y=0; y < HEIGHT; y++)
    {
        map[0][y].color = 119;
        map[0][y].character = 219;

        map[WIDTH-1][y].color = 119;
        map[WIDTH-1][y].character = 219;

        map[1][y].color = 119;
        map[1][y].character = 219;

        map[WIDTH-2][y].color = 119;
        map[WIDTH-2][y].character = 219;

        map[WIDTH/2][y].color = 119;
        map[WIDTH/2][y].character = 219;

        map[WIDTH/2 +1][y].color = 119;
        map[WIDTH/2 +1][y].character = 219;
    }

}

//On remet en blanc uniquement la partie de gauche
void resetLeft()
{
    /**En effet durant le programme, la partie de gauche est tout le temps reset et remis au propre
    la partie de droite est laissé tel quelles tout au long du programme **/
    for(int i =2; i < WIDTH/2-1;i++)
    {

        for(int j = 0; j < HEIGHT -1; j++)
        {
            map[i][j].color = 255;
            map[i][j].character = 219;
        }


    }

}

void resetBottomRight()
{

    for(int i = WIDTH/2 + 2; i < WIDTH-2;i++)
    {
        map[i][HEIGHT/2].color = 119;
        map[i][HEIGHT/2].character = 219;
    }


    for(int i = WIDTH/2 + 2; i < WIDTH-2;i++)
    {
        for(int j = HEIGHT/2+1; j < HEIGHT -1; j++)
        {
            map[i][j].color = 255;
            map[i][j].character = 219;
        }
    }


}


void printResult(struct Data output)
{
    int j = 0;



   resetBottomRight();



    print(output.filename,WIDTH/4 * 3 - strlen(output.filename),HEIGHT/2+1,248); //On imprime le nom du fichier préalablement scanné

    while(j < 7 && j < output.nbResult) //Deux condition, la première
    {
        char str[1000];

        //Ici plag correponds au pourcentage de plagiat dans le fichier.
        float plag = (float) output.result[j].occurence/ (output.totalLine) * 100;

        //On fait une jolie phrase pour chaque lien
        sprintf(str, "%d%% pour %s",(int) plag, output.result[j].url);

        if(strlen(str)>WIDTH/2-4)
        {
            str[WIDTH/2-4] = '\0';
            str[WIDTH/2-5] = '.';
            str[WIDTH/2-6] = '.';
        }

        //On définit une couleurs différences en fonctions du %
        if((int)plag>50)
        {
             print(str,WIDTH/2+2,HEIGHT/2+3+2*j,244);
        }
        else if((int)plag>40)
        {
             print(str,WIDTH/2+2,HEIGHT/2+3+2*j,252);
        }
        else if((int)plag>30)
        {
             print(str,WIDTH/2+2,HEIGHT/2+3+2*j,246);
        }
        else if((int)plag>20)
        {
             print(str,WIDTH/2+2,HEIGHT/2+3+2*j,240);
        }
        else if((int)plag>10)
        {
             print(str,WIDTH/2+2,HEIGHT/2+3+2*j,241);
        }
        else
        {
             print(str,WIDTH/2+2,HEIGHT/2+3+2*j,242);
        }

        j++;
    }



}



/*Imprime la barre de progrès*/
void progressBar(int pos,int progress, char *filename)
{
    int start = WIDTH/2+WIDTH/8;
    int end = WIDTH/2+3*WIDTH/8;

    int dif = end - start;

    int prog = progress*dif / 100;

    int posH = HEIGHT/2/5 + 3*pos;

    char str[10];
    sprintf(str, "%d%%", progress);
    print(str,start,posH -1,250);
    print(filename,start+ 5,posH-1,250);

    for(int i = start; i < start + prog;i++)
    {
        map[i][posH].color = 2;
        map[i][posH].character = 219;
    }



}


/*Cette fonction va imprimer a cote du temps de chargement
 un barre bleu pour indiquer qu'elle est selectionné*/
void selectThreadPos(int pos)
{ //contour color 136

    int color = 0;
    int start = WIDTH/2+WIDTH/8;
    int end = WIDTH/2+3*WIDTH/8;

    int posH;

    for(int w =0; w < 5; w++)
    {
        posH = HEIGHT/2/5 + 3*w;;

        if(w ==pos)
        {
            color = 153;
        }
        else
        {
           color = 255;
        }

        map[start-4][posH].color = color;
        map[start-4][posH].character = 219;

        map[start-4][posH-1].color = color;
        map[start-4][posH-1].character = 219;


    }


}


/*Cette fonction va imprimer du text,
 a l'emplacement donnée en x,y et de la bonne couleur*/
void print(char *text,int x,int y, int color)
{
    for(int i =0; i<strlen(text);i++)
    {
        if(i > WIDTH-1)
        {
            return;
        }
        map[x+i][y].character = (unsigned char) text[i];
        map[x+i][y].color = color;
    }
}
