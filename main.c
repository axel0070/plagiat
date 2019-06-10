#include <stdio.h>
#include <stdlib.h>

/* Fichiers d'en-t�te pour la librairie Curl.*/
#include <curl/curl.h>

/*Fichiers d'en-t�te pour les fonctions relatives aux Threads.*/
#include <pthread.h>

/* Fichiers d'en-t�te pour les appelles de fonctions */
#include "ThreadScreen.h"
#include "fileexplorer.h"


main()
{

    int rc1; /* code d'erreur */
    pthread_t thread1; /* Structure correspondant � un Thread */

    /**Un thread est similaire � un processus car tous deux repr�sentent l'ex�cution d'un ensemble
    d'instructions du langage machine d'un processeur.
    Du point de vue de l'utilisateur, ces ex�cutions semblent se d�rouler en parall�le. Toutefois, l�
    o� chaque processus poss�de sa propre m�moire virtuelle, les threads d'un m�me processus se partagent sa
    m�moire virtuelle. Par contre, tous les threads poss�dent leur propre pile d'ex�cution. **/

    bool isProxyNeeded; /* Variable pouvant �tre "vrai" ou "faux" suivant le besoin */
    CURLcode res; /* Code obtenu apr�s une connection */

    res = proxy_checker(); /* Connection internet Synchrome pour verifier si un proxy est n�c�ssaire */

    switch ( res )
    {
        case CURLE_OK :
            isProxyNeeded = false;
        break;
        case CURLE_COULDNT_CONNECT :
            isProxyNeeded = true;
        break;
        default :
            printf("\nErreur de connection (%d)",res);
            printf("\nVeuillez vous r�f�rer � https://curl.haxx.se/libcurl/c/libcurl-errors.html pour plus d'information");
            return res;
    }

    /* Cr�ation d'un thread pour la fonction "explore" */
    if( (rc1=pthread_create( &thread1, NULL, &explore, isProxyNeeded)) )
    {
        printf("Thread creation failed: %d\n", rc1);
    }

    /** pthread_join() suspend l'ex�cution du thread appelant (Thread principal) jusqu'� ce que le thread identifi� par thread1 ach�ve son ex�cution**/
    pthread_join( thread1, NULL);

    /*Fin du processus*/
    exit(EXIT_SUCCESS);
}
