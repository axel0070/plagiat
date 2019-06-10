#include <stdio.h>
#include <tidy.h>
#include <string.h>
#include <tidybuffio.h>
#include <curl/curl.h>

#include "minunit.h"
#include "uri_encode.h"
#include "phase_2.h"
#include "ThreadScreen.h"


//Variables globals.
struct Link link[5][MAX_LINK];
int gCounter = -1;
int NextLine = 0;

char memory[50000];

bool BIGERROR = false;
//int phase_2(char ** array,bool isProxyNeeded, int *MaxL)
void phase_2(void *vargp)
{
    char *reel_var[MAX_LINE];
    int j =0;
    struct Data *data;
    int pos = 0 ;
    int totalLine =0;


    data = vargp;
    pos = (*data).pos;
    totalLine = *(*data).MaxL;
    (*data).totalLine = totalLine;
    (*data).finish = false;

    char filename[120] = "";

    strcpy(filename,(*data).filename);


    initLink(pos);

    while( strlen(*((*data).array + j))>0 )
    {
            CURL *curl = NULL;
            char curl_errbuf[CURL_ERROR_SIZE];
            TidyDoc tdoc;
            TidyBuffer docbuf = {0};
            TidyBuffer tidy_errbuf = {0};
            int err;
            struct curl_slist *list = NULL;

        float progress = (float)j/ totalLine *50;

        reel_var[j] = *((*data).array + j);
        //printf( "\n*(tab + %d) : %s", j, reel_var[j]);

        progressBar(pos,(int) progress,filename);
        //printf("\nProgress %f | pos %d",progress,pos);


            char *uri = reel_var[j];

            const size_t len = strlen(uri);
            char buffer[ len * 3 + 1 ];
            buffer[0] = '\0';
            uri_encode(uri, len,  buffer);

            //printf("ENCODE: %s",buffer);

            char *url_ = "https://www.google.com/search?q=";
            for(int i =0; i < strlen(buffer);i++)
            {
                url_ = append_(url_,buffer[i]);
            }

            //This allow to avoid to be kicked from google.com
            Sleep(200);


            curl = curl_easy_init();


            //printf("\ndata.isProxyNeeded: %d",(*data).isProxyNeeded);
            /**Only on UTBM network.**/
            if((*data).isProxyNeeded)
            {
                curl_easy_setopt(curl, CURLOPT_PROXY, "http://proxy.utbm.fr:3128");
            }

    //printf("\nURL: %s",url_);

    curl_easy_setopt(curl, CURLOPT_URL, url_);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
    //curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);

    tdoc = tidyCreate();
    tidyOptSetBool(tdoc, TidyForceOutput, yes); /* try harder */
    tidyOptSetInt(tdoc, TidyWrapLen, 4096);//http: //wpad.utbm.local/wpad.dat

    tidySetErrorBuffer(tdoc, &(tidy_errbuf));
    tidyBufInit(&(docbuf));

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(docbuf));


                /**Simulate a real browser**/
    curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1L);
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");

    list = curl_slist_append(list, "accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
    list = curl_slist_append(list, "accept-language: fr-FR,fr;q=0.9,en-US;q=0.8,en;q=0.7");
    list = curl_slist_append(list, "cache-control: max-age=0");
    list = curl_slist_append(list, "upgrade-insecure-requests: 1");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.119 Safari/537.36");


    err = curl_easy_perform(curl);
    if(!err) {
      err = tidyParseBuffer(tdoc, &(docbuf)); /* parse the input */
      if(err >= 0) {
        err = tidyCleanAndRepair(tdoc); /* fix any problems */
        if(err >= 0) {
          err = tidyRunDiagnostics(tdoc); /* load tidy error buffer */
          if(err >= 0) {

            dumpNode(tdoc, tidyGetRoot(tdoc), 0,pos); /* walk the tree */

            /** There is error but not showing to the console because pointless**/
            //fprintf(stderr, "%s\n", tidy_errbuf.bp); /* show errors */
          }
        }
      }
    }
    else
    {
        fprintf(stderr, "%s\n", curl_errbuf);
    }



    //BOUCLE WHILE POUR CHAQUE PHRASE
    j++;

    /* clean-up */
    curl_easy_cleanup(curl);
    tidyBufFree(&docbuf);
    tidyBufFree(&tidy_errbuf);
    tidyRelease(tdoc);

    }

    progressBar(pos,50,filename);

    //printf("\nFinish");
    //listLink(pos);
    //result();

    //listLink(pos);

    char *buffer = NULL;
    int temp = 0;
    int total = estimation(pos);
    int positiveResult = 0;

    while((buffer = result(pos)) != NULL)
    {
        if(strlen(buffer)>1)
        {
        float progress = (float)temp/ total *50+50;
        int b = download((*data).isProxyNeeded,reel_var,buffer,j,pos);
        if(b)
        {
            strcpy( (*data).result[positiveResult].url,buffer);
            (*data).result[positiveResult].occurence = b;
            positiveResult++;
            (*data).nbResult = positiveResult;
        }
        Sleep(1000);
        progressBar(pos,(int) progress,filename);
        //printf("\n-------->Temp : %d",temp);
        temp ++;
        }
        //download((*data).isProxyNeeded,reel_var,buffer,j,pos);
    }

    progressBar(pos,100,filename);


    (*data).finish = true;
    //result(pos);

    //download(0,reel_var[0]);

    //free(data);
    //free(list);
    //free(reel_var);


    //printf("\nEnd");

    //return err;
    //initLink(pos);


    //return 0;

}





/* curl write callback, to fill tidy's input buffer...  */
uint write_cb(char *in, uint size, uint nmemb, TidyBuffer *out)
{
  uint r;
  r = size * nmemb;
  tidyBufAppend(out, in, r);
  return r;
}

/* Traverse the document tree */
void dumpNode(TidyDoc doc, TidyNode tnod, int indent,int pos) //Cette fonctions est appellé pour chaque ligne.
{

  TidyNode child;

  for(child = tidyGetChild(tnod); child; child = tidyGetNext(child) ) {
    ctmbstr name = tidyNodeGetName(child);
    if(name) {
      /* if it has a name, then it's an HTML tag ... */
      TidyAttr attr;
      //printf("NAME:%*.*s%s ", indent, indent, "<", name);


          //printf("%*.*s%s", indent, indent, "<", name);
        /* walk the attribute list */
      for(attr = tidyAttrFirst(child); attr; attr = tidyAttrNext(attr) ) {


        if(strstr(tidyAttrName(attr),"class") != NULL && strstr(tidyAttrValue(attr),"g") != NULL && strlen(tidyAttrValue(attr)) < 3)
        {
                    //printf("\nResult:|%s|%s|",tidyAttrName(attr),tidyAttrValue(attr)); //La on vient de passer la ligne class="g"
                    gCounter++;

        }else if(gCounter>= 0 && strstr(tidyAttrName(attr),"class") != NULL && strstr(tidyAttrValue(attr),"r") != NULL && strlen(tidyAttrValue(attr)) < 3)
        {
                   NextLine = 1;
                   /**
                   After the <class='r'> the <a> arrive and contain the href which is the link to the website.
                   **/
        }
        else if(NextLine>0)
        {

        char *link = tidyAttrValue(attr);

        //link?printf("LINK=|%s|\n",link):printf(" ");

        if(link)
        {

            if(strstr(link,"LC20lb"))
            {
               // system("CLS");
               // printf("ERROR DETECTED THIS SHOULD NEVER HAPPEN RESTART THE PROGRAM");
                //exit(-1);
            }
            else
            {
            //printf("\nLINK=|%s| pos %d",link,pos); //attention si il y a "sFZIhb" alors c'est un pdf.
            addlink(link,pos);
            NextLine = 0;
            }

        }
        else
        {
            NextLine = 0;
        }



        }


      }
      //printf(">\n");



    }
    dumpNode(doc, child, indent + 4,pos); /* recursive */
  }
}

void initLink(int pos)
{

    for(int i =0; i < MAX_LINK; i++)
    {
             link[pos][i].occurence = 0;
             strcpy(link[pos][i].url,"");
    }


}



void addlink(char *url,int pos)
{

    //printf("\nNew url pos %d : %s",pos,url);
    for(int i =0; i < MAX_LINK; i++)
    {
        if(strcmp(url, link[pos][i].url)== 0)//return 0 si identique, sinon autres choses.
        {
            //printf("\n-Found inside- |%s|",url);
            link[pos][i].occurence++;
            return;
        }
    }

    //La chaine n'est pas présente dans le tableau de structure. On l'ajoute
    for(int i =0; i < MAX_LINK; i++)
    {
        if(strlen(link[pos][i].url) ==0)//return 0 si identique, sinon autres choses.
        {
            //printf("\n-ADDED-");
            strcpy(link[pos][i].url,url);
            //link[pos][i].url =url;
            return;
        }
    }


}


/*Cete fonction compte le nombre de lien avec une occurence > 0
=> Elle renvoit combien de lien sont apparu au moins une fois*/
int estimation(int pos)
{

    int count = 0;

    for(int i =0; i < MAX_LINK; i++)
    {
        if(strlen(link[pos][i].url)>0)
        {
            if(link[pos][i].occurence)
            {
                count++;
            }

        }

    }

    return count;
}


/**La fonction résult renvoi l'url le plus occurente dans le tableau link[pos]
=> Elle renvoit le lien qui a été trouvé le plus de fois, et elle le suprime du tableau après**/
char *result(int pos)
{
    int posMax = -1; //Correspondra à la position dans le tableau du lien le plus apparu
    int valueMax = 0; //Corresponds à la valeur maximum ...


    /*On scan tous les liens*/
    for(int i =0; i < MAX_LINK; i++)
    {
        if(strlen(link[pos][i].url)>0) //Si le lien existe
        {
            if(link[pos][i].occurence) //Si le lien est apparu au moins une fois
            {

                if(valueMax<link[pos][i].occurence) //si la nouvelle occurence testé est meilleur que la précédente
                {
                    posMax = i; //Alors maj index
                    valueMax = link[pos][i].occurence; //Update
                }

            }

        }

    }


    //Maintenant, si posMax a été actualisé, c'est à dire, que dans tous les liens obtenu, au moins 1 est apparu plus d'une fois
    if(posMax>=0)
    {
            link[pos][posMax].occurence = 0; //On reset

            if(strstr(link[pos][posMax].url,".pdf") != NULL) //On verifie si ce ne serait pas un pdf.. histoire d'éviter les bugs
            {
                return ""; //on renvoit rien, car si on renvoit NULL le scan s'arrête.
            }


        return link[pos][posMax].url; //On renvoit la chaine de caracteres
    }


    return NULL;
}











