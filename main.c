#include <stdio.h>
#include <math.h>
#include <string.h>
#include <windows.h> //per utilizzare Sleep();
#include "bmp.h"


// Dimensione immagine in pixel, quadrata
#define DATA_DIM 512
#define GRAYSCALE 256
#define PI 3.14159

#define LOAD 0
#define SAVE 1

//formattazione stringa per lettura nelle funzioni load and save
void str_formatting(char []);
//menu per la selezione delle funzioni
int menu();
//verifica che il percorso indicato sia corretto
int verifica(char [], char [], int); //0 per il caricamento e 1 per il salvataggio
int checkbmp(char []);
//lista errori
void errori(char);
//scelta foto
void sceltafoto(char []);

/***Funzione 1: calcolo occorrenze suddivise in livelli scelti dall'utente e visualizzazione grafica su istogramma***/
int levels(char []);
void hist(unsigned char*, int*, float); //Calcolo dei livelli
void generatehist(unsigned char* , int*, int, int); //Generazione istogramma in un nuovo fil .bmp

/***Funzione 2: Filtro Gaussiano per l'eliminazione del rumore***/
int GaussianBlur(char []);

void G(float*, int, float);
float bi_gauss(float, int, int);
void gaussianfilter(unsigned char*, float*, unsigned char*, int);
//condivise con F3
void falseimage(unsigned char*, unsigned char*, int);
void convoluzione(int*, unsigned char*, float*, int);
int newpixel(unsigned char*, float*, int, int, int);
void normalisation(unsigned char*, int*);
int findmax(int*);
int findmin(int*);

/***Funzione 3: Edge Detection via Sobel Operator***/
int EdgeDetection(char []);
void sobeloperator(unsigned char*, float*, float*, float*, float*, int);
void magnitude(int*, int*, int*, int*, int*);

/***Funzione 4: Image Stretching**/
int imagestretching(char []);
void stretching(unsigned char* img);


int main(){

    char s[100]={0}; //stringa per il perocorso
    char sel;
    printf("ciao");

    system("cls");
    sceltafoto(s);


    do{
        sel=menu();
//      printf("---%d---", sel);
        if(sel==1){
            printf("---%d---", sel);
            if(levels(s))
                printf("\nSpiacente, si è verificato un errore...\n");
        }
        else if(sel==2){
            printf("---%d---", sel);
            if(GaussianBlur(s))
                printf("\nSpiacente, si è verificato un errore...\n");
        }
        else if(sel==3){
            printf("---%d---", sel);
            if(EdgeDetection(s))
                printf("\nSpiacente, si è verificato un errore...\n");
            }
        else if(sel==4){
            printf("---%d---", sel);
            if(imagestretching(s))
                printf("\nSpiacente, si è verificato un errore...\n");
        }
        else if(sel==5){
            getchar();
            system("cls");
            sceltafoto(s);
        }
        else{
            system("cls");
            Sleep(500);
            printf("\n\nGrazie! A presto!\n\n");
        }
    }while(sel!=6);

    return 0;
}

/**FRONTEND**/

void sceltafoto(char s[]){

    char errore; //return delle funzioni per comunicare eventuali errori all'utente

    printf("Ciao, indicami il percorso in cui si trova la foto\nOppure solo %cnomefoto.bmp%c se il file %c nella stessa cartella di questo programma\n", 34, 34, 138);
    do{
    str_formatting(s);
    errore=verifica(s, s, LOAD); //passo 2 volte s in modo che la funzione sia utilizzabile anche per il SAVE
        if(errore!=0)
            errori(errore);
    }while(errore!=0);
}

void destinazione(char s[], char d[]){

    char errore;

    while (getchar()!='\n');  // svuoto il buffer da tastiera
    do{
    str_formatting(d);
    errore=verifica(s, d, SAVE);
        if(errore!=0)
            errori(errore);
    }while(errore!=0);
}

int verifica(char s[], char d[], int m){
    int a;
    BMP_Image img;
    if(m==LOAD){
        a = loadBMP(s, &img);
        if(a==0){
            a = checkbmp(s);
    }
    }
    else{
        a = loadBMP(s, &img);
        a = saveBMP(img, d);
        a = loadBMP(d, &img);
        if(a==0){
            a = checkbmp(d);
        }
    }


    return a;
}

int checkbmp(char s[]){ //se carico un file che non hanno nessun formato ma che è stato salvato tramite saveBMP la funzione LOAD non individua l'errore

    int l;
    l=strlen(s);
    if(s[l-1]=='p' && s[l-2] == 'm' && s[l-3] == 'b')
        return 0;
    else
        return 2;

}

void errori(char e){
    if(e==1){
        printf("\n!!!Hai indicato un percorso errato! Ricorda che il percorso deve terminare con nomefile.bmp!\n");
    }else if(e==2){
        printf("\n!!!Hai indicato un'immagine che non %c in formato .bmp, convertila e riprova!\n",138);
    }else if(e==3){
        printf("\n!!!L'immagine non %c a 8 bit, mi dispiace ma per il momento so leggere solo immagini a 8 bit...\n", 138);
    }else{
        printf("\n!!!Le dimensioni dell'immagine sono errate, ricorda che devono essere %dx%d pixel!\n", DATA_DIM, DATA_DIM);
    }
}

int menu(){

    int s;

    Sleep(300);
    system("cls");
    printf("\nPerfetto! Ora dimmi cosa vuoi fare con la tua immagine:\n\n\n");
    printf("\t1. Calcolo e tracciamento dell'istogramma dell'immagine;\n\n");
    Sleep(350);
    printf("\t2. Applicazione di un filtro Gaussiano con dimensioni e deviazione standard a scelta;\n\n");
    Sleep(350);
    printf("\t3. Edge detection con l'operatore di Sobel;\n\n");
    Sleep(350);
    printf("\t4. Stretching dell'immagine;\n\n");
    Sleep(350);
    printf("\t5. Voglio modificare un'altra immagine;\n\n");
    Sleep(350);
    printf("\t6. Per oggi ho finito! Chiudi l'applicazione!");
    Sleep(350);

    do{
        printf("\n\nIndica la tua scelta <1/2/3/4/5>: ");
        scanf("%d", &s);
    }while(s<1 || s>6);

    return s;
}

void str_formatting(char s[]){

    unsigned char i=0;
    printf("\nPercorso: ");

    do{
		scanf("%c", &s[i]);
		if(s[i]== 92)
            s[i]='/';
		i++;
		s[i]=0;
	}while(s[i-1]!='\n');
    s[i-1]='\0';
}

/**TERMINE FRONTEND**/

/**INIZIO LEVELS**/

int levels(char s[]){

	BMP_Image img;
	int i, occ_max, sum = 0;
	float l;
	int *occ;
	char d[100];

	printf("\n\nPerfetto! Ora indicami il percorso in cui vuoi salvare l'istogramma dell'immagine\nOppure solo <<nomeistogramma.bmp>> se lo vuoi salvare nella stessa cartella dell'applicazione\n\n");
    Sleep(1300);
    printf("Ricorda che nel percorso deve essere incluso il 'nome' dell'istogramma\n(es: C:%cUsers%cNomeUtente%cDesktop%cistogramma_cranio1.bmp)\n\n", 92,92,92,92);
    destinazione(s, d);

	loadBMP(s, &img);

	do{
       printf("\n\nIndica il numero di livelli tra 0 e 256 per il calcolo dell'istogramma dell'immagine: ");
       scanf("%f", &l);
	}while(l<1 || l>256 || l-floor(l)!=0);

    occ=(int *) calloc(l,sizeof(int)); //alloco solo la memoria che mi serve per salvare le occcorrenze di ogni livello

	hist(&img.data[0][0].grey, occ, l);

	occ_max=*occ;
	for(i=0; i<l; i++){
        printf("\n%d%c livello ", i+1, 248);
        if(l!=256)
            printf("(%.0f-%.0f): %d", i*GRAYSCALE/l,i*GRAYSCALE/l+GRAYSCALE/l-1, *(occ+i));
        else
            printf("%d", *(occ+i));

        sum = sum + *(occ+i);
        if(*(occ+i)>occ_max)
            occ_max=*(occ+i);
	}

	if(sum != DATA_DIM*DATA_DIM){
        printf("\nMancano %d pixel\n", DATA_DIM*DATA_DIM-sum);
        return 1;
	}

	saveBMP(img, s);

	loadBMP(s, &img); //carico la stessa immagine in ingresso in modo da essere sicuro che non vi siano problemi di caricamento
	generatehist(&img.data[0][0].grey, occ, occ_max, l);
	free(occ);
	saveBMP(img, d);

	printf("\n\nTrovi l'immagine rappresentante l'istogramma nel percorso che mi hai indicato prima!\n\n");
	ShellExecute(NULL, NULL, d, NULL, NULL, SW_SHOWNORMAL);

	system("pause");

    return 0;
}

void hist(unsigned char* img, int* o, float l){
    float amp;
    int j;
    float x;
    int p;

    amp=(GRAYSCALE)/l; //salvo in amp l'ampiezza di ogni livello

    /**
        Tutti questi accorgimenti come ceil(), roof() e quindi l'utilizzo di variabili di tipo float invece che int è dovuto al voler migliorare
        la precisione nella determinazione del livello. Inizialmente le divisioni erano tra interi ma provocavano alcuni problemi quando l != 2^x.
        Infatti risultava che tutti i pixel con .grey >= di un certo valore andavano "persi" (es: l=162, amp = 1.57... approssimata a 1--> img...grey
        oltre un certo valore vanno persi (es img.data.grey = 196, 196/1 = 196 ma il livello 196 non è stato allocato...) e quindi la somma delle
        occorrenze non risulta più valida... Utilizzando invece i float e la accortezza che roof(grey/amp) non fosse maggiore o uguale al numero
        dei livelli si è sia migliorata la precisione, sia evitato la "perdita" di pixel...
        La miglior precisione la si può verificare anche confrontando i risultati con l=256 e l=x... (più immediato con le immagini con
        pochi pixel chiari o pochi scuri)
    **/

    for(j=0;j<DATA_DIM*DATA_DIM;j++){
        x=*(img+j)/amp; //la divisione ritorna quante volte l'"ampiezza" sta nel valore del pixel, determinando così il livello di appartenenza
        if(ceil(x)-x >= 0.5 || ceil(x) == l)
            p = floor(x);
        else
            p = ceil(x);

        *(o+p) += 1;
    }
}


void generatehist(unsigned char* histo, int* o, int o_max, int l){
    int i,j,k,amp,start;
    start = 0;
    amp = DATA_DIM/l; //qui lascio DATA_DIM per occupare tutto lo spazio dell'immagine .bmp

    for(i=0; i<DATA_DIM*DATA_DIM; i++){
        *(histo+i)=35; //sfondo nero-grigio
	}
	//NOTA:le "i" vanno dal basso verso l'alto, le j da sx verso dx
    for(k=0; k<l; k++){
        for(j=start; j<start+amp; j++){
            for(i=0; i<((*(o+k)*(DATA_DIM-1))/o_max); i++){
                *(histo+i*DATA_DIM+j)=255;
            }
        }
        start+=amp;
    }
}

/**FINE LEVELS **/

/**INIZIO GAUSSIAN BLUR**/

int GaussianBlur(char s[]){

    int dim, i, j;
    float sigma;
    float* g;
    BMP_Image img;
    unsigned char* f_img;
    char d[100], r;

	printf("\n\nPerfetto! Ora indicami il percorso in cui vuoi salvare l'immagine modificata\nOppure solo <<nomeimmagine.bmp>> se la vuoi salvare nella stessa cartella dell'applicazione\n\n");
    Sleep(1300);
    printf("Ricorda che nel percorso deve essere incluso il 'nome' dell'immagine\n(es: C:%cUsers%cNomeUtente%cDesktop%cGaussianBlurCranio1.bmp)\n\n", 92,92,92,92);
    destinazione(s, d);

    do{
        printf("Indica la dimensione (dispari) del kernel per la generazione del filtro Gausssiano: ");
        scanf("%d", &dim);
    }while(dim%2==0 || dim < 2);

    do{
        printf("Indica il valore della deviazione standard (std>0): ");
        scanf("%f", &sigma);
    }while(sigma <= 0);

    printf("Perfetto, genero un kernel %d%c%d con deviazione standard di %.5f\n\n", dim, 158, dim, sigma);

    g = (float*) calloc(dim*dim,sizeof(float));
    G(g, dim, sigma);

    printf("\n\n\n");
    for(i=0; i<dim; i++){
        for(j=0; j<dim; j++){
            printf("%.5f\t", *(g+i*dim+j));
        }
        printf("\n");
    }

    loadBMP(s, &img);

    f_img = (unsigned char*) calloc (pow(DATA_DIM+dim-1, 2), sizeof(unsigned char));
    gaussianfilter(&img.data[0][0].grey, g, f_img, dim);
    free(f_img);
    free(g);

    saveBMP(img, d);

    printf("\n\nTrovi l'immagine modificata nel percorso che mi hai indicato prima!\n\n");


    ShellExecute(NULL, NULL, d, NULL, NULL, SW_SHOWNORMAL);

    printf("\nVuoi utilizzare l'immagine modificata per le prossime modifiche? <y/n>: ");
    do{
        scanf("\n%c", &r);
    }while(r!='y' && r!='n');
    if(r=='y'){
        for(i=0; i<=strlen(d); i++)
            s[i] = d[i];
    }

    printf("\n\n");

    system("pause");

    return 0;
}

void G(float* g, int dim, float sigma){
    int i,j;
    int k;

    k=(dim-1)/2; //dato che il centro deve avere valori (0,0) calcolando k posso muovermi da un estremo all'altro ella matrice;
    for(i=-k; i<k+1; i++){
        for(j=-k; j<k+1; j++){
            *(g+((i+k)*dim)+(j+k))=bi_gauss(sigma, i, j);
        }
    }
}

float bi_gauss(float sigma, int x, int y){ //è simmetrica, x e y interscambiabili...

    float g;
    //printf("\n\nsigma = %.2f, x:%d, y:%d", sigma, x, y);

    g=(1/(2*PI*pow(sigma, 2)));
    //printf("\ng1: %.2f", g);
    g=g*exp((-x*x-y*y)/(2*sigma*sigma)); //completa
    //printf("\ng2: %.2f", g);

    return g;
}

void gaussianfilter(unsigned char* img, float* g, unsigned char* f_img, int dim){
    int* raw_image;

    falseimage(img, f_img, dim); //genero l'immagine fittizia
    raw_image = (int*) calloc (pow(DATA_DIM, 2), sizeof(int));
    convoluzione(raw_image, f_img, g, dim); //creo un immagine che avrà valori che possono andare oltre 255 (integrale di Gauss = sqrt(PI_Greco)
    normalisation(img, raw_image); //normalizzo l'immagine con valori compresi tra 0 e 255;
    free(raw_image);
}

void normalisation(unsigned char* img, int* G){
    int max, min, i, d;

    max=findmax(G);
    min=findmin(G);

    d=min-0;
    for(i=0; i<DATA_DIM*DATA_DIM; i++){
        *(G+i)-=d;
        *(img+i)=(255*(*(G+i)))/max;
    }

}

int findmax(int* G){
    int max=*G;
    int i;

    for(i=0; i<(DATA_DIM)*(DATA_DIM); i++){
        if(*(G+i)>max){
            max=*(G+i);
        }
    }
    return max;
}

int findmin(int* G){
    int min=*G;
    int i;

    for(i=0; i<(DATA_DIM)*(DATA_DIM); i++){
        if(*(G+i)<min){
            min=*(G+i);
        }
    }
    return min;
}

void convoluzione(int* img, unsigned char* f_img, float *g, int dim){

    int i,j;

    for(i=0; i<DATA_DIM; i++){
        for(j=0; j<DATA_DIM; j++){
            *(img+i*DATA_DIM+j)=newpixel(f_img, g, i, j, dim);
        }
    }
}

int newpixel(unsigned char* f_img, float *g, int x, int y, int dim){

    float new_pixel = 0;
    int i, j;

    for(i=0; i<dim; i++){
        for(j=0; j<dim; j++){
            new_pixel = new_pixel + (*(g+i*dim+j))*(*(f_img+(x+i)*(DATA_DIM+dim-1)+(y+j)));
        }
    }
    if(ceil(new_pixel)-new_pixel >= 0.5)
        return floor(new_pixel);
    else
        return ceil(new_pixel);

}

void falseimage(unsigned char* img, unsigned char* f_img, int k){//genero un'immagine 258x258 al fine di aggirare il problema ai bordi...
    int i,j;

    //copia immagine reale al centro dell'immagine fittizia(che ha dim (256+k-1)x(256+k-1));
    for(i=0; i<DATA_DIM; i++){
        for(j=0; j<DATA_DIM; j++){
            *(f_img+(i+k/2)*(DATA_DIM+k-1)+j+k/2)=*(img+i*DATA_DIM+j);
        }
    }
    for(j=0; j<k/2; j++){//devo copiare dim/2 volte i bordi
        for(i=k/2; i<k/2+DATA_DIM; i++){
            *(f_img+((k/2)-1-j)*(DATA_DIM+k-1)+i)=*(img+i-k/2);//bordo superiore
            *(f_img+((k/2)+DATA_DIM+j)*(DATA_DIM+k-1)+i)=*(img+((DATA_DIM-1)*DATA_DIM)+i-k/2);//bordo inferiore
            *(f_img+i*(DATA_DIM+k-1)+DATA_DIM+k/2+j)=*(img+(i-k/2)*DATA_DIM+DATA_DIM-1);//bordo dx
            *(f_img+i*(DATA_DIM+k-1)+k/2-1-j)=*(img+(i-k/2)*DATA_DIM);//bordo sx

        }
    }
    for(i=0; i<k/2; i++){
        for(j=0; j<k/2; j++){
            *(f_img+i*(DATA_DIM+k-1)+j)=*img; //angolo sup sx
            *(f_img+i*(DATA_DIM+k-1)+(DATA_DIM+k/2+j))=*(img+DATA_DIM-1); //angolo sup dx
            *(f_img+(i+k/2+DATA_DIM)*(DATA_DIM+k-1)+j)=*(img+(DATA_DIM-1)*DATA_DIM); //angolo inf sx
            *(f_img+(i+k/2+DATA_DIM)*(DATA_DIM+k-1)+(DATA_DIM+k/2+j))=*(img+(DATA_DIM-1)*DATA_DIM+DATA_DIM-1); //angolo inf dx
        }
    }
}

/**FINE GAUSSIAN BLUR**/

/**INIZIO EDGE DETECTION**/ //nota: le funzioni comuni sono nella parte "gaussianblur"

int EdgeDetection(char s[]){

	BMP_Image img;
	char d[100], r;
	int i;

    float Sx[5][5]={{2, 3, 0, -3, -2},
                    {3, 4, 0, -4, -3},
                    {6, 12, 0, -12, -6},
                    {3, 4, 0, -4, -3},
                    {2, 3, 0, -3, -2}};

    float Sy[5][5]={{2, 3, 6, 3, 2},
                    {3, 4, 12, 4, 3},
                    {0, 0, 0, 0, 0},
                   {-3,-4,-12,-4,-3},
                   {-2,-3,-6,-3,-2}};

    float S_45[5][5]={{0, -2, -3, -2, -6},
                      {2,  0, -4, -12, -2},
                      {3,  4,  0, -4, -3},
                      {2,  12,  4,  0, -2},
                      {6,  2,  3,  2,  0}};

    float S_135[5][5]={{-6, -2, -3, -2, 0},
                       {-2, -12, -4,  0, 2},
                       {-3, -4,  0,  4, 2},
                       {-2,  0,  4,  12, 2},
                        {0,  2,  3,  2, 6}};

    printf("\n\nPerfetto! Ora indicami il percorso in cui vuoi salvare l'immagine modificata\nOppure solo <<nomeimmagine.bmp>> se la vuoi salvare nella stessa cartella dell'applicazione\n\n");
    Sleep(1300);
    printf("Ricorda che nel percorso deve essere incluso il 'nome' dell'immagine\n(es: C:%cUsers%cNomeUtente%cDesktop%cEdgeDetectionCranio1.bmp)\n\n", 92,92,92,92);
    destinazione(s, d);

	loadBMP(s, &img);

    sobeloperator(&img.data[0][0].grey, &Sx[0][0], &Sy[0][0], &S_45[0][0], &S_135[0][0], 5);

	saveBMP(img, d);

	printf("\n\nTrovi l'immagine modificata nel percorso che mi hai indicato prima!\n\n");
	ShellExecute(NULL, NULL, d, NULL, NULL, SW_SHOWNORMAL);

	printf("\nVuoi utilizzare l'immagine modificata per le prossime modifiche? <y/n>: ");
    do{
        scanf("\n%c", &r);
    }while(r!='y' && r!='n');
    if(r=='y'){
        for(i=0; i<=strlen(d); i++)
            s[i] = d[i];
    }

    printf("\n\n");
	system("pause");
    return 0;
}

void sobeloperator(unsigned char* img, float* Sx, float* Sy, float* S_45, float* S_135, int DIM){

    //genero le "immagini" con cui individuerò i bordi verticali e orizzontali separatamente
    int* Gx;
    int* Gy;
    int* G_45;
    int* G_135;
    int* G;
    unsigned char* f_img;

    f_img = (unsigned char*) calloc (pow(DATA_DIM+DIM-1, 2), sizeof(unsigned char));
	falseimage(img, f_img, DIM); //genero immaggine fittizia per la convoluzione

	Gx = (int*) calloc (pow(DATA_DIM, 2), sizeof(int)); //è un int poicè ci saranno anche valori negativi inferiori a -128
	Gy = (int*) calloc (pow(DATA_DIM, 2), sizeof(int));
	G_45 = (int*) calloc (pow(DATA_DIM, 2), sizeof(int)); //è un int poicè ci saranno anche valori negativi inferiori a -128
	G_135 = (int*) calloc (pow(DATA_DIM, 2), sizeof(int));

	convoluzione(Gx, f_img, Sx, DIM);
	convoluzione(Gy, f_img, Sy, DIM);
	convoluzione(G_45, f_img, S_45, DIM);
	convoluzione(G_135, f_img, S_135, DIM);
	free(f_img);

	G = (int*) calloc (pow(DATA_DIM, 2), sizeof(int));

	magnitude(Gx, Gy, G_45, G_135, G);
	free(Gx);
	free(Gy);
	free(G_45);
    free(G_135);

	normalisation(img, G);
    free(G);
}

void magnitude(int* Gx, int* Gy, int* G_45, int* G_135, int* G){
    int i;
    for(i=0; i<DATA_DIM*DATA_DIM; i++){
        *(G+i)=sqrt(pow(*(Gx+i), 2)+pow(*(Gy+i), 2)+pow(*(G_45+i), 2)+pow(*(G_135+i), 2));
    }
}

/**FINE EDGE DETECTION**/


int imagestretching(char s[]){

    BMP_Image img;
    char d[100], r;
    int i;

	printf("\n\nPerfetto! Ora indicami il percorso in cui vuoi salvare l'immagine modificata\nOppure solo <<nomeimmagine.bmp>> se la vuoi salvare nella stessa cartella dell'applicazione\n\n");
    Sleep(1300);
    printf("Ricorda che nel percorso deve essere incluso il 'nome' dell'immagine\n(es: C:%cUsers%cNomeUtente%cDesktop%cStretchedBrain.bmp)\n\n", 92,92,92,92);
    destinazione(s, d);

	loadBMP(s, &img);
    stretching(&img.data[0][0].grey);
	saveBMP(img, d);

	printf("\n\nTrovi l'immagine modificata nel percorso che mi hai indicato prima!\n\n");
	ShellExecute(NULL, NULL, d, NULL, NULL, SW_SHOWNORMAL);

    printf("\nVuoi utilizzare l'immagine modificata per le prossime modifiche? <y/n>: ");
    do{
        scanf("\n%c", &r);
    }while(r!='y' && r!='n');
    if(r=='y'){
        for(i=0; i<=strlen(d); i++)
            s[i] = d[i];
    }

    printf("\n\n");
	system("pause");
    return 0;
}

void stretching(unsigned char* img){

    int i, s;
    int* occ;
    int c,d;
    float p_inf, p_sup;
    int count = 0;
    char done=0;

    printf("\n\nPreferisci procedere indicando i limiti inferiori e superiori tramite:\n\t1.Valore pixel in scala di grigio(0-255);\n\t2.Percentili");
    Sleep(500);
    do{
        printf("\n\nIndica la tua scelta: ");
        scanf("%d", &s);
    }while(s<1 || s>2);

    if(s==2){
        printf("\n\nUna scelta comune dei percentili sono il 5%c e il 95%c.", 37, 37);

        do{
            printf("\n\nIndicami il percentile inferiore: ");
            scanf("%f", &p_inf);
            printf("\n\nIndicami il percentile superiore: ");
            scanf("%f", &p_sup);
        }while(p_inf>p_sup || p_inf<0 || p_inf>99.99 || p_sup<0 || p_sup >99.99);

        p_inf/=100;
        p_sup/=100;
        occ=(int *) calloc(GRAYSCALE,sizeof(int)); //alloco solo la memoria che mi serve per salvare le occcorrenze di ogni livello
        hist(img, occ, GRAYSCALE);

        for(i=0; i<GRAYSCALE; i++){
            count += *(occ+i);
            if(count>=DATA_DIM*DATA_DIM * p_inf && done == 0){
                c = i;
                done = 1;
            }
            if(count>=DATA_DIM*DATA_DIM * p_sup){
                d = i;
                break;
            }
        }
        free(occ);

    }else{
         do{
            printf("\n\nIndicami il valore inferiore: ");
            scanf("%d", &c);
            printf("\n\nIndicami il valore superiore: ");
            scanf("%d", &d);
        }while(c>d || c<0 || c>255 || d<0 || d>255);
    }

	for(i=0; i<DATA_DIM*DATA_DIM; i++)
        *(img+i)=((*(img+i)-c)*((255-0)/(d-c)))+0; //255 e 0 sono il lower and upper limit della scala di grigi
}
