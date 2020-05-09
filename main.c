#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
/** Sabitler */
#define dosyaOkumaBoyutu 10000000
#define true 1
#define false 0

/** Degiskenler */
char text[dosyaOkumaBoyutu];

/** Fonksiyonlar */
void LZ77();
void ReadFile();

/*

searchBuffer  -> 8
forwordBuffer -> 6

*/



void encode(int offset,int lenght,char character,FILE *dosya)
{
    fprintf(dosya,"%c,%c,%c\n",(unsigned char)offset,(unsigned char)lenght,(unsigned char)character);
}


void LZ77()
{

    FILE *encodeFile;
    encodeFile = fopen("metin.lz77","ab+");

    /**
              8                 6               XXX
        |**SEARCH**|------|**FORWARD**|-------|**DEVAMI**||
    */
    int pointer = 0;
    while(pointer < strlen(text))
    {

        int i= pointer -1;
        if(i == -1)
        {
            //printf("<0,0,%c>\n",text[pointer]);
            encode(0,0,text[pointer],encodeFile);
            pointer++;
        }
        else
        {
            int harfVarMi = false;
            while (i >= 0)
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
                //printf("<0,0,%c>\n",text[pointer]);
                encode(0,0,text[pointer],encodeFile);
                pointer++;
            }
            else
            {
                int i = pointer-1,hafizaBenzerlik = 0,hangisindenItibaren = 0;

                while(i >= 0)
                {
                    if(text[i] == text[pointer])
                    {
                        //printf("%d ",i); Benzerlik indisleri
                        int benzerlik = 0;
                        while(text[i+benzerlik] == text[pointer+benzerlik])
                        {
                            benzerlik++;
                            //printf("asd");

                        }
                        if(benzerlik > hafizaBenzerlik)
                        {
                            hafizaBenzerlik = benzerlik;
                            hangisindenItibaren = i;
                        }
                    }
                    i--;
                }


                if(strlen(text)-1 < pointer+hafizaBenzerlik)
                    {
                       // printf("<%d,%d,%c>\n",(pointer-hangisindenItibaren),hafizaBenzerlik,text[pointer+hafizaBenzerlik-1]);
                        encode((pointer-hangisindenItibaren),hafizaBenzerlik,text[pointer+hafizaBenzerlik-1],encodeFile);
                    }
                else
                    {
                      //  printf("<%d,%d,%c>\n",(pointer-hangisindenItibaren),hafizaBenzerlik,text[pointer+hafizaBenzerlik]);
                        encode((pointer-hangisindenItibaren),hafizaBenzerlik,text[pointer+hafizaBenzerlik],encodeFile);
                    }
                pointer += hafizaBenzerlik +1;
                //printf("Harf: %c | Nerede: %d | hangisindenItibaren %d | benzerlikUzunlugu: %d\n",text[pointer],pointer,hangisindenItibaren,hafizaBenzerlik);
            }
        }
    }
    fclose(encodeFile);
}
    void ReadFile()
    {
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
    int main()
    {
        ReadFile();
        //puts(text);
        system("cls");
        printf("Harf sayisi: %d Encode ediliyor...\n",strlen(text));
        LZ77();
        printf("Encode islemi tamamlandi...\n");
        return 0;
    }
