#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <stdint.h>
/** Sabitler */
#define dosyaOkumaBoyutu 50000000
#define true 1
#define false 0

/** Degiskenler */
char text[dosyaOkumaBoyutu];

/** Fonksiyonlar */
void LZ77();
void ReadFile();
void encode(int offset,int lenght,char character,FILE *encodeFile);


















































































void LZ77()
{

    int searchBufferSize,forwardBufferSize;
    ReadFile();
    printf("\e[?25l");
    printf("Search Buffer Lenght (Arama Tamponu Uzunlugu): ");
    scanf("%d",&searchBufferSize);

    printf("Forward Buffer Lenght (Ýleri Tamponu Uzunlugu): ");
    scanf("%d",&forwardBufferSize);
    system("cls");

    printf("---LZ77 INFO---\nSearch Buffer : %d\n", searchBufferSize);
    printf("Forward Buffer : %d\n", forwardBufferSize);
    printf("In progress : \n");
    FILE *encodeFile;
    encodeFile = fopen("metin.lz77","wb");

    /**
              256               256               XXX
        |**SEARCH**|------|**FORWARD**|-------|**DEVAMI**||
    */
    int pointer = 0,i = 0,harfVarMi = false,hafizaBenzerlik = 0,hangisindenItibaren = 0,benzerlik = 0;
    while(pointer < strlen(text))
    {
        i= pointer -1;
        if(i == -1)
        {
            encode(0,0,text[pointer],encodeFile);
            pointer++;
        }
        else
        {
            harfVarMi = false;
            while (i > pointer - searchBufferSize)
            {
                if(text[i] == text[pointer])
                {
                    harfVarMi = true;
                    break;
                }
                i--;
            }
            if(harfVarMi == false)
            {
                encode(0,0,text[pointer],encodeFile);
                pointer++;
            }
            else
            {
                hafizaBenzerlik = 0;
                hangisindenItibaren = 0;
                while(i > pointer - searchBufferSize)
                {
                    if(text[i] == text[pointer])
                    {
                        benzerlik = 0;
                        while(text[i+benzerlik] == text[pointer+benzerlik] && benzerlik < forwardBufferSize)
                            benzerlik++;

                        if(benzerlik > hafizaBenzerlik)
                        {
                            hafizaBenzerlik = benzerlik;
                            hangisindenItibaren = i;
                        }
                    }
                    i--;
                }

                encode((pointer-hangisindenItibaren),hafizaBenzerlik,text[pointer+hafizaBenzerlik],encodeFile);
                pointer += hafizaBenzerlik +1;
            }
        }
        /*
        total 100
        pointer x
        */

        if((pointer * 100  ) / strlen(text) % 5 == 0)
            printf("\rDurum: %d%%",(pointer * 100  ) / strlen(text));
    }
    fclose(encodeFile);
    sikistirmaOrani();

}

void sikistirmaOrani()
{
    int orjinalDosyaBoyutu,sikismisDosyaBoyutu;
    FILE *orDosya = fopen("metin.txt", "rb");
    fseek(orDosya,0,SEEK_END);
    orjinalDosyaBoyutu = ftell(orDosya);
    fclose(orDosya);
    FILE *lzdosya = fopen("metin.lz77", "rb");
    fseek(lzdosya,0,SEEK_END);
    sikismisDosyaBoyutu = ftell(lzdosya);
    fclose(lzdosya);
    /* ORJ 100
       SKT  ?
       SKT * 100  / ORJ
    */
    printf("\n---------------------------------------\n");
    printf("Dosya boyutu : %d byte\n",orjinalDosyaBoyutu);
    printf("Sikistirilmis dosya boyutu: %d byte\n",sikismisDosyaBoyutu);
    float sikistirilmisYuzde;
    sikistirilmisYuzde = 100 - ((sikismisDosyaBoyutu * 100.0) / orjinalDosyaBoyutu);
    printf("Sikistirma orani: [%.2f%%]\n",sikistirilmisYuzde);
    printf("\n---------------------------------------\n");

}

void ReadFile()
{
    for(int i = 0; i<strlen(text);i++)
    {
        text[i] = "\0";
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
    char harf;
};
void encode(int offset,int lenght,char character,FILE *encodeFile)
{

    //printf("<%d,%d,C(%c)>\n",offset,lenght,character);
    offset = offset<<4;
    offset += lenght;


    struct encoNode temp;
    temp.harf = character;
    temp.offsetLen = offset;

    //printf("Dosya Bicimi: \t<%d,C(%c)> \n",offset,character);
    //fprintf(dosya,"%c%c",offset,(unsigned char)character);
    fwrite(&temp,sizeof(temp),1,encodeFile);

}
int main()
{
    setlocale(LC_ALL, "Turkish");
    system("cls");
    int secim = 0;

    do{
        printf("\e[?25h");
        printf("\n\n------------------\n-------MENU-------\n------------------\n");
        printf("---- 1-) LZ77 ----\n");
        printf("---- 2-) DEFLATE -\n");
        printf("---- 9-) Cikis ---\n");
        printf("------------------\n");
        printf("Secim Yapiniz: ");
        scanf("%d",&secim);
        switch(secim)
        {
        case 1:
            LZ77();
            break;
        case 2:
            printf("---DEFLATE---");
            break;
        case 9:
            return 0;
            break;
        }


    }while(secim != 9);
    return 0;
}
