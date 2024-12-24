#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include "gamelib.h"


int leggi_scelta() {
    int scelta;
    while (scanf("%d", &scelta) != 1 || scelta > 3 || scelta < 1) {
        printf("\n\t\tSCELTA NON CORRETTA! REINSERISCI IL VALORE: ");
        while (getchar() != '\n'); // Pulisce il buffer
    }
    return scelta;
}

int main() {
    int scelta;

    do {
        printf("\033[1;36m");
        printf("\t\tINGRANDISCI LO SCHERMO PER EVITARE GLITCH GRAFICI\n");
        printf("\n");
        printf("  |||||   ||||||  ||||||  ||||||  ||||||  |     |  |     |  ||||||  ||  |||||||   \n");
        printf("  |       |    |  ||||||  |       |       ||    |  ||    |  |    |  ||     |      \n");
        printf("  |||||   |    |  ||      ||||||  ||||||  |  |  |  |  |  |  ||||||  ||     |      \n");
        printf("  |       |    |  |  |         |  |       |   | |  |   | |  |    |  ||     |      \n");
        printf("  |       ||||||  |   |   ||||||  ||||||  |     |  |     |  |    |  ||     |      \n");
        printf("\n");
        printf("\t\t--------------------------------------------------\n");
        printf("\t\t|                                                |\n");
        printf("\t\t|                1......CREA MAPPA               |\n");
        printf("\t\t|                2...........GIOCA               |\n");
        printf("\t\t|                3............ESCI               |\n");
        printf("\t\t|                                                |\n");
        printf("\t\t--------------------------------------------------\n");

        scelta = leggi_scelta();

        switch (scelta) {
            case 1:
                crea_mappa();
                break;
            case 2:
                gioca();
                break;
            case 3:
                terminagioco();
                break;
        }
    } while (scelta != 3);

    return 0;
}