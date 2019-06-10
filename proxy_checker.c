#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Fichiers d'en-t�te pour la librairie Curl.*/
#include <curl/curl.h>

int proxy_checker()
{

    /* Procedure basique de connection � un url donn�e r�f�rences : https://curl.haxx.se/libcurl/ */
    CURL *curl_handle;
    CURLcode res;

    /*On met en place l'environnement libcurl avec curl_global_init()*/
    curl_global_init(CURL_GLOBAL_ALL);

    /*On initialise la variables CURL*/
    curl_handle = curl_easy_init();

    /*On sp�cifie l'url dans les param�tres*/
    curl_easy_setopt(curl_handle, CURLOPT_URL, "https://www.google.fr/");

    /*On demande � la librairie de travailler uniquement avec le Header de la conneciton*/
    curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1);

      //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

      //curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    /*On ignore les v�rifications SSL (trop compliqu� et sources de nombreuses erreur) */
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, FALSE);

    /*On lance la connection, et on r�cup�re le code r�sultant*/
    res = curl_easy_perform(curl_handle);

    /*On nettoie l'environnement pour �viter les "memory leak" */
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return res;
}

