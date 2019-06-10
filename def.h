

/**La console s'adaptera pour diff�rentes valeurs not� ici
Attention, le programme � �t� fait pour ces valeurs donn�es, des changements
dans ces d�fitions peuvent amener � des probl�mes d'affichage. **/
#define WIDTH 150
#define HEIGHT 45

/**Le programme se reserve de limit� le nombre de phrase maximum � scaner � 1000
pour des questions de stabilit�s, et de fluidit�.**/
#define MAX_LINE 1000

/**Le programme limite le nombre de lien � 1000 question de m�moire**/
#define MAX_LINK 1000

/** Question de facilit� et d'habitude, il est plus agr�able de travailler avec des boolean**/
#define true 1
#define false 0
typedef int bool;



/**Structure relative � la console
On remplis la console de caract�re dont on veut pouvoir changer : la couleur, et le type de caract�re
ainsi on d�finit un structure qui sera utilis� dans un tableau a deux dimension pour correspondre
� la taille de la console**/
struct block
{
    int color;
    unsigned char character;
};

/**Cette structure correponds � la fen�tre de validation, qui peut �tre active ou non
elle a �galement, deux choix (oui / non ) qui auront une valeur correspondante dans select
comme information � afficher elle contient �galement, le nom du fichier.**/
struct Validpop
{
    char filePath[120];
    int select;
    bool Active;
};

/**Dans la phase 2, on r�alise des recherches succ�ssive sur google, les r�sultats seront stoquer dans
un tableau de structure Link, chaque url est stoquer une seule fois, et on lui attribut un entier indiquant
le nombre de fois qu'il est apparu**/
struct Link
{
    int occurence;
    char url[1000];
};


/**Cette structure est tr�s importante, elle permet de faire la liason, entre diff�rents threads. En effet,
lors du d�marrage d'un nouveaux threads, un seul argument peut �tre envoy� � la fonction d'arriv�,
donc on creer une structure Data, qui contient toute les informations n�c�ssaire,
les diff�rents processus pourront alors y acceder en simultan�**/
struct Data
{
    char ** array; //array of all sentences to scan.
    bool isProxyNeeded;
    int *MaxL; //Number of sentence to scan (useful to compute progress without strlen on array.
    int totalLine;
    int pos; //Position on the console (max = 5)
    char filename[120];
    bool finish;
    struct Link result[100];
    int nbResult;
};

