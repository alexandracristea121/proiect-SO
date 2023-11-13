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

// structura fisier care contine toate info
struct Fisier {
    char numeFisier[128];
    uint32_t dim;
    u_int32_t inaltime;
    u_int32_t latime;
    uint32_t dimensiuneReferintaFisier;
    uid_t uid;
    mode_t mod;
    __time_t ultimaModificare;
    nlink_t nrLinkuri;
};

int verificareFisierBMP(char *numeFisier) {

    struct stat informatiiFisier;
    if(lstat(numeFisier, &informatiiFisier) == -1) return -1;
    
    //verific daca path-ul este un fisier normal si nu dir sau link
    if(!S_ISREG(informatiiFisier.st_mode)) return 0;
    
    int fisierBMP = open(numeFisier, O_RDONLY);
    if(fisierBMP == -1) return -1;
    
    //un fisier bmp are 2 biti 'b' si 'm'
    char semnatura[2];
    if(read(fisierBMP, semnatura, 2) != 2) {
        close(fisierBMP);
        return -1;
    }

    //daca contine bm returnam info fisier
    if(semnatura[0] == 'B' && semnatura[1] == 'M') {
        close(fisierBMP);
        return 1;
    }

    close(fisierBMP);
    return 0;
}

int verificareLink(char *numeFisier) {
    struct stat informatiiFisier;
    if(lstat(numeFisier, &informatiiFisier) == -1) return -1;
    
    if (S_ISLNK(informatiiFisier.st_mode)) {
        return 1;
    } else {
        return 0;
    }
}

int verificareDir(char *numeFisier) {
    struct stat informatiiFisier;
    if(lstat(numeFisier, &informatiiFisier) == -1) return -1;

    if (S_ISDIR(informatiiFisier.st_mode)) {
        return 1;
    } else {
        return 0;
    }
}

//citire din fisier de la o anumita pozitie
int citesteDinFisier(int fisier, void *buffer, int number_of_bytes, int cursor_position) {
    if (lseek(fisier, cursor_position, SEEK_SET) == -1) return -1;
    if (read(fisier, buffer, number_of_bytes) == -1) return -1;

    return 1;
}

int scriereInformatiiBMP(int fisier, struct Fisier *fisierPrincipal) {

    char buffer[512];
    char permisiuniUser[4];
    char permisiuniGrup[4];
    char altePermisiuni[4];

    //aici extragem user,grup si celelelate
    mode_t mod = fisierPrincipal->mod;
    sprintf(permisiuniUser, "%c%c%c",
           (mod & S_IRUSR) ? 'r' : '-',
           (mod & S_IWUSR) ? 'w' : '-',
           (mod & S_IXUSR) ? 'x' : '-');

    sprintf(permisiuniGrup, "%c%c%c",
           (mod & S_IRGRP) ? 'r' : '-',
           (mod & S_IWGRP) ? 'w' : '-',
           (mod & S_IXGRP) ? 'x' : '-');

    sprintf(altePermisiuni, "%c%c%c",
           (mod & S_IROTH) ? 'r' : '-',
           (mod & S_IWOTH) ? 'w' : '-',
           (mod & S_IXOTH) ? 'x' : '-');

    int lungimeFis = sprintf(buffer, 
        "Nume fisier: %s\n"
        "inaltime: %d\n"
        "latime: %d\n"
        "dim: %d bytes\n"
        "UID: %u\n"
        "ultima modif modified: %s"
        "links: %lu\n"
        "user: %s\n"
        "group: %s\n"
        "others %s\n\n",
            fisierPrincipal->numeFisier, 
            fisierPrincipal->inaltime, 
            fisierPrincipal->latime, 
            fisierPrincipal->dim, 
            fisierPrincipal->uid,
            ctime(&fisierPrincipal->ultimaModificare),
            fisierPrincipal->nrLinkuri,
            permisiuniUser,
            permisiuniGrup,
            altePermisiuni
            );

    if(write(fisier, buffer, lungimeFis) == -1) return -1;
   
    return 1;
}

int scriereInformatiiDirector(int fisier, struct Fisier *fisierPrincipal) {
    char buffer[512];
    char permisiuniUser[4];
    char permisiuniGrup[4];
    char altePermisiuni[4];

    //extract the user, group and others rights from the file mod
    mode_t mod = fisierPrincipal->mod;
    sprintf(permisiuniUser, "%c%c%c",
           (mod & S_IRUSR) ? 'r' : '-',
           (mod & S_IWUSR) ? 'w' : '-',
           (mod & S_IXUSR) ? 'x' : '-');

    sprintf(permisiuniGrup, "%c%c%c",
           (mod & S_IRGRP) ? 'r' : '-',
           (mod & S_IWGRP) ? 'w' : '-',
           (mod & S_IXGRP) ? 'x' : '-');

    sprintf(altePermisiuni, "%c%c%c",
           (mod & S_IROTH) ? 'r' : '-',
           (mod & S_IWOTH) ? 'w' : '-',
           (mod & S_IXOTH) ? 'x' : '-');

    int lungimeFis = sprintf(buffer, 
        "Fisier: %s\n"
        "UID: %u\n"
        "user: %s\n"
        "group: %s\n"
        "others %s\n\n",
            fisierPrincipal->numeFisier, 
            fisierPrincipal->uid,
            permisiuniUser,
            permisiuniGrup,
            altePermisiuni
            );

    if(write(fisier, buffer, lungimeFis) == -1) return -1;
   
    return 1;
}

int scriereInformatiiLink(int fisier, struct Fisier *fisierPrincipal) {
    char buffer[512];
    char permisiuniUser[4];
    char permisiuniGrup[4];
    char altePermisiuni[4];

    //extract the user, group and others rights from the file mod
    mode_t mod = fisierPrincipal->mod;
    sprintf(permisiuniUser, "%c%c%c",
           (mod & S_IRUSR) ? 'r' : '-',
           (mod & S_IWUSR) ? 'w' : '-',
           (mod & S_IXUSR) ? 'x' : '-');

    sprintf(permisiuniGrup, "%c%c%c",
           (mod & S_IRGRP) ? 'r' : '-',
           (mod & S_IWGRP) ? 'w' : '-',
           (mod & S_IXGRP) ? 'x' : '-');

    sprintf(altePermisiuni, "%c%c%c",
           (mod & S_IROTH) ? 'r' : '-',
           (mod & S_IWOTH) ? 'w' : '-',
           (mod & S_IXOTH) ? 'x' : '-');

    int lungimeFis = sprintf(buffer, 
        "Link name: %s\n"
        "Link dim: %d bytes\n"
        "Target file dim: %d bytes\n"
        "UID: %u\n"
        "Rights for user: %s\n"
        "Rights for group: %s\n"
        "Rights for others %s\n\n",
            fisierPrincipal->numeFisier, 
            fisierPrincipal->dim,
            fisierPrincipal->dimensiuneReferintaFisier, 
            fisierPrincipal->uid,
            permisiuniUser,
            permisiuniGrup,
            altePermisiuni
            );

    if(write(fisier, buffer, lungimeFis) == -1) return -1;
   
    return 1;
}

int citireInformatiiBMP(char *numeFisier, struct Fisier *fisierPrincipal) {

    struct stat informatiiFisier;
    int fisierBMP;

    if(lstat(numeFisier, &informatiiFisier) == -1) return -1;
    
    fisierBMP = open(numeFisier, O_RDONLY);
    if(fisierBMP == -1) return -1;
    
    
    //setez numeFisier
    strcpy(fisierPrincipal->numeFisier, numeFisier);

    //latime
    if(!citesteDinFisier(fisierBMP, &fisierPrincipal->latime , sizeof(fisierPrincipal->latime), 18)) {
        close(fisierBMP);
        return -1;
    }

    //inaltime
    if(!citesteDinFisier(fisierBMP, &fisierPrincipal->inaltime, sizeof(fisierPrincipal->inaltime), 22)) {
        close(fisierBMP);
        return -1;
    }

    //dim
    if(!citesteDinFisier(fisierBMP, &fisierPrincipal->dim, sizeof(fisierPrincipal->dim), 2)) {
        close(fisierBMP);
        return -1;
    }

    fisierPrincipal->nrLinkuri = informatiiFisier.st_nlink;
    fisierPrincipal->uid = informatiiFisier.st_uid;
    fisierPrincipal->ultimaModificare = informatiiFisier.st_mtime;
    fisierPrincipal->mod = informatiiFisier.st_mode;

    return 1;
}

int citireInformatiiDir(char *numeFisier, struct Fisier *fisierPrincipal) {

    struct stat informatiiFisier;
    if(lstat(numeFisier, &informatiiFisier) == -1) return -1;

    strcpy(fisierPrincipal->numeFisier, numeFisier);
    fisierPrincipal->uid = informatiiFisier.st_uid;
    fisierPrincipal->mod = informatiiFisier.st_mode;
    fisierPrincipal->dim = informatiiFisier.st_size;

    return 1;
}

int citireInformatiiLink(char *numeFisier, struct Fisier *fisierPrincipal) {

    struct stat informatiiFisier;
    if(lstat(numeFisier, &informatiiFisier) == -1) return -1;

    strcpy(fisierPrincipal->numeFisier, numeFisier);
    fisierPrincipal->uid = informatiiFisier.st_uid;
    fisierPrincipal->dim = informatiiFisier.st_size;
    fisierPrincipal->mod = informatiiFisier.st_mode;

    if(stat(numeFisier, &informatiiFisier) == -1) return -1;
    fisierPrincipal->dimensiuneReferintaFisier = informatiiFisier.st_size;

    return 1;
}

int main(int argc, char **argv) {
    
    int fisierOutput;
    struct Fisier *fisierPrincipal = NULL;
    struct dirent *directorCurent = NULL;
    DIR *director = NULL;

    fisierPrincipal = malloc(sizeof(struct Fisier));
    if(fisierPrincipal == NULL) {
        printf("Error allocating space for file information");
        exit(-1);
    }

    if(argc != 2) {   
        printf("Usage ./program <input_folder>\n");
        exit(-1);
    }

    //verificare ca arg e fisier
    if(!verificareDir(argv[1])) {
        printf("Argument must be a folder");
        exit(-1);
    }

    //fis output
    fisierOutput = open("statistica.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR | S_IRUSR  | S_IXUSR);
    if(fisierOutput == -1) {
        perror("Error creating output file");
        exit(-2);
    }
    
    //fis input
    director = opendir(argv[1]);
    if(director == NULL) {
        perror("Error opening dir:");
        exit(-2);
    }

    //parcurgem dir
    directorCurent = readdir(director);
    do{
        char numeFisier[128];
        strcpy(numeFisier, argv[1]);
        strcat(numeFisier, "/");
        strcat(numeFisier, directorCurent->d_name);

        if(directorCurent->d_name[0] != '.') {
            if(verificareFisierBMP(numeFisier) == 1) {
                if((citireInformatiiBMP(numeFisier, fisierPrincipal)) == 1)
                    scriereInformatiiBMP(fisierOutput, fisierPrincipal);
            }
            if(verificareDir(numeFisier) == 1){
                if((citireInformatiiDir(numeFisier, fisierPrincipal)) == 1)
                    scriereInformatiiDirector(fisierOutput, fisierPrincipal);
            }
            if(verificareLink(numeFisier) == 1){
                if((citireInformatiiLink(numeFisier, fisierPrincipal)) == 1)
                    scriereInformatiiLink(fisierOutput, fisierPrincipal);
            }
        }
        directorCurent = readdir(director);
    } while(directorCurent != NULL);

    closedir(director);
    close(fisierOutput);

    return 0;
}