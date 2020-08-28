#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include "bmpv2.h"

#define GRAYSCALE 256
#define PI 3.14159
#define LOAD 0
#define SAVE 1

/**FRONTEND**/
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
void destinazione(char [], char []);

/***Funzione 1: calcolo occorrenze suddivise in livelli scelti dall'utente e visualizzazione grafica su istogramma***/
int levels(char []);
void hist(unsigned char*, int*, float, int); //Calcolo dei livelli
void generatehist(BMP_Image, int*, int, int, char []); //Generazione istogramma in un nuovo fil .bmp

/***Funzione 2: Filtro Gaussiano per l'eliminazione del rumore***/
int GaussianBlur(char []);

void G(float*, int, float);
float bi_gauss(float, int, int);
void gaussianfilter(unsigned char*, float*, unsigned char*, int, int, int);
//condivise con F3
void falseimage(unsigned char*, unsigned char*, int, int, int);
void convoluzione(int*, unsigned char*, float*, int, int, int);
int newpixel(unsigned char*, float*, int, int, int, int);
void normalisation(unsigned char*, int*, int, int);
int findmax(int*, int, int);
int findmin(int*, int, int);

/***Funzione 3: Edge Detection via Sobel Operator***/
int EdgeDetection(char []);
void sobeloperator(unsigned char*, float*, float*, float*, float*, int, int, int);
void magnitude(int*, int*, int*, int*, int*, int, int);

/***Funzione 4: Image Stretching**/
int imagestretching(char []);
void stretching(unsigned char*, int, int);
void automaticdetection(int*, int*, int*, int, int);

int main(){

	char s[100]={0}; //stringa per il perocorso
    char sel;

    system("cls");
    sceltafoto(s);

    do{
        sel=menu();
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
    unsigned char *image;
    if(m==LOAD){
        a = loadBMP(s, &img);
        if(a==0){
            a = checkbmp(s);
    }
    }
    else{
        a = loadBMP(s, &img);
        image = (unsigned char*) malloc(img.info.imagesize*sizeof(unsigned char));
        a= loadIMG(s, image, img.info.imagesize);
        a = saveBMP(img, d, image);
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
        printf("\n!!!Hai indicato un percorso errato o inesistente! Ricorda che il percorso deve terminare con nomefile.bmp\n");
    }else if(e==2){
        printf("\n!!!Hai indicato un'immagine che non %c in formato .bmp, convertila e riprova!\n",138);
    }else{
        printf("\n!!!L'immagine non %c a 8 bit, mi dispiace ma per il momento so leggere solo immagini a 8 bit...\n", 138);
    }
}

int menu(){

    int s;

    Sleep(200);
    system("cls");
    printf("\nPerfetto! Ora dimmi cosa vuoi fare con la tua immagine:\n\n\n");
    printf("\t1. Calcolo e tracciamento dell'istogramma dell'immagine;\n\n");
    Sleep(250);
    printf("\t2. Applicazione di un filtro Gaussiano con dimensioni e deviazione standard a scelta;\n\n");
    Sleep(250);
    printf("\t3. Edge detection con l'operatore di Sobel;\n\n");
    Sleep(250);
    printf("\t4. Stretching dell'immagine;\n\n");
    Sleep(250);
    printf("\t5. Voglio modificare un'altra immagine;\n\n");
    Sleep(250);
    printf("\t6. Voglio visualizzare l'immagine corrente;\n\n");
    Sleep(250);
    printf("\t7. Per oggi ho finito! Chiudi l'applicazione!");
    Sleep(250);

    do{
        printf("\n\nIndica la tua scelta <1/2/3/4/5/6/7>: ");
        scanf("%d", &s);
    }while(s<1 || s>7);

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
	unsigned char* image;

	int i, occ_max, sum = 0;
	float l;
	int *occ;
	char d[100];

	printf("\n\nPerfetto! Ora indicami il percorso in cui vuoi salvare l'istogramma dell'immagine\nOppure solo <<nomeistogramma.bmp>> se lo vuoi salvare nella stessa cartella dell'applicazione\n\n");
    Sleep(1300);
    printf("Ricorda che nel percorso deve essere incluso il 'nome' dell'istogramma\n(es: C:%cUsers%cNomeUtente%cDesktop%cistogramma_cranio1.bmp)\n\n", 92,92,92,92);
    destinazione(s, d);

	loadBMP(s, &img);
	image = (unsigned char*) malloc(img.info.imagesize*sizeof(unsigned char));
    loadIMG(s, image, img.info.imagesize); //i pixel sono in image

	do{
       printf("\n\nIndica il numero di livelli tra 0 e 256 per il calcolo dell'istogramma dell'immagine: ");
       scanf("%f", &l);
	}while(l<1 || l>256 || l-floor(l)!=0);

    occ=(int *) calloc(l,sizeof(int)); //alloco solo la memoria che mi serve per salvare le occcorrenze di ogni livello
	hist(image, occ, l, img.info.imagesize);

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

	if(sum != img.info.height*img.info.width){
        printf("\nMancano %d pixel\n", img.info.height*img.info.width-sum);
        return 1;
	}

    generatehist(img, occ, occ_max, l, d);
	free(occ);

	printf("\n\nTrovi l'immagine rappresentante l'istogramma nel percorso che mi hai indicato prima!\n\n");
	ShellExecute(NULL, NULL, d, NULL, NULL, SW_SHOWNORMAL);

	system("pause");

    return 0;
}

void hist(unsigned char* img, int* o, float l, int size){

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

    for(j=0;j<size;j++){
        x=*(img+j)/amp; //la divisione ritorna quante volte l'"ampiezza" sta nel valore del pixel, determinando così il livello di appartenenza
        if(ceil(x)-x >= 0.5 || ceil(x) == l)
            p = floor(x);
        else
            p = ceil(x);

        *(o+p) += 1;
    }
}

void generatehist(BMP_Image img, int* o, int o_max, int l, char d[]){

    BMP_Image histo;

    histo = img;
    histo.header.size = 656438;
    histo.info.width = 1280;
    histo.info.height = 512;
    histo.info.imagesize = 655360;

    unsigned char* histogram;
    histogram = (unsigned char*) malloc(655360*sizeof(unsigned char));

    int i,j,k,amp,start;
    start = 0;
    amp = histo.info.width/l; //qui lascio DATA_DIM per occupare tutto lo spazio dell'immagine .bmp

    for(i=0; i<histo.info.imagesize; i++){
        *(histogram+i)=10; //sfondo grigio-scuro
	}
	//NOTA:le "i" vanno dal basso verso l'alto, le j da sx verso dx
    for(k=0; k<l; k++){
        for(j=start; j<start+amp; j++){
            for(i=0; i<((*(o+k)*(histo.info.height-1-20))/o_max); i++){
                *(histogram+(i+20)*histo.info.width+j)=200;
            }
        }
        start+=amp;
    }
    //tacchette 0,25,50,100...250
    for(j=0; j<GRAYSCALE; j++){
        if(j%25==0){
            if(j%50==0){
                if(j%100==0){
                    for(i=0; i<17; i++)
                        *(histogram+i*histo.info.width+j*(histo.info.width/GRAYSCALE))=255;
                }
                for(i=0; i<9; i++)
                    *(histogram+i*histo.info.width+j*(histo.info.width/GRAYSCALE))=255;
            }
            for(i=0; i<3; i++)
                *(histogram+i*histo.info.width+j*(histo.info.width/GRAYSCALE))=255;
        }
    }

    saveBMP(histo, d, histogram);
}

/**FINE LEVELS **/

/**INIZIO GAUSSIAN BLUR**/

int GaussianBlur(char s[]){

    int dim, i, j;
    float sigma;
    float* g;
    BMP_Image img;
    unsigned char* f_img;
    unsigned char* image;
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
    image = (unsigned char*) malloc(img.info.imagesize*sizeof(unsigned char));
    loadIMG(s, image, img.info.imagesize); //i pixel sono in image

    f_img = (unsigned char*) malloc((img.info.height+dim-1)*(img.info.width+dim-1)* sizeof(unsigned char));
    gaussianfilter(image, g, f_img, dim, img.info.height, img.info.width);
    free(f_img);
    free(g);

    saveBMP(img, d, image);

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

    g=(1/(2*PI*pow(sigma, 2)));
    g=g*exp((-x*x-y*y)/(2*sigma*sigma)); //completa

    return g;
}

void gaussianfilter(unsigned char* img, float* g, unsigned char* f_img, int dim, int h, int w){
    int* raw_image;

    falseimage(img, f_img, dim, h, w); //genero l'immagine fittizia
    raw_image = (int*) calloc (h*w, sizeof(int));
    convoluzione(raw_image, f_img, g, dim, h, w); //creo un immagine che avrà valori che possono andare oltre 255 (integrale di Gauss = sqrt(PI_Greco)
    normalisation(img, raw_image, h, w); //normalizzo l'immagine con valori compresi tra 0 e 255;
    free(raw_image);
}

void normalisation(unsigned char* img, int* G, int h, int w){
    int max, min, i, d;

    max=findmax(G, h, w);
    min=findmin(G, h, w);

    d=min-0;
    for(i=0; i<h*w; i++){
        *(G+i)-=d;
        *(img+i)=(255*(*(G+i)))/max;
    }

}

int findmax(int* G, int h, int w){
    int max=*G;
    int i;

    for(i=0; i<h*w; i++){
        if(*(G+i)>max){
            max=*(G+i);
        }
    }
    return max;
}

int findmin(int* G, int h, int w){
    int min=*G;
    int i;

    for(i=0; i<h*w; i++){
        if(*(G+i)<min){
            min=*(G+i);
        }
    }
    return min;
}

void convoluzione(int* img, unsigned char* f_img, float *g, int dim, int h, int w){

    int i,j;

    for(i=0; i<h; i++){
        for(j=0; j<w; j++){
            *(img+i*w+j)=newpixel(f_img, g, i, j, dim, w);
        }
    }
}

int newpixel(unsigned char* f_img, float *g, int x, int y, int dim, int w){

    float new_pixel = 0;
    int i, j;

    for(i=0; i<dim; i++){
        for(j=0; j<dim; j++){
            new_pixel = new_pixel + (*(g+i*dim+j))*(*(f_img+(x+i)*(w+dim-1)+(y+j)));
        }
    }
    if(ceil(new_pixel)-new_pixel >= 0.5)
        return floor(new_pixel);
    else
        return ceil(new_pixel);

}

void falseimage(unsigned char* img, unsigned char* f_img, int k, int h, int w){

    int i,j;

    for(i=0; i<h; i++){
        for(j=0; j<w; j++){
            *(f_img+(i+k/2)*(w+k-1)+j+k/2)=*(img+i*w+j);
        }
    }

    for(j=0; j<k/2; j++){
        for(i=k/2; i<k/2+h; i++){
            *(f_img+i*(w+k-1)+k/2-1-j)=*(img+(i-k/2)*w); //bordo sx
            *(f_img+i*(w+k-1)+k/2+w+j)=*(img+(i-k/2)*w+w-1); //bordo dx
        }
    }

    for(i=0; i<k/2; i++){
        for(j=k/2; j<k/2+w; j++){
            *(f_img+i*(w+k-1)+j)=*(img+j-k/2); //bordo superiore
            *(f_img+(i+h+k/2)*(w+k-1)+j)=*(img+(h-1)*w+j-k/2); //bordo inferiore
        }
    }

    for(i=0; i<k/2; i++){
        for(j=0; j<k/2; j++){
            *(f_img+i*(w+k-1)+j)=*(img); //angolo sup sx
            *(f_img+i*(w+k-1)+j+w+k/2)=*(img+w-1); //angolo sup dx
            *(f_img+(i+k/2+h)*(w+k-1)+j)=*(img+(h-1)*w); //angolo inf sx
            *(f_img+(i+k/2+h)*(w+k-1)+j+w+k/2)=*(img+(h-1)*w+w-1); //angolo inf dx
        }
    }

}


/**FINE GAUSSIAN BLUR**/

/**INIZIO EDGE DETECTION**/ //nota: le funzioni comuni sono nella parte "gaussianblur"

int EdgeDetection(char s[]){

	BMP_Image img;
	unsigned char* image;
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
    image = (unsigned char*) malloc(img.info.imagesize*sizeof(unsigned char));
    loadIMG(s, image, img.info.imagesize); //i pixel sono in image

    sobeloperator(image, &Sx[0][0], &Sy[0][0], &S_45[0][0], &S_135[0][0], 5, img.info.height, img.info.width);

	saveBMP(img, d, image);

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

void sobeloperator(unsigned char* img, float* Sx, float* Sy, float* S_45, float* S_135, int DIM, int h, int w){

    //genero le "immagini" con cui individuerò i bordi verticali, orizzontali e obliqui separatamente
    int* Gx;
    int* Gy;
    int* G_45;
    int* G_135;
    int* G;
    unsigned char* f_img;

    f_img = (unsigned char*) malloc((h+DIM-1)*(w+DIM-1)* sizeof(unsigned char));
	falseimage(img, f_img, DIM, h, w); //genero l'immagine fittizia

	Gx = (int*) malloc (h*w * sizeof(int)); //è un int poicè ci saranno anche valori negativi inferiori a -128
	Gy = (int*) malloc (h*w * sizeof(int));
	G_45 = (int*) malloc (h*w * sizeof(int)); //è un int poicè ci saranno anche valori negativi inferiori a -128
	G_135 = (int*) malloc (h*w * sizeof(int));

	convoluzione(Gx, f_img, Sx, DIM, h, w);
	convoluzione(Gy, f_img, Sy, DIM, h, w);
	convoluzione(G_45, f_img, S_45, DIM, h, w);
	convoluzione(G_135, f_img, S_135, DIM, h, w);
	free(f_img);

	G = (int*) malloc (h*w * sizeof(int));

	magnitude(Gx, Gy, G_45, G_135, G, h, w);
	free(Gx);
	free(Gy);
	free(G_45);
    free(G_135);

	normalisation(img, G, h, w);
    free(G);
}

void magnitude(int* Gx, int* Gy, int* G_45, int* G_135, int* G, int h, int w){
    int i;
    for(i=0; i<h*w; i++){
        *(G+i)=sqrt(pow(*(Gx+i), 2)+pow(*(Gy+i), 2)+pow(*(G_45+i), 2)+pow(*(G_135+i), 2));
    }
}

/**FINE EDGE DETECTION**/

/**INIZIO IMAGE STRETCHING**/

int imagestretching(char s[]){

    BMP_Image img;
    unsigned char* image;
    char d[100], r;
    int i;

	printf("\n\nPerfetto! Ora indicami il percorso in cui vuoi salvare l'immagine modificata\nOppure solo <<nomeimmagine.bmp>> se la vuoi salvare nella stessa cartella dell'applicazione\n\n");
    Sleep(1300);
    printf("Ricorda che nel percorso deve essere incluso il 'nome' dell'immagine\n(es: C:%cUsers%cNomeUtente%cDesktop%cStretchedBrain.bmp)\n\n", 92,92,92,92);
    destinazione(s, d);

	loadBMP(s, &img);
    image = (unsigned char*) malloc(img.info.imagesize*sizeof(unsigned char));
    loadIMG(s, image, img.info.imagesize); //i pixel sono in image

    stretching(image, img.info.height, img.info.width);
	saveBMP(img, d, image);

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

void stretching(unsigned char* img, int h, int w){

    int i, s;
    int* occ;
    int c,d;
    float p_inf, p_sup, x;
    int count = 0;
    char done=0;
    int p;

    printf("\n\nPreferisci procedere indicando i limiti inferiori e superiori tramite:\n\t1.Valori livello scala di grigio(0-255);\n\t2.Percentili\n\t3.Detenzione automatica");
    Sleep(500);
    do{
        printf("\n\nIndica la tua scelta: ");
        scanf("%d", &s);
    }while(s<1 || s>3);

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
        hist(img, occ, GRAYSCALE, h*w);

        for(i=0; i<GRAYSCALE; i++){
            count += *(occ+i);
            if(count>=h*w * p_inf && done == 0){
                c = i;
                done = 1;
            }
            if(count>=h*w * p_sup){
                d = i;
                break;
            }
        }
        free(occ);
    }else if(s==1){
         do{
            printf("\n\nIndicami il valore inferiore: ");
            scanf("%d", &c);
            printf("\n\nIndicami il valore superiore: ");
            scanf("%d", &d);
        }while(c>d || c<0 || c>255 || d<0 || d>255);
    }else{
        occ=(int *) calloc(GRAYSCALE,sizeof(int)); //alloco solo la memoria che mi serve per salvare le occcorrenze di ogni livello
        hist(img, occ, GRAYSCALE, h*w);
        automaticdetection(occ, &c, &d, h, w);
        free(occ);
    }

	for(i=0; i<h*w; i++){
	    x = ((*(img+i)-c)*((255.0-0.0)/(d-c)))+0.0; //255 e 0 sono il lower and upper limit della scala di grigi
	    if(x<0)                                     // i valori sotto 0 vanno settati a 0, quelli sopra 255 vanno settati a 255
            x=0;
        if(x>255)
            x=255;
        if(ceil(x)-x >= 0.5 || ceil(x) == GRAYSCALE-1)
            p = floor(x);
        else
            p = ceil(x);

        *(img+i)=p;
	}
}

void automaticdetection(int* occ, int* c, int* d, int h, int w){
    int noise = ((w*h)/GRAYSCALE)/5; //empirico
    char r;
    int j;

    printf("\n\n\n\n\nDesideri scegliere il numero minimo di pixel appartenenti ad un livello affinch%c il livello non sia considerato rumore? ", 138);
    Sleep(150);
    printf("\nRispondi no (n) per utilizzare i valori di default");
    do{
        printf("\n\n<y/n>: ");
        scanf("\n%c", &r);
    }while(r!='y' && r!='n');

    if(r=='y'){
        printf("\n\nQuale vuoi sia il numero minimo di pixel di un livello affinch%c non sia considerato rumore?", 138);
        do{
            printf("\n\nValore (0-%d): ", (h*w)/GRAYSCALE);
            scanf("\n%d", &noise);
        }while(noise<0 || noise > (h*w)/GRAYSCALE);
    }

    for(j=0; j<GRAYSCALE; j++){
            if(*(occ+j)>=noise){
                *c=j;
                break;
            }
    }
    for(j=GRAYSCALE-1; j>=0; j--){
            if(*(occ+j)>=noise){
                *d=j;
                break;
            }
    }
}

/**FINE IMAGE STRETCHING**/
