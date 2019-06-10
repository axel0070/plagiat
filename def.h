

/**La console s'adaptera pour différentes valeurs noté ici
Attention, le programme à été fait pour ces valeurs données, des changements
dans ces défitions peuvent amener à des problèmes d'affichage. **/
#define WIDTH 150
#define HEIGHT 45

/**Le programme se reserve de limité le nombre de phrase maximum à scaner à 1000
pour des questions de stabilités, et de fluidité.**/
#define MAX_LINE 1000

/**Le programme limite le nombre de lien à 1000 question de mémoire**/
#define MAX_LINK 1000

/** Question de facilité et d'habitude, il est plus agréable de travailler avec des boolean**/
#define true 1
#define false 0
typedef int bool;



/**Structure relative à la console
On remplis la console de caractère dont on veut pouvoir changer : la couleur, et le type de caractère
ainsi on définit un structure qui sera utilisé dans un tableau a deux dimension pour correspondre
à la taille de la console**/
struct block
{
    int color;
    unsigned char character;
};

/**Cette structure correponds à la fenêtre de validation, qui peut être active ou non
elle a également, deux choix (oui / non ) qui auront une valeur correspondante dans select
comme information à afficher elle contient également, le nom du fichier.**/
struct Validpop
{
    char filePath[120];
    int select;
    bool Active;
};

/**Dans la phase 2, on réalise des recherches succèssive sur google, les résultats seront stoquer dans
un tableau de structure Link, chaque url est stoquer une seule fois, et on lui attribut un entier indiquant
le nombre de fois qu'il est apparu**/
struct Link
{
    int occurence;
    char url[1000];
};


/**Cette structure est très importante, elle permet de faire la liason, entre différents threads. En effet,
lors du démarrage d'un nouveaux threads, un seul argument peut être envoyé à la fonction d'arrivé,
donc on creer une structure Data, qui contient toute les informations nécéssaire,
les différents processus pourront alors y acceder en simultané**/
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

