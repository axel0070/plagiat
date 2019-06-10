#include "def.h"

//Fonctions
void screen();
void resetScreen();
void print(char *text,int x,int y, int color);
void initBorders();
void BordersTop();

void *functionThread1(int val);
void *functionThread2(int val);
void resetLeft();
void resetBottomRight();
void popup(char * msg,int select);

//for thread 3
void progressBar(int pos,int progress, char *filename);
void selectThreadPos(int pos);

void printResult(struct Data output);

