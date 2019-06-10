#include <stdio.h>
#include <tidy.h>
#include <string.h>
#include <tidybuffio.h>
#include <curl/curl.h>


//Prototypes de fonctions
uint write_cb(char *in, uint size, uint nmemb, TidyBuffer *out); //== Résultat Curl
void dumpNode(TidyDoc doc, TidyNode tnod, int indent,int pos); //==analyse des noeux XML ( HTML )
void addlink(char *url,int pos);
void initLink(int pos);
char *result(int pos);
void phase_2(void *vargp);
void listLink(int pos);
int estimation(int pos);
