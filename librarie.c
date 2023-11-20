#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/errno.h>
#include "librarie.h"

//functie auxiliara de citire din fisier
int citesteDinFisier(int fisier, void *buffer, int numarBiti, int pozitieCursor) {
    if (lseek(fisier, pozitieCursor, SEEK_SET) == -1) return -1;    //setez pozitia cursorului in fisier la pozitieCursor
    if (read(fisier, buffer, numarBiti) == -1) return -1;
    return 1;
}

int scriereInformatieBMP(int fisier, struct Fisier *fisierInformatii) {

    char buffer[512];
    char permisiuniUser[4];
    char permisiuniGrup[4];
    char permisiuniOther[4];

    //extragere permisiuni
    mode_t mode = fisierInformatii->mode;
    sprintf(permisiuniUser, "%c%c%c",
           (mode & S_IRUSR) ? 'r' : '-',
           (mode & S_IWUSR) ? 'w' : '-',
           (mode & S_IXUSR) ? 'x' : '-');

    sprintf(permisiuniGrup, "%c%c%c",
           (mode & S_IRGRP) ? 'r' : '-',
           (mode & S_IWGRP) ? 'w' : '-',
           (mode & S_IXGRP) ? 'x' : '-');

    sprintf(permisiuniOther, "%c%c%c",
           (mode & S_IROTH) ? 'r' : '-',
           (mode & S_IWOTH) ? 'w' : '-',
           (mode & S_IXOTH) ? 'x' : '-');

    int length = sprintf(buffer, 
        "Nume fisier: %s\n"
        "Inaltime: %d\n"
        "Latime: %d\n"
        "Dimensiune: %d bytes\n"
        "UID: %u\n"
        "Ultima modificare: %s"
        "Nr links: %lu\n"
        "Drepturi user: %s\n"
        "Drepturi grup: %s\n"
        "Drepturi others %s",
            fisierInformatii->numeFisier, 
            fisierInformatii->inaltime, 
            fisierInformatii->latime, 
            fisierInformatii->size, 
            fisierInformatii->user_id,
            ctime(&fisierInformatii->last_modified),
            fisierInformatii->nrLink,
            permisiuniUser,
            permisiuniGrup,
            permisiuniOther
            );

    if(write(fisier, buffer, length) == -1) return -1;
    
    
    return 10; //return nr linii citite
}

int scriereInformatieFisierObisnuit(int fisier, struct Fisier *fisierInformatii) {
    char buffer[512];
    char permisiuniUser[4];
    char permisiuniGrup[4];
    char permisiuniOther[4];

    mode_t mode = fisierInformatii->mode;
    sprintf(permisiuniUser, "%c%c%c",
           (mode & S_IRUSR) ? 'r' : '-',
           (mode & S_IWUSR) ? 'w' : '-',
           (mode & S_IXUSR) ? 'x' : '-');

    sprintf(permisiuniGrup, "%c%c%c",
           (mode & S_IRGRP) ? 'r' : '-',
           (mode & S_IWGRP) ? 'w' : '-',
           (mode & S_IXGRP) ? 'x' : '-');

    sprintf(permisiuniOther, "%c%c%c",
           (mode & S_IROTH) ? 'r' : '-',
           (mode & S_IWOTH) ? 'w' : '-',
           (mode & S_IXOTH) ? 'x' : '-');

    int length = sprintf(buffer, 
        "Nume fisier: %s\n"
        "Size fisier: %d bytes\n"
        "UID: %u\n"
        "Ultima modificare: %s"
        "Numar links: %lu\n"
        "Drepturi user: %s\n"
        "Drepturi grup: %s\n"
        "Drepturi others %s",
            fisierInformatii->numeFisier, 
            fisierInformatii->size, 
            fisierInformatii->user_id,
            ctime(&fisierInformatii->last_modified),
            fisierInformatii->nrLink,
            permisiuniUser,
            permisiuniGrup,
            permisiuniOther
            );

    if(write(fisier, buffer, length) == -1) return -1;
    
    return 8;
}

int scriereInformatieDirector(int fisier, struct Fisier *fisierInformatii) {
    char buffer[512];
    char permisiuniUser[4];
    char permisiuniGrup[4];
    char permisiuniOther[4];

    mode_t mode = fisierInformatii->mode;
    sprintf(permisiuniUser, "%c%c%c",
           (mode & S_IRUSR) ? 'r' : '-',
           (mode & S_IWUSR) ? 'w' : '-',
           (mode & S_IXUSR) ? 'x' : '-');

    sprintf(permisiuniGrup, "%c%c%c",
           (mode & S_IRGRP) ? 'r' : '-',
           (mode & S_IWGRP) ? 'w' : '-',
           (mode & S_IXGRP) ? 'x' : '-');

    sprintf(permisiuniOther, "%c%c%c",
           (mode & S_IROTH) ? 'r' : '-',
           (mode & S_IWOTH) ? 'w' : '-',
           (mode & S_IXOTH) ? 'x' : '-');

    int length = sprintf(buffer, 
        "NUme director: %s\n"
        "UID: %u\n"
        "Drepturi user: %s\n"
        "Drepturi grup: %s\n"
        "Drepturi others %s",
            fisierInformatii->numeFisier, 
            fisierInformatii->user_id,
            permisiuniUser,
            permisiuniGrup,
            permisiuniOther
            );

    if(write(fisier, buffer, length) == -1) return -1;
    
    return 5;
}

int scriereInformatieLink(int fisier, struct Fisier *fisierInformatii) {
    char buffer[512];
    char permisiuniUser[4];
    char permisiuniGrup[4];
    char permisiuniOther[4];

    mode_t mode = fisierInformatii->mode;
    sprintf(permisiuniUser, "%c%c%c",
           (mode & S_IRUSR) ? 'r' : '-',
           (mode & S_IWUSR) ? 'w' : '-',
           (mode & S_IXUSR) ? 'x' : '-');

    sprintf(permisiuniGrup, "%c%c%c",
           (mode & S_IRGRP) ? 'r' : '-',
           (mode & S_IWGRP) ? 'w' : '-',
           (mode & S_IXGRP) ? 'x' : '-');

    sprintf(permisiuniOther, "%c%c%c",
           (mode & S_IROTH) ? 'r' : '-',
           (mode & S_IWOTH) ? 'w' : '-',
           (mode & S_IXOTH) ? 'x' : '-');

    int length = sprintf(buffer, 
        "Nume link: %s\n"
        "Size link: %d bytes\n"
        "Size fisier target: %d bytes\n"
        "UID: %u\n"
        "Drepturi user: %s\n"
        "Drepturi grup: %s\n"
        "Drepturi others %s",
            fisierInformatii->numeFisier, 
            fisierInformatii->size,
            fisierInformatii->dimensiuneFisierReferinta, 
            fisierInformatii->user_id,
            permisiuniUser,
            permisiuniGrup,
            permisiuniOther
            );

    if(write(fisier, buffer, length) == -1) return -1;
    
    return 7;
}

int citireInformatieBMP(char *numeFisier, struct Fisier *fisierInformatii) {

    struct stat statisiciFisier;
    int fisierBMP;

    if(lstat(numeFisier, &statisiciFisier) == -1) return -1;
    
    fisierBMP = open(numeFisier, O_RDONLY);
    if(fisierBMP == -1) return -1;
    
    //citire latime
    if(!citesteDinFisier(fisierBMP, &fisierInformatii->latime , sizeof(fisierInformatii->latime), 18)) {
        close(fisierBMP);
        return -1;
    }

    //citire inaltime
    if(!citesteDinFisier(fisierBMP, &fisierInformatii->inaltime, sizeof(fisierInformatii->inaltime), 22)) {
        close(fisierBMP);
        return -1;
    }

    //citire dimensiune fisier
    if(!citesteDinFisier(fisierBMP, &fisierInformatii->size, sizeof(fisierInformatii->size), 2)) {
        close(fisierBMP);
        return -1;
    }

    //restul informatiilor din fisier
    fisierInformatii->nrLink = statisiciFisier.st_nlink;
    fisierInformatii->user_id = statisiciFisier.st_uid;
    fisierInformatii->last_modified = statisiciFisier.st_mtime;
    fisierInformatii->mode = statisiciFisier.st_mode;

    return 1;
}

int citireInformatieDirector(char *numeFisier, struct Fisier *fisierInformatii) {

    struct stat statisiciFisier;
    if(lstat(numeFisier, &statisiciFisier) == -1) return -1;

    fisierInformatii->user_id = statisiciFisier.st_uid;
    fisierInformatii->mode = statisiciFisier.st_mode;
    fisierInformatii->size = statisiciFisier.st_size;

    return 1;
}

int citireFisierObisnuit(char *numeFisier, struct Fisier *fisierInformatii) {
    struct stat statisiciFisier;
    if(lstat(numeFisier, &statisiciFisier) == -1) return -1;

    fisierInformatii->user_id = statisiciFisier.st_uid;
    fisierInformatii->mode = statisiciFisier.st_mode;
    fisierInformatii->size = statisiciFisier.st_size;
    fisierInformatii->nrLink = statisiciFisier.st_nlink;
    fisierInformatii->last_modified = statisiciFisier.st_mtime;
    return 1;
}

int citireInformatieLink(char *numeFisier, struct Fisier *fisierInformatii) {
    
    struct stat statisiciFisier;
    if(lstat(numeFisier, &statisiciFisier) == -1) return -1;

    fisierInformatii->user_id = statisiciFisier.st_uid;
    fisierInformatii->size = statisiciFisier.st_size;
    fisierInformatii->mode = statisiciFisier.st_mode;

    if(stat(numeFisier, &statisiciFisier) == -1) return -1;
    fisierInformatii->dimensiuneFisierReferinta = statisiciFisier.st_size;

    return 1;
}

int esteBMP(char *numeFisier) {

    struct stat statisiciFisier;
    if(lstat(numeFisier, &statisiciFisier) == -1) return -1;
    
    //verific daca calea duce la un fisier normal
    if(!S_ISREG(statisiciFisier.st_mode)) return 0;
    
    int fisierBMP = open(numeFisier, O_RDONLY);
    if(fisierBMP == -1) return -1;
    

    char semnaturaFisier[2];
    if(read(fisierBMP, semnaturaFisier, 2) != 2) {
        close(fisierBMP);
        return -1;
    }
    
    //un fisier .bmp contine biti de B si M in header
    //daca contine acei biti returnam headerul
    if(semnaturaFisier[0] == 'B' && semnaturaFisier[1] == 'M') {
        close(fisierBMP);
        return 1;
    }

    close(fisierBMP);
    return 0;
}

int esteFisierObisnuit(char *numeFisier) {
    struct stat statisiciFisier;
    if(lstat(numeFisier, &statisiciFisier) == -1) return -1;

     //verific daca calea duce la un fisier normal
    if(S_ISREG(statisiciFisier.st_mode)) {
        return 1;
    } else {
        return 0;
    }
}

int esteLink(char *numeFisier) {

    struct stat statisiciFisier;
    if(lstat(numeFisier, &statisiciFisier) == -1) return -1;
    
    if (S_ISLNK(statisiciFisier.st_mode)) {
        return 1;
    } else {
        return 0;
    }
}

int esteDirector(char *numeFisier) {
    struct stat statisiciFisier;
    if(lstat(numeFisier, &statisiciFisier) == -1) return -1;

    if (S_ISDIR(statisiciFisier.st_mode)) {
        return 1;
    } else {
        return 0;
    }
}

int transformaBMPInGrayscale(char *numeFisier) {

    int dimensiuneBuffer = 900; //numarul de pixeli pe care il citim in buffer 
    u_int8_t pixelBuffer[dimensiuneBuffer];

    
    int fisierBMP = open(numeFisier, O_RDWR);
    if(fisierBMP == -1) return -1;

    //citesc data offset-ul
    uint32_t dataOffset;
    if(!citesteDinFisier(fisierBMP, &dataOffset, 4, 10)) {
        perror("Eroare:");
        close(fisierBMP);
        return -1;
    }   

    //setez cursorul la inceputul pixelilor
    if (lseek(fisierBMP, dataOffset, SEEK_SET) == -1) {
        close(fisierBMP);
        return -1;
    }

    int bitiCititi = read(fisierBMP, pixelBuffer, sizeof(pixelBuffer));
    while(bitiCititi > 0) {
        //mut cursorul la inceputul acestui grup de pixeli
        if (lseek(fisierBMP, -bitiCititi, SEEK_CUR) == -1) {
            perror("Error:");
            close(fisierBMP);
            return -1;
        }
        //transform pixelii
        for(int i = 0; i < bitiCititi; i = i + 3) {
            uint8_t red = pixelBuffer[i];
            uint8_t green = pixelBuffer[i + 1];
            uint8_t blue = pixelBuffer[i + 2];
            uint8_t gray_value = 0.299 * red + 0.587 * green + 0.114 * blue;
            pixelBuffer[i] = gray_value;
            pixelBuffer[i + 1] = gray_value;
            pixelBuffer[i + 2] = gray_value;
        }

        if(write(fisierBMP, pixelBuffer, bitiCititi) == -1) {
            perror("Error:");
            close(fisierBMP);
            return -1;
        }
        bitiCititi = read(fisierBMP, pixelBuffer, sizeof(pixelBuffer));
    }

    close(fisierBMP);
    return 1;
}