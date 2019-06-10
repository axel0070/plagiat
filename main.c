#include <stdio.h>
#include <stdlib.h>

/* Fichiers d'en-tête pour la librairie Curl.*/
#include <curl/curl.h>

/*Fichiers d'en-tête pour les fonctions relatives aux Threads.*/
#include <pthread.h>

/* Fichiers d'en-tête pour les appelles de fonctions */
#include "ThreadScreen.h"
#include "fileexplorer.h"


main()
{

    int rc1; /* code d'erreur */
    pthread_t thread1; /* Structure correspondant à un Thread */

    /**Un thread est similaire à un processus car tous deux représentent l'exécution d'un ensemble
    d'instructions du langage machine d'un processeur.
    Du point de vue de l'utilisateur, ces exécutions semblent se dérouler en parallèle. Toutefois, là
    où chaque processus possède sa propre mémoire virtuelle, les threads d'un même processus se partagent sa
    mémoire virtuelle. Par contre, tous les threads possèdent leur propre pile d'exécution. **/

    bool isProxyNeeded; /* Variable pouvant être "vrai" ou "faux" suivant le besoin */
    CURLcode res; /* Code obtenu après une connection */

    res = proxy_checker(); /* Connection internet Synchrome pour verifier si un proxy est nécéssaire */

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
            printf("\nVeuillez vous référer à https://curl.haxx.se/libcurl/c/libcurl-errors.html pour plus d'information");
            return res;
    }

    /* Création d'un thread pour la fonction "explore" */
    if( (rc1=pthread_create( &thread1, NULL, &explore, isProxyNeeded)) )
    {
        printf("Thread creation failed: %d\n", rc1);
    }

    /** pthread_join() suspend l'exécution du thread appelant (Thread principal) jusqu'à ce que le thread identifié par thread1 achève son exécution**/
    pthread_join( thread1, NULL);

    /*Fin du processus*/
    exit(EXIT_SUCCESS);
}
