#include <stdio.h>
#include <stdlib.h>
#include "bmpv2.h"

/** Funzione per caricare un'immagine BMP da file
Parametri:
	nome del file in lettura, puntatore alla struttura in cui memorizzare i dati dell'immagine
Valore di ritorno:
	0 se il caricamento è avvenuto con successo, un numero diverso da 0 altrimenti
*/
int loadBMP(char *filename, BMP_Image *image)
{

	FILE *fp;

	fp = fopen(filename, "rb");

	if(fp == NULL)
	{
		//printf("Error: impossibile aprire il file in lettura\n");
		return 1;
	}

    //fread(dove salvare i dati, dimensione singolo dato, quantità di dati, puntatore al file da cui leggere i dati)
	fread(image->magic, sizeof(image->magic), 1, fp);

	if(image->magic[0]!='B' || image->magic[1]!='M')
	{
		//printf("Error: tipo di immagine non corretto\n");
		return 2;
	}

	fread(&image->header, sizeof(image->header), 1, fp);
	fread(&image->info, sizeof(image->info), 1, fp);

	if(image->info.bits!=8)
	{
		//printf("Error: numero di bits/pixel diverso da 8\n");
		return 3;
	}

	fread(&image->color_table, sizeof(image->color_table), 1, fp);

	/*
	printf("\nmagic: %d", sizeof(image->magic));
	printf("\nheader: %d", sizeof(image->header));
	printf("\ninfo: %d", sizeof(image->info));
	printf("\ncolortable: %d", sizeof(image->color_table));
    */

    //*size=image->info.width*image->info.height;

	fclose(fp);
	return 0;

}

int loadIMG(char *filename, unsigned char* img, int size){

    FILE *fp;

	fp = fopen(filename, "rb");

	if(fp == NULL)
	{
		//printf("Error: impossibile aprire il file in lettura\n");
		return 1;
	}

	fseek(fp, 1078, SEEK_SET); //termine header file, header bitmap e color palette
	fread(img, size*sizeof(unsigned char), 1, fp);


	return 0;
}

/** Funzione per salvare una struttura BMP_Image su file
Parametri:
	puntatore alla struttura da cui leggere i dati dell'immagine, nome del file su cui scrivere
Valore di ritorno:
	0 se il salvataggio è avvenuto con successo, 1 altrimenti
*/
int saveBMP(BMP_Image image, char * filename, unsigned char* img){

	FILE *fp2;
	int size;

	fp2 = fopen(filename, "wb");

	if(fp2==NULL)
	{
		printf("Impossibile aprire il file in scrittura\n");
		return 1;
	}

	fwrite(&image.magic, sizeof(image.magic), 1, fp2);
	fwrite(&image.header, sizeof(image.header), 1, fp2);
	fwrite(&image.info, sizeof(image.info), 1, fp2);
	fwrite(&image.color_table, sizeof(image.color_table), 1, fp2);
	size = image.info.height*image.info.width;
	fwrite(img, size*sizeof(unsigned char), 1, fp2);

	fclose(fp2);
	return 0;

}


