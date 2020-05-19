#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <stdint.h>
/** Sabitler */
#define dosyaOkumaBoyutu 5000000
#define okuyaBilecegimBoyut 1500001
#define true 1
#define false 0
#define MaksimumAgac 100
#define bitKaydir 4
#define searchBufferSize 4095
#define forwardBufferSize 15

/** Degiskenler */

char text[dosyaOkumaBoyutu];
char hufmanHarf[dosyaOkumaBoyutu / 10];
char huffmanOzelText[dosyaOkumaBoyutu * 5];
char huffmanKod[dosyaOkumaBoyutu / 10][1000];
char LZ77Metin[dosyaOkumaBoyutu * 5];
char huffMetin[dosyaOkumaBoyutu * 10];
char DeflateMetin[dosyaOkumaBoyutu *10];
int k = 0;

/** Fonksiyonlar */
void HuffmanCiktisi();
void LZ77(char sikistirilacakMetin[],int algoritmaTipi);
void ReadFile();
void encode(int offset,int lenght,char character,FILE *encodeFile,int algoritmaTipi);
struct MinHeapNode *yeniNode(char data, unsigned freanksDegeri);
struct MinHeap *createMinHeap(unsigned kapasite);
void MinHeapDugumTakasi(struct MinHeapNode **a, struct MinHeapNode **b);
void minHeapify(struct MinHeap *minHeap, int idx);
int birlikBoyut(struct MinHeap *minHeap);
struct MinHeapNode *extractMin(struct MinHeap *minHeap);
void MinHeapOlustur(struct MinHeap *minHeap, struct MinHeapNode *minHeapNode);
void buildMinHeap(struct MinHeap *minHeap);
void printArray(int arr[], int n);
int isLeaf(struct MinHeapNode *root);
struct MinHeap *olusturVeDerleMinHeap(char data[], int freanksDegeri[], int size);
struct MinHeapNode *buildHuffmanTree(char data[], int freanksDegeri[], int size);
void printHuffmanKodlari(struct MinHeapNode *root, int arr[], int top);
void printHuffmanTamMetin(char sikistirilacakMetin[]);
void HuffmanKodlari(char data[], int freanksDegeri[], int size);
void Frekans(char sikistirilacakMetin[]);
void Deflate(char metin[]);
void sikistirmaOrani(char algoritmaTipi);



/* HUFFMAN*/
struct MinHeapNode
{
    char data;
    unsigned freanksDegeri;
    struct MinHeapNode *sol, *sag;
};

struct MinHeap
{
    unsigned size;
    unsigned kapasite;
    struct MinHeapNode **array;
};


struct MinHeapNode *yeniNode(char data, unsigned freanksDegeri)
{
    struct MinHeapNode *temp = (struct MinHeapNode *)malloc(sizeof(struct MinHeapNode));

    temp->sol = temp->sag= NULL;
    temp->data = data;
    temp->freanksDegeri = freanksDegeri;

    return temp;
}

struct MinHeap *createMinHeap(unsigned kapasite)
{
    struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));

    minHeap->size = 0;

    minHeap->kapasite = kapasite;

    minHeap->array = (struct MinHeapNode **)malloc(minHeap->kapasite * sizeof(struct MinHeapNode *));
    return minHeap;
}

void MinHeapDugumTakasi(struct MinHeapNode **a, struct MinHeapNode **b)
{
    struct MinHeapNode *t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(struct MinHeap *minHeap, int idx)
{
    int enKucuk = idx;
    int sol = 2 * idx + 1;
    int sag= 2 * idx + 2;

    if (sol < minHeap->size && minHeap->array[sol]->freanksDegeri < minHeap->array[enKucuk]->freanksDegeri)
        enKucuk = sol;

    if (sag< minHeap->size && minHeap->array[sag]->freanksDegeri < minHeap->array[enKucuk]->freanksDegeri)
        enKucuk = sag;

    if (enKucuk != idx)
    {
        MinHeapDugumTakasi(&minHeap->array[enKucuk], &minHeap->array[idx]);
        minHeapify(minHeap, enKucuk);
    }
}

int birlikBoyut(struct MinHeap *minHeap)
{
    return (minHeap->size == 1);
}

struct MinHeapNode *extractMin(struct MinHeap *minHeap)
{
    struct MinHeapNode *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];

    --minHeap->size;
    minHeapify(minHeap, 0);

    return temp;
}

void MinHeapOlustur(struct MinHeap *minHeap, struct MinHeapNode *minHeapNode)
{
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freanksDegeri < minHeap->array[(i - 1) / 2]->freanksDegeri)
    {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

void buildMinHeap(struct MinHeap *minHeap)
{
    int n = minHeap->size - 1;
    int i;

    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

void printArray(int arr[], int n)
{
    int i;
    char harf[n];
    for(int m = 0; m<n; m++)
        harf[m] = '\0';

    for (i = 0; i < n; ++i)
    {
        //printf("%d", arr[i]);
        sprintf(harf+strlen(harf), "%d", arr[i]);

    }
    strcpy(huffmanKod[k],harf);
    k++;


    //printf("\n");
}

int isLeaf(struct MinHeapNode *root)
{

    return !(root->sol) && !(root->sag);
}

struct MinHeap *olusturVeDerleMinHeap(char data[], int freanksDegeri[], int size)
{
    struct MinHeap *minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = yeniNode(data[i], freanksDegeri[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    return minHeap;
}

struct MinHeapNode *buildHuffmanTree(char data[], int freanksDegeri[], int size)
{
    struct MinHeapNode *sol, *sag, *top;
    struct MinHeap *minHeap = olusturVeDerleMinHeap(data, freanksDegeri, size);

    while (!birlikBoyut(minHeap))
    {

        sol = extractMin(minHeap);
        sag= extractMin(minHeap);

        top = yeniNode('$', sol->freanksDegeri + sag->freanksDegeri);

        top->sol = sol;
        top->sag= sag;

        MinHeapOlustur(minHeap, top);
    }
    return extractMin(minHeap);
}


void printHuffmanKodlari(struct MinHeapNode *root, int arr[], int top)
{
    if (root->sol)
    {

        arr[top] = 0;
        printHuffmanKodlari(root->sol, arr, top + 1);
    }
    if (root->sag)
    {

        arr[top] = 1;
        printHuffmanKodlari(root->sag, arr, top + 1);
    }
    if (isLeaf(root))
    {

        //printf("  %c   | ", root->data);
        hufmanHarf[k] = root->data;
        printArray(arr, top);
    }
}

char huffSifreliNormalMetin[dosyaOkumaBoyutu * 10];
void printHuffmanTamMetin(char sikistirilacakMetin[])
{
    printf("Lütfen bekleyiniz..\n");
    for(int i = 0; i<strlen(sikistirilacakMetin); i++)
    {
        for(int l = 0; l < k; l++)
        {
            if(sikistirilacakMetin[i] == hufmanHarf[l])
            {
                strcat(huffMetin,huffmanKod[l]);
            }
        }
    }
    for(int i = 0; i< strlen(hufmanHarf);i++)
    {
        char ozelKoda[50];
        sprintf(ozelKoda,"%s%c",huffmanKod[i],hufmanHarf[i]);
        strcat(huffmanOzelText,ozelKoda);

    }
}

void HuffmanKodlari(char data[], int freanksDegeri[], int size)
{
    struct MinHeapNode *root = buildHuffmanTree(data, freanksDegeri, size);

    int arr[MaksimumAgac], top = 0;

    printHuffmanKodlari(root, arr, top);
}


void Frekans(char sikistirilacakMetin[])
{

    int frekansData[260];
    char frekansChar[260];
    int frekansArrayPointer = 0;
    for(int textPointer = 0; textPointer< strlen(sikistirilacakMetin); textPointer++)
    {
        int frekansVarMi = false;
        for(int frekansPointer = 0; frekansPointer < 260; frekansPointer++)
        {
            if(sikistirilacakMetin[textPointer] == frekansChar[frekansPointer])
            {
                frekansVarMi = true;
                frekansPointer++;
                frekansData[frekansPointer -1] += 1;
            }
        }
        if(frekansVarMi == false)
        {
            frekansData[frekansArrayPointer] = 1;
            frekansChar[frekansArrayPointer] = sikistirilacakMetin[textPointer];
            frekansArrayPointer+=1;
        }
               if((textPointer * 100  ) / (strlen(sikistirilacakMetin) - 5) % 5 == 0)
            printf("\rHuffman Durum: %5d%%",(textPointer * 100  ) / (strlen(sikistirilacakMetin) -5));
    }
    printf("\n");
    HuffmanKodlari(frekansChar,frekansData,frekansArrayPointer);
    printHuffmanTamMetin(sikistirilacakMetin);



}
void Deflate(char metin[])
{
    char kopyaMetin[strlen(metin)];
    strcpy(kopyaMetin,metin);
    //printf("%s\n",kopyaMetin);
    Frekans(metin);
    HuffmanCiktisi();
    LZ77(huffMetin,1);

}

void HuffmanCiktisi()
{
    FILE *huffDosya = fopen("outputs/HuffmanAgaci.txt","wb");
//outputs/metin.lz77","wb"
    for(int i = 0; i< strlen(hufmanHarf); i++)
    {
        fprintf(huffDosya,"%c | %s\n",hufmanHarf[i],huffmanKod[i]);
    }
    fclose(huffDosya);
    printHuffmanTamMetin("Huffman agaci olusturuldu...\n");

}



/** LZ 77 */
void LZ77(char sikistirilacakMetin[],int algoritmaTipi)
{

    char kopyaMetin[strlen(sikistirilacakMetin)];
    strcpy(kopyaMetin,sikistirilacakMetin);

/// 1111 1111 111 11111


    FILE *encodeFile;
    if(algoritmaTipi == 0)
    {

        encodeFile = fopen("outputs/metin.lz77","wb");
    }
    else if(algoritmaTipi == 1)
    {

        encodeFile = fopen("outputs/metin.deflate","wb");
    }

    /**
              256               256               XXX
        |**SEARCH**|------|**FORWARD**|-------|**DEVAMI**||
    */
    int pointer = 0,i = 0,harfVarMi = false,hafizaBenzerlik = 0,hangisindenItibaren = 0,benzerlik = 0;
    while(pointer < strlen(kopyaMetin))
    {
        i= pointer -1;
        if(i == -1)
        {
            encode(0,0,kopyaMetin[pointer],encodeFile,algoritmaTipi);
            pointer++;
        }
        else
        {
            harfVarMi = false;
            while (i > pointer - searchBufferSize)
            {
                if(kopyaMetin[i] == kopyaMetin[pointer])
                {
                    harfVarMi = true;
                    break;
                }
                i--;
            }
            if(harfVarMi == false)
            {
                encode(0,0,kopyaMetin[pointer],encodeFile,algoritmaTipi);
                pointer++;
            }
            else
            {
                hafizaBenzerlik = 0;
                hangisindenItibaren = 0;
                while(i > pointer - searchBufferSize)
                {
                    if(kopyaMetin[i] == kopyaMetin[pointer])
                    {
                        benzerlik = 0;
                        while(kopyaMetin[i+benzerlik] == kopyaMetin[pointer+benzerlik] && benzerlik < forwardBufferSize)
                            benzerlik++;

                        if(benzerlik > hafizaBenzerlik)
                        {
                            hafizaBenzerlik = benzerlik;
                            hangisindenItibaren = i;
                        }
                    }
                    i--;
                }

                //encode((pointer-hangisindenItibaren),hafizaBenzerlik,kopyaMetin[pointer+hafizaBenzerlik],encodeFile,algoritmaTipi);



                if(strlen(text)-1 < pointer+hafizaBenzerlik)
                {
                    // printf("<%d,%d,%c>\n",(pointer-hangisindenItibaren),hafizaBenzerlik,text[pointer+hafizaBenzerlik-1]);
                    encode((pointer-hangisindenItibaren),hafizaBenzerlik,kopyaMetin[pointer+hafizaBenzerlik -1],encodeFile,algoritmaTipi);
                }
                else
                {
                    //  printf("<%d,%d,%c>\n",(pointer-hangisindenItibaren),hafizaBenzerlik,text[pointer+hafizaBenzerlik]);

                    encode((pointer-hangisindenItibaren),hafizaBenzerlik,kopyaMetin[pointer+hafizaBenzerlik],encodeFile,algoritmaTipi);
                }



                pointer += hafizaBenzerlik +1;
            }
        }
        /*
        total 100
        pointer x
        */

        if((pointer * 100  ) / strlen(sikistirilacakMetin) % 5 == 0)
            printf("\rDurum: %13d%%",(pointer * 100  ) / strlen(sikistirilacakMetin));


    }
    fclose(encodeFile);
    sikistirmaOrani(algoritmaTipi);
}

float lzSikistirmaKB,defSikistirmaKB;
void sikistirmaOrani(char algoritmaTipi)
{
    int orjinalDosyaBoyutu,sikismisDosyaBoyutu;
    FILE *orDosya = fopen("metin.txt", "rb");
    fseek(orDosya,0,SEEK_END);
    orjinalDosyaBoyutu = ftell(orDosya);
    fclose(orDosya);
    FILE *lzdosya;
    if(algoritmaTipi == 0)
        lzdosya = fopen("outputs/metin.lz77", "rb");
    else if(algoritmaTipi == 1)
        lzdosya = fopen("outputs/metin.deflate", "rb");
    fseek(lzdosya,0,SEEK_END);
    sikismisDosyaBoyutu = ftell(lzdosya);
    fclose(lzdosya);
     float kbOrj = orjinalDosyaBoyutu /1024.0;
    float kbSikist = sikismisDosyaBoyutu /1024.0;
    if(algoritmaTipi == 0)
    {
        lzSikistirmaKB = kbSikist;
    } else {
        defSikistirmaKB = kbSikist;
    }
    printf("\n---------------------------------------\n");


    printf("Dosya boyutu              : %5.2f KB\n",kbOrj);
    printf("Sikistirilmis dosya boyutu: %5.2f KB\n",kbSikist);
    float sikistirilmisYuzde;
    sikistirilmisYuzde = 100 - ((kbSikist * 100.0) / kbOrj);
    printf("Sikistirma orani          : [%5.2f%%]\n",sikistirilmisYuzde);
    printf("\n---------------------------------------\n");

}

void ReadFile()
{
    for(int i = 0; i<strlen(text); i++)
    {
        text[i] = '\0';
    }
    FILE *dosya;
    if((int)(dosya = fopen("metin.txt", "r")) == 0)
        printf("Dosya bulunamadi");
    else
    {
        char ch[10000];
        while(!feof(dosya))
        {
            fgets(ch,10000,dosya);
            strcat(text,ch);
        }

    }
    fclose(dosya);
}

struct encoNode
{
    uint16_t offsetLen;
    unsigned char  harf;
};
int yazmaDurumu = 0;
void encode(int offset,int lenght,char character,FILE *encodeFile,int algoritmaTipi)
{
//247 248
    //printf("<%d,%d,C(%c)>\n",offset,lenght,character);
    char tempText[20];
    if(algoritmaTipi == 0)
    {
        sprintf(tempText,"<%d,%d,C(%c)>",offset,lenght,character);
        strcat(LZ77Metin,tempText);
    }
    else if(algoritmaTipi == 1)
    {
        sprintf(tempText,"<%d,%d,C(%c)>",offset,lenght,character);
        //fprintf(encodeFile,"%s",huffSifreliNormalMetin);
        //fwrite(huffmanOzelText,strlen(huffmanOzelText)*3,1,encodeFile);

        if(yazmaDurumu == 0)
        {
            //printf("%s",huffmanOzelText);
            fwrite(huffmanOzelText,strlen(huffmanOzelText)*3,1,encodeFile);
            //fwrite(huffSifreliNormalMetin,strlen(huffSifreliNormalMetin),1,encodeFile);
            yazmaDurumu += 1;
        }
        strcat(DeflateMetin,tempText);
    }


    offset = offset<<bitKaydir;
    offset += lenght;




    struct encoNode temp;
    temp.harf = character;
    temp.offsetLen = offset;

    //printf("Dosya Bicimi: \t<%d,C(%c)> \n",offset,character);
    //fprintf(dosya,"%c%c",offset,(unsigned char)character);
    fwrite(&temp,sizeof(temp),1,encodeFile);

}

void SikistirilmamisDosya()
{
    FILE *LZ77Normal =fopen("outputs/LZ77Normal.txt","w");
    FILE *DEFLATENormal =fopen("outputs/DeflateNormal.txt","w");
    fprintf(LZ77Normal,"%s",LZ77Metin);
    fprintf(DEFLATENormal,"%s",huffmanOzelText);
    fprintf(DEFLATENormal,"%s",DeflateMetin);
    fclose(LZ77Normal);
    fclose(DEFLATENormal);


}
void karsilastirmaYap(){
printf("Algoritma Karsilastirma Basliyor\n");
if(lzSikistirmaKB > defSikistirmaKB)
{

    float oran = 100 - (defSikistirmaKB * 100.0) / lzSikistirmaKB;
    printf("-----------------------------------------------------------------------\n\n");
    printf("DEFLATE algoritmasi LZ77'den  %3.2f%% oraninda daha iyi sikistirmistir.\n",oran);
    printf("HESAPLAMA FORMULU\n");
    printf("YUZDE ORAN: %d%% - ( %5.2f KB * 100%% ) / %5.2f KB = %5.2f%%\n",100,defSikistirmaKB,lzSikistirmaKB,oran);
    printf("-----------------------------------------------------------------------\n");
} else {
    float oran = 100 - (lzSikistirmaKB * 100.0) / defSikistirmaKB;
    printf("-----------------------------------------------------------------------\n");

    printf("LZ77 algoritmasi DEFLATE'den %3.2f%% oraninda daha iyi sikistirmistir.\n\n",oran);
    printf("HESAPLAMA FORMULU\n");
    printf("YUZDE ORAN: %d%% - ( %5.2f KB * 100%% ) / %5.2f KB = %5.2f%%\n",100,lzSikistirmaKB,defSikistirmaKB,oran);
    printf("-----------------------------------------------------------------------\n");
}
printf("Algoritma Karsilastirma Bitti\n");

}
int main()
{
    setlocale(LC_ALL, "Turkish");

    system("mkdir outputs");
    system("cls");

    printf("Adi Soyadi: Engin Yenice\n");
    printf("Numara    :    190201133\n\n");
    printf("---------------------------------------------------------------\n");
    printf("!!!          PROGRAM YAVAS CALISMAKTADIR                    !!!\n");
    printf("!!!          LUTFEN SONUCLARI BEKLEYINIZ                    !!!\n");
    printf("!!! HIZLI SONUC ICIN 1MB ALTINDAKI DOSYALAR TEST EDILEBILIR !!!\n");
    printf("---------------------------------------------------------------\n");
    printf("Search Buffer  : %4d\n", searchBufferSize);
    printf("Forward Buffer : %4d\n", forwardBufferSize);

    ReadFile();

    if(strlen(text) > okuyaBilecegimBoyut)
    {
        printf("%d degerinin ustundeki verileri kucultemiyorum...\n",okuyaBilecegimBoyut);
        printf("%d Dosya boyutunuz......\n",strlen(text));
    }
    else {
    printf("LZ77 BASLIYOR\n");

    LZ77(text,0);
    printf("LZ77 BITTI\n\n");

    printf("---------------------------------------\n");


    printf("DEFLATE BASLIYOR\n\n");
    printf("Lutfen bekleyiniz...\n");
    ReadFile();
    Deflate(text);
    //printf("%s\n",huffMetin);
    printf("DEFLATE BITTI\n\n");
    printf("---------------------------------------\n");
    karsilastirmaYap();
    SikistirilmamisDosya();
    }



    system("pause");
    return 0;
}
