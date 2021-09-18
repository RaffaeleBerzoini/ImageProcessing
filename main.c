#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include "bmpv2.h"

#define GRAYSCALE 256
#define PI 3.14159
#define LOAD 0
#define SAVE 1

/**FRONTEND**/
//instruction file generation
void writeInstruction();
//strings formatting for path reading in load and save functions
void str_formatting(char []);
//menu for function selection
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
void threshold(unsigned char*, int);

/***Funzione 4: Image Stretching**/
int imagestretching(char []);
void stretching(unsigned char*, int, int);
void automaticdetection(int*, int*, int*, int, int);

/***Funzione 5: Clustering**/
int segmentation(char []);
void clustering(unsigned char*, int, int);
void newclustval(int*, unsigned char*, unsigned char*, int, int);


int main(){

	char s[200]={0}; //stringa per il perocorso
    char sel;

    writeInstruction();
    system("cls");
    sceltafoto(s);

    do{
        sel=menu();
        if(sel==1){
            printf("---%d---", sel);
            if(levels(s)){
                printf("\nSorry, an error occured...\n");
                system("pause");
            }
        }
        else if(sel==2){
            printf("---%d---", sel);
            if(GaussianBlur(s)){
                printf("\nSorry, an error occured...\n");
                system("pause");
            }
        }
        else if(sel==3){
            printf("---%d---", sel);
            if(EdgeDetection(s)){
                printf("\nSorry, an error occured...\n");
                system("pause");
            }
        }
        else if(sel==4){
            printf("---%d---", sel);
            if(imagestretching(s)){
                printf("\nSorry, an error occured...\n");
                system("pause");
            }
        }
        else if(sel==5){
            printf("---%d---", sel);
            if(segmentation(s)){
                printf("\nSorry, an error occured...\n");
                system("pause");
            }
        }
        else if(sel==6){
            getchar();
            system("cls");
            sceltafoto(s);
        }
        else if(sel==7){
            ShellExecute(NULL, NULL, s, NULL, NULL, SW_SHOWNORMAL);
        }
        else if(sel==8){
            ShellExecute(NULL, NULL, "READ_ME.txt", NULL, NULL, SW_SHOWNORMAL);
        }
        else{
            system("cls");
            Sleep(500);
            printf("\n\nThanks! See you soon!\n\n");
        }
    }while(sel!=9);

    system("pause");

    return 0;
}


/**FRONTEND**/

void sceltafoto(char s[]){

    char errore; //return delle funzioni per comunicare eventuali errori all'utente

    printf("Hi, tell me the path to the image\nOtherwise only %cnomefoto.bmp%c if the file is in the working directory\n", 34, 34);
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
    int size;

    if(m==LOAD){
        a = loadBMP(s, &img);
        if(a==0){
            a = checkbmp(s);
    }
    }
    else{
        a = loadBMP(s, &img);
        size = img.info.height*img.info.width; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        image = (unsigned char*) malloc(size*sizeof(unsigned char));
        a= loadIMG(s, image, size);
        a = saveBMP(img, d, image);
        a = loadBMP(d, &img);
        if(a==0){
            a = checkbmp(d); //se l'header � corretto l'errore della mancanza del formato corretto nel percorso viene ignorato
        }
    }

    return a;
}

int checkbmp(char s[]){ //se carico un file che non ha nessun formato ma che � stato salvato tramite saveBMP la funzione LOAD non individua l'errore

    int l;
    l=strlen(s);
    if(s[l-1]=='p' && s[l-2] == 'm' && s[l-3] == 'b')
        return 0;
    else
        return 2;

}

void errori(char e){
    if(e==1){
        printf("\n!!!You have inserted an unexistent or incorrect path! Remember that the path must end with filename.bmp\n");
    }else if(e==2){
        printf("\n!!!You have tried to load a non bmp image, convert it and try again!\n");
    }else{
        printf("\n!!!The image it's not an 8-bit one, sorry but i can only process 8-bit images\n");
    }
}

int menu(){

    int s;

    Sleep(100);
    system("cls");
    printf("\nNice! Now tell me what you want to do with your image:\n\n\n");
    printf("\t1. Histrogram visualization;\n\n");
    Sleep(150);
    printf("\t2. Gaussian filter application. Dimension and standard deviation can be choosed;\n\n");
    Sleep(150);
    printf("\t3. Edge detection with Sobel operator;\n\n");
    Sleep(150);
    printf("\t4. Image stretching;\n\n");
    Sleep(150);
    printf("\t5. Image clustering;\n\n");
    Sleep(150);
    printf("\t6. Change the image to modify;\n\n");
    Sleep(150);
    printf("\t7. Visualize the current image;\n\n");
    Sleep(150);
    printf("\t8. Instructions;\n\n");
    Sleep(150);
    printf("\t9. I'm done! Close the app!");
    Sleep(150);

    do{
        printf("\n\nMake your choice <1/2/3/4/5/6/7/8/9>: ");
        scanf("%d", &s);
    }while(s<1 || s>9);

    return s;
}

void str_formatting(char s[]){

    unsigned char i=0;
    printf("\nFile path: ");

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

	int size;
	int i, occ_max, sum = 0;
	float l;
	int *occ;
	char d[100];

	destinazione(s, d);

	loadBMP(s, &img);
	size = img.info.height*img.info.width;
	image = (unsigned char*) malloc(size*sizeof(unsigned char));
    loadIMG(s, image, size); //i pixel sono in image

	do{
       printf("\n\nTell me the number of levels (0 - 256) for the histogram extraction: ");
       scanf("%f", &l);
	}while(l<1 || l>256 || l-floor(l)!=0);

    occ=(int *) calloc(l,sizeof(int)); //alloco solo la memoria che mi serve per salvare le occcorrenze di ogni livello
	hist(image, occ, l, size);
	free(image);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	occ_max=*occ;

	for(i=0; i<l; i++){
        printf("\n%d%c Level ", i+1, 248);
        if(l!=256)
            printf("(%.0f-%.0f): %d", i*GRAYSCALE/l,i*GRAYSCALE/l+GRAYSCALE/l-1, *(occ+i));
        else
            printf("%d", *(occ+i));

        sum = sum + *(occ+i);
        if(*(occ+i)>occ_max)
            occ_max=*(occ+i);
	}

	if(sum != img.info.height*img.info.width){
        printf("\n%d pixel are missing\n", img.info.height*img.info.width-sum);
        system("pause");
        return 1;
	}

    generatehist(img, occ, occ_max, l, d);
	free(occ);

	printf("\n\nYou can find the histogram in the path you told me before!\n\n");
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
        Tutti questi accorgimenti come ceil(), roof() e quindi l'utilizzo di variabili di tipo float invece che int � dovuto al voler migliorare
        la precisione nella determinazione del livello. Inizialmente le divisioni erano tra interi ma provocavano alcuni problemi quando l != 2^x.
        Infatti risultava che tutti i pixel con .grey >= di un certo valore andavano "persi" (es: l=162, amp = 1.57... approssimata a 1--> img...grey
        oltre un certo valore vanno persi (es img.data.grey = 196, 196/1 = 196 ma il livello 196 non � stato allocato...) e quindi la somma delle
        occorrenze non risulta pi� valida... Utilizzando invece i float e la accortezza che roof(grey/amp) non fosse maggiore o uguale al numero
        dei livelli si � sia migliorata la precisione, sia evitato la "perdita" di pixel...
        La miglior precisione la si pu� verificare anche confrontando i risultati con l=256 e l=x... (pi� immediato con le immagini con
        pochi pixel chiari o pochi scuri)
    **/

    for(j=0;j<size;j++){
        x=*(img+j)/amp; //la divisione ritorna quante volte l'"ampiezza" sta nel valore del pixel, determinando cos� il livello di appartenenza
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
    histo.header.size = 656438; //dimensioni su dico
    histo.info.width = 1280;
    histo.info.height = 512;
    histo.info.imagesize = histo.info.width*histo.info.height;

    unsigned char* histogram;
    histogram = (unsigned char*) malloc(histo.info.imagesize*sizeof(unsigned char));

    int i,j,k,amp,start;
    start = 0;
    amp = histo.info.width/l;

    for(i=0; i<histo.info.imagesize; i++){
        *(histogram+i)=10; //sfondo grigio-scuro
	}

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
    free(histogram);
}

/**FINE LEVELS **/

/**INIZIO GAUSSIAN BLUR**/

int GaussianBlur(char s[]){

    int dim, i, j, size;
    float sigma;
    float* g;
    BMP_Image img;
    unsigned char* f_img;
    unsigned char* image;
    char d[100], r;

    destinazione(s, d);

    do{
        printf("Tell me the dimension (odd) of the Gaussian kernel: ");
        scanf("%d", &dim);
    }while(dim%2==0 || dim < 2);

    do{
        printf("Tell me the standard deviation of the filter you want (std>0): ");
        scanf("%f", &sigma);
    }while(sigma <= 0);

    printf("Good, i'm generating a %d%c%d kernel with a std of %.5f\n\n", dim, 158, dim, sigma);

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
    size = img.info.height*img.info.width;
    image = (unsigned char*) malloc(size*sizeof(unsigned char));
    loadIMG(s, image, size); //i pixel sono in image

    f_img = (unsigned char*) malloc((img.info.height+dim-1)*(img.info.width+dim-1)* sizeof(unsigned char));
    gaussianfilter(image, g, f_img, dim, img.info.height, img.info.width);
    free(f_img);
    free(g);

    saveBMP(img, d, image);
    free(image);//!!!!!!!!!!!!!!!!!!!!!!!!

    printf("\n\nYou can find the modified image in the path you told me before!\n\n");
	ShellExecute(NULL, NULL, d, NULL, NULL, SW_SHOWNORMAL);

	printf("\nDo you want to use the modified image for next operations? <y/n>: ");
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

float bi_gauss(float sigma, int x, int y){ //� simmetrica, x e y interscambiabili...

    float g;

    g=(1/(2*PI*pow(sigma, 2)));
    g=g*exp((-x*x-y*y)/(2*sigma*sigma)); //completa

    return g;
}

void gaussianfilter(unsigned char* img, float* g, unsigned char* f_img, int dim, int h, int w){
    int* raw_image;

    falseimage(img, f_img, dim, h, w); //genero l'immagine fittizia
    raw_image = (int*) calloc (h*w, sizeof(int));
    convoluzione(raw_image, f_img, g, dim, h, w); //creo un immagine che avr� valori che possono andare oltre 255 (integrale di Gauss = sqrt(PI_Greco)
    normalisation(img, raw_image, h, w); //normalizzo l'immagine con valori compresi tra 0 e 255;
    free(raw_image);
}

void normalisation(unsigned char* img, int* G, int h, int w){
    int max, min, i, d;

    max=findmax(G, h, w);
    min=findmin(G, h, w);
    //nota che il max andava trovato dopo *(G+i)-=d;
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
	int i, size;

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

    destinazione(s, d);

    loadBMP(s, &img);
    size = img.info.height*img.info.width;
    image = (unsigned char*) malloc(size*sizeof(unsigned char));
    loadIMG(s, image, size); //i pixel sono in image

    sobeloperator(image, &Sx[0][0], &Sy[0][0], &S_45[0][0], &S_135[0][0], 5, img.info.height, img.info.width);

	saveBMP(img, d, image);
	free(image);

	printf("\n\nYou can find the modified image in the path you told me before!\n\n");
	ShellExecute(NULL, NULL, d, NULL, NULL, SW_SHOWNORMAL);

	printf("\nDo you want to use the modified image for next operations? <y/n>: ");
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

    //genero le "immagini" con cui individuer� i bordi verticali, orizzontali e obliqui separatamente
    int* Gx;
    int* Gy;
    int* G_45;
    int* G_135;
    int* G;
    unsigned char* f_img;

    f_img = (unsigned char*) malloc((h+DIM-1)*(w+DIM-1)* sizeof(unsigned char));
	falseimage(img, f_img, DIM, h, w); //genero l'immagine fittizia

	Gx = (int*) malloc (h*w * sizeof(int)); //� un int poic� ci saranno anche valori negativi inferiori a -128
	Gy = (int*) malloc (h*w * sizeof(int));
	G_45 = (int*) malloc (h*w * sizeof(int)); //� un int poic� ci saranno anche valori negativi inferiori a -128
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

    threshold(img, h*w);
}

void magnitude(int* Gx, int* Gy, int* G_45, int* G_135, int* G, int h, int w){
    int i;
    for(i=0; i<h*w; i++){
        *(G+i)=sqrt(pow(*(Gx+i), 2)+pow(*(Gy+i), 2)+pow(*(G_45+i), 2)+pow(*(G_135+i), 2));
    }
}

void threshold(unsigned char* img, int size){
    int i, thresh;
    char r;

    printf("\n\nWould you like to insert the threshold value?");
    do{
        printf("\n\n<y/n>: ");
        scanf("\n%c", &r);
    }while(r!='y' && r!='n');

    if(r=='y'){
        printf("\n\nInsert threshold value: ");
        scanf("%d", &thresh);

        for(i=0; i<size; i++){
            if(*(img+i)>=thresh)
                *(img+i)=255;
            else
                *(img+i)=0;
        }
    }
}

/**FINE EDGE DETECTION**/

/**INIZIO IMAGE STRETCHING**/

int imagestretching(char s[]){

    BMP_Image img;
    unsigned char* image;
    char d[100], r;
    int i, size;

	destinazione(s, d);

	loadBMP(s, &img);
	size = img.info.height*img.info.width;
    image = (unsigned char*) malloc(size*sizeof(unsigned char));
    loadIMG(s, image, size); //i pixel sono in image

    stretching(image, img.info.height, img.info.width);
	saveBMP(img, d, image);
	free(image);

	printf("\n\nYou can find the modified image in the path you told me before!\n\n");
	ShellExecute(NULL, NULL, d, NULL, NULL, SW_SHOWNORMAL);

	printf("\nDo you want to use the modified image for next operations? <y/n>: ");
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

    printf("\n\nDo you rather proceed setting upper and lower limits via:\n\t1. Grayscale values (0-255);\n\t2. Percentiles\n\t3. Automatic detection");
    Sleep(500);
    do{
        printf("\n\nYour choice: ");
        scanf("%d", &s);
    }while(s<1 || s>3);

    if(s==2){
        printf("\n\nA common percentiles choice is 5%c and 95%c.", 37, 37);

        do{
            printf("\n\nLower percentile: ");
            scanf("%f", &p_inf);
            printf("\n\nUpper percentile: ");
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
            printf("\n\nLower value: ");
            scanf("%d", &c);
            printf("\n\nUpper value: ");
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

    printf("\n\n\n\n\nWould you like to chose the minimum amount of pixel belonging to a level in order to not be considered as noise? ");
    Sleep(150);
    printf("\nAnswer no (n) to use default values");
    do{
        printf("\n\n<y/n>: ");
        scanf("\n%c", &r);
    }while(r!='y' && r!='n');

    if(r=='y'){
        printf("\n\nWhat is the minimum amount of pixel to consider a level as noise?");
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

/**INIZIO IMAGE-SEGMENTATION**/

int segmentation(char s[]){

    BMP_Image img;
    unsigned char* image;
    char d[100], r;
    int i, size;

	destinazione(s, d);

	loadBMP(s, &img);
	size = img.info.height*img.info.width;
    image = (unsigned char*) malloc(size*sizeof(unsigned char));
    loadIMG(s, image, size); //i pixel sono in image

    clustering(image, img.info.height, img.info.width);

    saveBMP(img, d, image);
	free(image);

	printf("\n\nYou can find the modified image in the path you told me before!\n\n");
	ShellExecute(NULL, NULL, d, NULL, NULL, SW_SHOWNORMAL);

	printf("\nDo you want to use the modified image for next operations? <y/n>: ");
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

void clustering(unsigned char* img, int h, int w){

    int i, j, k, diff, clusters; //numero di "parti" nelle quali verr� suddivisa l'immagine
    int min, tmp;
    int* clust_val;
    unsigned char* k_bel; //cluster di appartenenza

    printf("\nChoose the number of centroids: ");
    scanf("%d", &clusters);

    k_bel = (unsigned char*) calloc(h*w, sizeof(unsigned char));
    clust_val = (int*) calloc(clusters, sizeof(int));
    srand(time(NULL));

     //genero x clusters casualmente
    for(i=0; i<clusters; i++){
        do{
            *(clust_val+i) = rand() % (GRAYSCALE); // genero numeri casuali tra 0 e 255
            diff = 0;
            for(j=i-1; j>=0; j--){                  //controllo che non ci siano due cluster uguali
                if(*(clust_val+i) == *(clust_val+j))
                    diff=1;
            }
        }while(diff==1);
    }

    for(k=0; k<50; k++){
    //assegno cluster di appartenenza per ogni pixel
        printf("\n#It: %d", k);
        for(i=0; i<h*w; i++){
            min = *(img+i)-*clust_val;
            if(min<0)
            min*=-1;
            *(k_bel+i) = 0;

            for(j=1; j<clusters; j++){
                tmp = *(img+i)-*(clust_val+j);
                if(tmp<0)
                    tmp *= -1;
                if(tmp<min){
                    min = tmp;
                    *(k_bel+i) = j; //significa che l'i-esimo pixel appartiene al j-esimo cluster
                }
            }
        }
        newclustval(clust_val, k_bel, img, clusters, h*w);
    }
    free(clust_val);

    for(i=0; i<h*w; i++){
        *(img+i) = ((GRAYSCALE-1)/clusters)*(*(k_bel+i));
    }
    free(k_bel);

}

void newclustval(int* clust_val, unsigned char* k_bel, unsigned char* img, int clusters, int size){

    int* count, i;
    count = (int*) calloc(clusters, sizeof(int));

    for(i=0; i<clusters; i++)
        *(clust_val+i)=0;


    for(i=0; i<size; i++){
        *(clust_val+(*(k_bel+i)))+=*(img+i); //sto sommando i valori di tutti i pixel appartenenti ad un determinato cluster
        *(count+(*(k_bel+i)))+=1;
    }
//    printf("\n\n");
    for(i=0; i<clusters; i++){
        if(*(count+i)!=0)
            *(clust_val+i)/=*(count+i);

//        printf("\n%d: %d:", i+1, *(clust_val+i));
    }
    free(count);
}

/**END IMAGE-SEGMENTATION**/

void writeInstruction(){

    FILE* fp;
    fp = fopen("instructions.txt", "w+");

    fprintf(fp, "Usage:\n\n");

    fprintf(fp, "1. The file path where the image is stored is requested\n");
    fprintf(fp, "2. The action to perform is requested. You can change the image to modify (back to\n");
    fprintf(fp, "   point 1) or you can exit the application\n");
    fprintf(fp, "3. Once the action to perform is chosen, the path where you want to store the result is requested\n");

    fprintf(fp, "\nP:\n\n");

    fprintf(fp, "1. The path to the folder must already exist\n");
    fprintf(fp, "2. Path file must include the file name with his extension (i.e. filename.bmp)\n");
    fprintf(fp, "\t e.g: C:%cUsers%cUserName%cDesktop%cPictures%cfilename.bmp\n", 92, 92, 92, 92, 92);
    fprintf(fp, "3. The application can load and save only 8 bit-bitmap-not compressed images\n");



    fclose(fp);


}

