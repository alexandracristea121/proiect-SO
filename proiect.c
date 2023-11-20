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
#include <sys/wait.h>
#include "librarie.h"

int main(int argc, char **argv) {
    
    int fisierOutput;
    struct Fisier *fisierPrincipal = NULL;
    struct dirent *directorCurent = NULL;
    DIR *director = NULL;
    int nrProcese = 0;

    fisierPrincipal = malloc(sizeof(struct Fisier)); //alocare dinamica pt structura
    if(fisierPrincipal == NULL) {
        printf("Eroare fisier");
        exit(-1);
    }

    //vom avea 3 arg : nume program, fis input, fis output
    if(argc != 3) {   
        printf("Usage ./program <input_folder> <output_folder>\n");
        exit(-1);
    }

    //verificare ca arg[1] e dir
    if(!esteDirector(argv[1])) {
        printf("Argumentul trebuie sa fie director");
        exit(-1);
    }

    // verificare ca directorul exista, daca nu => il creez
    struct stat directorOutput;
    if (stat(argv[2], &directorOutput) == -1) {      //verific ca exista director
        if (mkdir(argv[2], 0777) != 0) {             // il creez
            perror("Eroare la creearea directorului");
            exit(errno);
        } 
    }

    //deschidere dir
    director = opendir(argv[1]);
    if(director == NULL) {
        perror("Error la citire dir:");
        exit(-2);
    }

    // iteram prin tot ce este in director
    pid_t pid;
    directorCurent = readdir(director);
    do{
        // facem path-ul fisierului curent
        char path[128];
        strcpy(path, argv[1]);
        strcat(path, "/");
        strcat(path, directorCurent->d_name);
        strcpy(fisierPrincipal->numeFisier, directorCurent->d_name);

        //trecem peste directorul curent si cel parinte
        if(directorCurent->d_name[0] != '.') {

            nrProcese++;
            pid = fork();
            //proces copil
            if(pid == 0) {

                // creez fisier output
                char numeFisierOutput[128];
                strcpy(numeFisierOutput, argv[2]);
                strcat(numeFisierOutput, "/");
                strcat(numeFisierOutput, directorCurent -> d_name);
                strcat(numeFisierOutput, "_statistica.txt");
                fisierOutput = open(numeFisierOutput, O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR | S_IRUSR  | S_IXUSR);
                if(fisierOutput == -1) {
                    perror("Eroare la deschiderea fisierului output");
                    exit(errno);
                }

                int liniiScrise = 0;
                if(esteBMP(path) == 1) {
                    if((citireInformatieBMP(path, fisierPrincipal)) == 1)
                        liniiScrise = scriereInformatieBMP(fisierOutput, fisierPrincipal);
                }
                else if(esteFisierObisnuit(path) == 1) {
                    if((citireFisierObisnuit(path, fisierPrincipal)) == 1){
                        liniiScrise = scriereInformatieFisierObisnuit(fisierOutput, fisierPrincipal);
                    }
                }
                else if(esteDirector(path) == 1){
                    if((citireInformatieDirector(path, fisierPrincipal)) == 1)
                        liniiScrise = scriereInformatieDirector(fisierOutput, fisierPrincipal);
                }
                else if(esteLink(path) == 1){
                    if((citireInformatieLink(path, fisierPrincipal)) == 1)
                        liniiScrise = scriereInformatieLink(fisierOutput, fisierPrincipal);
                }
                close(fisierOutput);
                exit(liniiScrise);
            }
            
            //daca e BMP => alt proces care sa schimbe poza in nuante de gri

            if(esteBMP(path) == 1 && pid != 0) {
                nrProcese++;
                pid = fork();
                //proces copil
                if(pid == 0) exit(transformaBMPInGrayscale(path));
            }
        }
        directorCurent = readdir(director);
    } while(directorCurent != NULL);

    closedir(director);

    //proces parinte
    if(pid != 0) {
        for(int i = 0; i < nrProcese; i++) {  //procesul parinte asteapta terminarea tuturor proceselor copil 
            int status, procesCopil;
            procesCopil = wait(&status);
            if(WEXITSTATUS(status) > 1) printf("Procesul cu pid = %d s-a terminat cu codul: %d (linii scrise)\n", procesCopil, WEXITSTATUS(status));
            else printf("Procesul cu pid = %d s-a terminat cu codul: %d\n", procesCopil, WEXITSTATUS(status));
        }
    }
    return 0;
}