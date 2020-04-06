#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#define dosyaOkumaBoyutu 1000000
char a[dosyaOkumaBoyutu];
void LZ77();
void DEFLATE();
void ReadFile();

void LZ77()
{
    printf("%s", a);

}
void DEFLATE()
{
    printf("\nDEFLATE\n");
}
void ReadFile()
{
    FILE *dosya;
    if ((dosya = fopen("metin.txt", "r")) == NULL)
    {
        printf("Dosya bulunamadi...");
        system("exit");
    }
    else
    {
        for (int i = 0; i < dosyaOkumaBoyutu; i++)
        {
            fscanf(dosya, "%c", &a[i]);
        }
    }

}



int main()

{
    setlocale(LC_ALL, "Turkish");
    int menuInput = -1;
    ReadFile();

    printf("\n\t#MENU#\n");
    printf("#1 -) LZ77 Algorýtmasi\n");
    printf("#2 -) DEFLATE Algoritmasi\n");
    printf("#0 -) CIKIS\n");
    printf("#...DEF: 1");
    LZ77();



    return 0;

}
