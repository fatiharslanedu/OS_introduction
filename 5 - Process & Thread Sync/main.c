/*
    @author Fatih Arslan
    @no 152120161003
    İşletim Sistemleri Lab - Uyg5 / 20-21 Second Term
    POP-OS 20.10 
*/

/*
    Programda multi-thread, multi-semaphore yapıları kullanılmıştır.
    Tüm threadler arası iletişim sağlanmıştır.
    Exacute process: gcc -pthread -o 161003_uyg5A 161003_uyg5A.c
    After exacute: ./161003_uyg5A
*/

/*Kütüphane tanımlamaları*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#define YIYECEK_SAYISI 13
#define TUR_SAYISI 5

int i = 1;
int counter = 0;
int remainder1 = 12;
int yiyecek_sayisi = 0;

char *YIYECEKLER[YIYECEK_SAYISI] = {"Mercimek", "Pirinç", "Çilek", "Muz", "Camasir Suyu",
                                    "Deterjan", "Baklava", "Kadayif", "Kunefe", "Kofte", "Tavuk", "Balik",
                                    "Kivi"};


char *TUR[TUR_SAYISI] = {"meyve", "temizlik", "et", "baklagil", "tatli"}; /* Türler */

/*İletişim için gerekli olan semaphore tanımlamaları*/
sem_t meyve_temizlik, baklagil_meyve, tatli_et, et_meyve, temizlik_tatli, baklagil_tatli;

void printFunc(int); /* Ön tanımlamalar*/
void finishStatus(int);

/*Thread fonskiyonları*/
void *Meyve(void *args)
{

    sem_wait(&baklagil_meyve); // set to 0 for lock, baklagil thread'i ile iletişim
    printFunc(0); // Print Çilek
    printFunc(0); // Print Muz
    sleep(0.5);
    sem_post(&meyve_temizlik); // set to 1 for unlock
    counter++;
    sem_wait(&et_meyve); // set to 0 for lock, et thread'i ile iletişim
    printFunc(0);
    counter++;
    finishStatus(0); // Print finish message.
}

void *Temizlik(void *args)
{

    sem_wait(&meyve_temizlik); // set to 0 for lock, meyve thread'i ile iletişim
    printFunc(1);
    sleep(0.5);
    printFunc(1);
    finishStatus(1); // Print finish message.
    counter++;
    sem_post(&temizlik_tatli);  // set to 1 for unlock
}

void *Et(void *args)
{
    sem_wait(&tatli_et); // set to 0 for lock, tatli thread'i ile iletişim
    printFunc(2);
    sleep(0.5);
    printFunc(2);
    
    printFunc(2);
    finishStatus(2); // Print finish message.
    counter++;
    sem_post(&et_meyve); // set to 1 for unlock
}

void *Baklagil(void *args)
{
    
    printFunc(3);
    printFunc(3);
    sleep(0.5);
    finishStatus(3); // Print finish message.
    sem_post(&baklagil_meyve); // set to 1 for unlock
    counter++;
}

void *Tatli(void *args)
{
    
    sem_wait(&temizlik_tatli); // set to 0 for lock, temizlik thread'i ile iletişim
    printFunc(4);
    printFunc(4);
    sleep(0.5);
    printFunc(4);
    finishStatus(4); // Print finish message.
    counter++;
    sem_post(&tatli_et); // set to 1 for unlock
}

/* Check the status for finish product and print result the screen. */
void finishStatus(int status)
{
    if (status == 0)
    {
        printf("[UYARI] Meyve ürünlerinin tümü alındı !!\n");
    }
    if (status == 1)
    {
        printf("[UYARI] Temizlik ürünlerinin tümü alındı !!\n");
    }
    if (status == 2)
    {
        printf("[UYARI] Et ürünlerinin tümü alındı !!\n");
    }
    if (status == 3)
    {
        printf("[UYARI] Baklagil ürünlerinin tümü alındı !!\n");
    }
    if (status == 4)
    {
        printf("[UYARI] Tatli ürünlerinin tümü alındı !!\n");
    }
}
/*Ana yazdırma fonskiyonu; tür ve yiyecekler için. gerekli formata göre uyarlandı.*/
void printFunc(int tur_sayisi)
{
    if (remainder1 <= 6 && remainder1 >= 4)
    {
        printf("%3d. (%s) %s\t\t--> Kalan oge sayisi: %d\n", i++, TUR[tur_sayisi], YIYECEKLER[yiyecek_sayisi++], remainder1--);
    }
    else if (remainder1 <= 3 && remainder1 >= 1)
    {
        printf("%3d. (%s) %s\t\t\t--> Kalan oge sayisi: %d\n", i++, TUR[tur_sayisi], YIYECEKLER[yiyecek_sayisi++], remainder1--);
    }
    else if (remainder1 == 0)
    {
        printf("%3d. (%s) %s\t\t--> Goreve hazirim.\n", i++, TUR[tur_sayisi], YIYECEKLER[yiyecek_sayisi++]);
    }
    else
    {
        printf("%3d. (%s) %s\n", i++, TUR[tur_sayisi], YIYECEKLER[yiyecek_sayisi++]);
        remainder1--;
    }
}

int main(int argc, char *argv[])
{

    pthread_t th_Meyve, th_Temizlik, th_Et, th_Baklagil, th_tatli; /*5 adet threadimiz*/
    pthread_attr_t detachedThread;
    pthread_attr_init(&detachedThread);
    pthread_attr_setdetachstate(&detachedThread, PTHREAD_CREATE_DETACHED); /*join yapmamızı attribute yolu ile sağladı.*/

    sem_init(&baklagil_meyve, 0, 0); /*semaphorelera initialize edildi.*/
    sem_init(&meyve_temizlik, 0, 0);    /*Semaphore ilk parametre olarak tanımlamanın referans degerini alıyor.*/
    sem_init(&temizlik_tatli, 0, 0);    /*2. parametre 0 olarak atadık çünkü bir process var oda main process.*/
    sem_init(&tatli_et, 0, 0);          /*3. parametre olarak 0 atandı oda semaphorenun baslangıc degerini gösteriyor.*/
    sem_init(&et_meyve, 0, 0);
    sem_init(&baklagil_tatli, 0, 0);
    printf("-->> Başlangıç!!\n");
    int i;

    if (pthread_create(&th_Meyve, &detachedThread, &Meyve, NULL) != 0) /*Tüm threadler olusamamam semaryosu check edilip ona göre oluşturuldu.*/
    {
        perror("Failed to create Meyve thread.");
    }

    if (pthread_create(&th_Temizlik, &detachedThread, &Temizlik, NULL) != 0)
    {
        perror("Failed to create Temizlik thread.");
    }

    if (pthread_create(&th_Et, &detachedThread, &Et, NULL) != 0)
    {
        perror("Failed to create Et thread.");
    }

    if (pthread_create(&th_Baklagil, &detachedThread, &Baklagil, NULL) != 0)
    {
        perror("Failed to create Baklagil thread.");
    }

    if (pthread_create(&th_tatli, &detachedThread, &Tatli, NULL) != 0)
    {
        perror("Failed to create Tatli thread.");
    }

    sem_destroy(&meyve_temizlik); /*semaphorelera yok edildi.*/
    sem_destroy(&tatli_et);
    sem_destroy(&et_meyve);
    sem_destroy(&temizlik_tatli);
    sem_destroy(&baklagil_meyve);
    sem_destroy(&baklagil_tatli);

    pthread_attr_destroy(&detachedThread);
    pthread_exit(0);
    return 0;
}
