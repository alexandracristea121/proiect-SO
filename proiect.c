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
    int desc[2];
    int pipeFisier[2];

    fisierPrincipal = malloc(sizeof(struct Fisier)); //alocare dinamica pt structura
    if(fisierPrincipal == NULL) {
        printf("Eroare fisier");
        exit(-1);
    }

    //vom avea 4 arg : nume program, fis input, fis output <c>
    if(argc != 4) {   
        printf("Usage ./program <input_folder> <output_folder> <c>\n");
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
        perror("Eroare la citire dir:");
        exit(-2);
    }
    
    //deschidere pipe
    if(pipe(desc) == -1) {
        perror("Creare pipe esuata");
        exit(-1);
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
            
            //deschid pipe pt a trm catre copil continutul
            if(pipe(pipeFisier) == -1) {
                perror("Creare pipe esuata");
                exit(-1);
            } 

            pid = fork();
            //proces copil
            if(pid == 0) {
                close(desc[0]); //inchid citire
                close(desc[1]); //inchid scriere
                close(pipeFisier[0]); 

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
                        scrieNumarLinii(path, pipeFisier[1]);
                        close(pipeFisier[1]);
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
                close(pipeFisier[1]); //inchid scriere
                exit(liniiScrise);
            }
            
            //daca e BMP => alt proces care sa schimbe poza in nuante de gri
            if(esteBMP(path) == 1 && pid != 0) {
                pid = fork();
                //proces copil
                if(pid == 0) {
                    close(desc[0]); //inchid citire
                    close(desc[1]); //inchid scriere
                    close(pipeFisier[0]); 
                    close(pipeFisier[1]); 
                    exit(transformaBMPInGrayscale(path));
                }
            }
            
            //daca nu e fisier bmp facem alt proces si verificam cate propozitii contin caracterul c
            else if(esteFisierObisnuit(path) == 1 && pid != 0) {
                pid = fork();
                if(pid == 0) {
                    close(pipeFisier[1]); //inchid scriere
                    close(desc[0]);
            
                    dup2(pipeFisier[0], 0); // redirectare stdin la pipeFisier[0]
                    dup2(desc[1], 1);   // redirectare stdout la desc[1]

                    close(desc[1]); //inchid scriere
                    close(pipeFisier[0]); //inchid citire
                    execl("./script.sh", "./script.sh", argv[3], NULL);
                
                    close(desc[1]);
                    close(pipeFisier[0]); //inchid citire
                    exit(0);
                }
            }
        }
        directorCurent = readdir(director);
    } while(directorCurent != NULL);

    closedir(director);

    //proces parinte
    close(desc[1]); //inchid scriere
    int status, pid_copil;
    if(pid != 0) {
        while((pid_copil = wait(&status)) >= 0) {
            if(WEXITSTATUS(status) > 1) printf("Procesul cu pid-ul = %d s-a terminat cu codul: %d\n", pid_copil, WEXITSTATUS(status));
            else printf("Procesul cu pid-ul = %d s-a terminat cu exit code-ul: %d\n", pid_copil, WEXITSTATUS(status));
        }
	}

    
    //citeste pipe-ul
    char pipe[16];
    char propozitiiCorecte = 0;
    int sum = 0;
    while((read(desc[0], &pipe, 4)) == 4) {  
        propozitiiCorecte = atoi(pipe);
        if(propozitiiCorecte >= 0) sum = sum + propozitiiCorecte;    
    }
    
    printf("%d propozitii corecte care contin caracterul: '%c'\n", sum, argv[3][0]);
    close(desc[0]); //inchid citire
    
    return 0;
}
