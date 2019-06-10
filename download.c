#include <stdio.h>
#include <tidy.h>
#include <stdlib.h>
#include <string.h>
#include <tidybuffio.h>
#include <curl/curl.h>
#include "minunit.h"
#include "uri_encode.h"
#include "minunit.h"
#include "def.h"

//Prototypes de fonctions
uint write_cb_dl(char *in, uint size, uint nmemb, TidyBuffer *out); //== Résultat Curl
void dumpNode_dl(TidyDoc doc, TidyNode tnod, int indent, char *memoryTemp); //==analyse des noeux XML ( HTML )


int download(bool isProxyNeeded,char *test[], char* url,int NbLine,int pos)
{
    CURL *curl = NULL;
    char curl_errbuf[CURL_ERROR_SIZE];
    TidyDoc tdoc;
    TidyBuffer docbuf = {0};
    TidyBuffer tidy_errbuf = {0};
    int err;
    struct curl_slist *list = NULL;


    char memoryTemp[10000];
    //printf("new download start");

    memoryTemp[0] = '\0';



    curl = curl_easy_init();
    int plagiat = 0;


        /**Only on UTBM network.**/
        if(isProxyNeeded)
        {
            curl_easy_setopt(curl, CURLOPT_PROXY, "http://proxy.utbm.fr:3128");
        }

        //printf("\nDow : %s",url);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
    //curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb_dl);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);

    tdoc = tidyCreate();
    tidyOptSetBool(tdoc, TidyForceOutput, yes); /* try harder */
    tidyOptSetInt(tdoc, TidyWrapLen, 4096);//http: //wpad.utbm.local/wpad.dat

    tidySetErrorBuffer(tdoc, &tidy_errbuf);
    tidyBufInit(&docbuf);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &docbuf);



    err = curl_easy_perform(curl);

   // printf("\nError ????%d",err);
    if(!err) {
      err = tidyParseBuffer(tdoc, &docbuf); /* parse the input */
      if(err >= 0) {
        err = tidyCleanAndRepair(tdoc); /* fix any problems */
        if(err >= 0) {
          err = tidyRunDiagnostics(tdoc); /* load tidy error buffer */
          if(err >= 0) {
            dumpNode_dl(tdoc, tidyGetRoot(tdoc), 0,memoryTemp); /* walk the tree */

            /** There is error but not showing to the console because pointless**/
            //fprintf(stderr, "%s\n", tidy_errbuf.bp); /* show errors */
          }
        }
      }
    }
    else
    {
        //fprintf(stderr, "%s\n", curl_errbuf);
        return 0;
    }

    //printf(fuckingData);


    //printf("\nFinScanTidy");

    for(int i =0; i < NbLine ; i++)
    {

        if(strstr(memoryTemp, test[i]) != NULL)
        {
            plagiat++;
        }




    }

    tidyBufFree(&docbuf);
    tidyBufFree(&tidy_errbuf);
    tidyRelease(tdoc);
    return plagiat;

}






/* curl write callback, to fill tidy's input buffer...  */
uint write_cb_dl(char *in, uint size, uint nmemb, TidyBuffer *out)
{
  uint r;
  r = size * nmemb;
  tidyBufAppend(out, in, r);
  return r;
}

/* Traverse the document tree */
void dumpNode_dl(TidyDoc doc, TidyNode tnod, int indent, char *memoryTemp)
{

  TidyNode child;

  for(child = tidyGetChild(tnod); child; child = tidyGetNext(child) ) {

    ctmbstr name = tidyNodeGetName(child);
    if(!name) {
      /* if it doesn't have a name, then it's probably text, cdata, etc... */
      TidyBuffer buf;
      tidyBufInit(&buf);
      tidyNodeGetText(doc, child, &buf);
      //printf("%*.*s\n", indent, indent, buf.bp?(char *)buf.bp:"");

      char *out = buf.bp;
      int len = strlen(out);

      if(buf.bp)
      {

          for(int i =0 ; i < len; i++)
          {
              if(out[i] != 13 && out[i] != 10)
              {
                  //printf("\ncrash? 1 len %d",len);
                  int len = strlen(memoryTemp);
                  if(len<9999)
                  {
                    memoryTemp[len] = out[i];
                    memoryTemp[len+1] = '\0';
                  }
                  //bigData = append_(bigData,out[i]);
                //printf("\ncrash? 10");
              }
          }

      }

      tidyBufFree(&buf);
    }


    dumpNode_dl(doc, child, indent + 4,memoryTemp); /* recursive */
  }
}






