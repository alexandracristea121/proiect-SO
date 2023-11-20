#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#ifndef LIBRARIE_H
#define LIBRARIE_H

struct Fisier {
    char numeFisier[128];
    u_int32_t inaltime;
    u_int32_t latime;
    uint32_t size;
    uint32_t dimensiuneFisierReferinta;
    uid_t user_id;
    __time_t last_modified;
    nlink_t nrLink;
    mode_t mode;
};

int citesteDinFisier(int fisierDestinatie, void *buffer, int numarBiti, int pozitieCursor);
int scriereInformatieBMP(int fisierDestinatie, struct Fisier *fisierInput);
int scriereInformatieFisierObisnuit(int fisierDestinatie, struct Fisier *fisierInput);
int scriereInformatieDirector(int fisierDestinatie, struct Fisier *fisierInput);
int scriereInformatieLink(int fisierDestinatie, struct Fisier *fisierInput);
int citireInformatieBMP(char *numeFisier, struct Fisier *fisierInput);
int citireInformatieDirector(char *numeFisier, struct Fisier *fisierInput);
int citireFisierObisnuit(char *numeFisier, struct Fisier *fisierInput);
int citireInformatieLink(char *numeFisier, struct Fisier *fisierInput);
int esteBMP(char *numeFisier);
int esteFisierObisnuit(char *numeFisier);
int esteLink(char *numeFisier);
int esteDirector(char *numeFisier);
int transformaBMPInGrayscale(char *numeFisier);

#endif
