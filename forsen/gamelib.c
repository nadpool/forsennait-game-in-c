#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include<unistd.h>
#include "gamelib.h"


static unsigned int turno = 0; // Per contare i turni
void crea_mappa(void);
void muovi(struct giocatore *g);
void gioca(void);
void terminagioco(void);
static void crea_scacchiera(void);
int lato=0;
static struct cella *scacchiera=NULL;
static struct giocatore ciccio;
static struct giocatore ninja;

void usa_oggetto(struct giocatore *giocatore);
static void scontro(void);
static void inseriscip(Pianoptr *headptr, Pianoptr *tailptr,int val); //inserisce piani di ninja
static void inseriscip1(Pianoptr *headptr1, Pianoptr *tailptr1,int val1);//inserisce piani di ciccio
int print(Pianoptr currentptr); //stampa piani di ninja
int print1(Pianoptr currentptr1);//stampa piani di ciccio
int rimuovip(Pianoptr *headptr, Pianoptr *tailptr);      //rimuove piano di ninja
int rimuovip1(Pianoptr *headptr1, Pianoptr *tailptr1);   //rimuove piano di ciccio
static void contaciccio(void);
static void contaninja(void);
int vuota(Pianoptr headptr);     //funzioni che impostano a null il puntatore in cima alla lista
int vuota1(Pianoptr headptr1);
int vittciccio;
int vittninja;

void crea_mappa(void){
    int scelta;
    do{
           printf("\t\t                                                  \n");
           printf("\t\t                                                  \n");
           printf("\t\t                1......CREA SCACCHIERA            \n");
           printf("\t\t                2.....STAMPA SCACCHIERA           \n");
           printf("\t\t                3......TORNA INDIETRO             \n");
           printf("\t\t                                                  \n");
           printf("\t\t                                                  \n");

           scelta = leggi_scelta();

              switch(scelta){

              case 1:
                    crea_scacchiera();
                break;
              case 2:
                    stampa_scacchiera();
                break;
              case 3:

                break;
                          }

      }while( scelta != 3);

}

void leggi_probabilita(int *prob, int n, const char **nomi) {
    int totale = 0; // Controllo per la somma delle probabilità
    do {
        totale = 0; // Resetta il totale a ogni iterazione
        for (int i = 0; i < n; i++) { // Itera sulle probabilità richieste
            printf("Probabilità di incontrare %s: ", nomi[i]);

            // Legge il valore e controlla che sia valido (intero >= 0)
            while (scanf("%d", &prob[i]) != 1 || prob[i] < 0) {
                printf("Valore non valido! Riprova: ");
                while (getchar() != '\n'); // Pulisce il buffer
            }

            totale += prob[i]; // Aggiunge il valore al totale
        }

        // Controlla se la somma è 100
        if (totale != 100) {
            printf("La somma delle probabilità deve essere 100! Riprova.\n");
        }
    } while (totale != 100); // Continua fino a quando il totale è corretto
}

void crea_scacchiera(void) {
    printf("Digita la lunghezza e altezza della mappa: ");
    while (scanf("%d", &lato) != 1 || lato <= 0) {
        printf("\nScelta non corretta! Inserisci un valore valido: ");
        while (getchar() != '\n'); // Pulisce il buffer
    }

    // Alloca dinamicamente la memoria per la scacchiera
    scacchiera = (struct cella *)malloc(lato * lato * sizeof(struct cella));
    if (scacchiera == NULL) {
        fprintf(stderr, "Errore di allocazione memoria!\n");
        exit(EXIT_FAILURE);
    }

    // Array per probabilità di oggetti e pericoli
    int prob_oggetti[N_OGGETTI], prob_pericoli[N_PERICOLI];

    // Richiede le probabilità per gli oggetti
    leggi_probabilita(prob_oggetti, N_OGGETTI, nomi_oggetti);

    // Richiede le probabilità per i pericoli
    leggi_probabilita(prob_pericoli, N_PERICOLI, nomi_pericoli);

    srand((unsigned)time(NULL)); // Seed per la generazione casuale
    for (int i = 0; i < lato; i++) {
        for (int j = 0; j < lato; j++) {
            // Assegna oggetti in base alle probabilità
            int r = rand() % 100;
            if (r < prob_oggetti[0])
                scacchiera[i * lato + j].oggetto = medikit;
            else if (r < prob_oggetti[0] + prob_oggetti[1])
                scacchiera[i * lato + j].oggetto = pozione;
            else if (r < prob_oggetti[0] + prob_oggetti[1] + prob_oggetti[2])
                scacchiera[i * lato + j].oggetto = materiale;
            else if (r < prob_oggetti[0] + prob_oggetti[1] + prob_oggetti[2] + prob_oggetti[3])
                scacchiera[i * lato + j].oggetto = colpi;
            else
                scacchiera[i * lato + j].oggetto = nessun_oggetto;

            // Assegna pericoli in base alle probabilità
            r = rand() % 100;
            if (r < prob_pericoli[0])
                scacchiera[i * lato + j].pericolo = nessun_pericolo;
            else if (r < prob_pericoli[0] + prob_pericoli[1])
                scacchiera[i * lato + j].pericolo = trappola;
            else
                scacchiera[i * lato + j].pericolo = alieno;
        }
    }

    // Imposta posizione iniziale dei giocatori
    ciccio.x = rand() % lato;
    ciccio.y = rand() % lato;
    ciccio.stato = solo_vita;
    ciccio.ufokill = 0;

    ninja.x = rand() % lato;
    ninja.y = rand() % lato;
    ninja.stato = solo_vita;
    ninja.ufokill = 0;
}


void stampa_scacchiera(void) {
    // Controlla se la scacchiera è stata creata
    if (scacchiera == NULL) {
        printf("DEVI PRIMA CREARE UNA SCACCHIERA...\n");
        return; // Esci dalla funzione se la scacchiera non esiste
    }

    // Legenda per gli oggetti e i pericoli
    printf("LEGENDA:\n");
    printf("| medikit: 0 | pozione: 1 | materiale: 2 | colpi: 3 | nessun oggetto: 4 |\n");
    printf("| nessun pericolo: 5 | trappola: 6 | alieno: 7 |\n\n");

    // Colori ANSI
    #define ANSI_COLOR_DEFAULT "\x1b[36m" // Colore azzurro predefinito
    #define ANSI_COLOR_CICCIO  "\x1b[32m" // Verde per Ciccio
    #define ANSI_COLOR_NINJA   "\x1b[31m" // Rosso per Ninja

    // Applicazione del colore azzurro di default
    printf(ANSI_COLOR_DEFAULT);

    // Stampa la scacchiera con oggetti e pericoli
    for (i = 0; i < lato; i++) {
        for (j = 0; j < lato; j++) {
            char simbolo_ciccio = '-'; // Simbolo per Ciccio
            char simbolo_ninja = '-';  // Simbolo per Ninja

            // Verifica se Ciccio è sulla cella corrente
            if (ciccio.x == j && ciccio.y == i) {
                simbolo_ciccio = 'C'; // Usa 'C' per Ciccio
            }

            // Verifica se Ninja è sulla cella corrente
            if (ninja.x == j && ninja.y == i) {
                simbolo_ninja = 'N'; // Usa 'N' per Ninja
            }

            // Stampa la cella con i simboli colorati
            printf("[");
            printf("%d,%d,", scacchiera[i * lato + j].oggetto, scacchiera[i * lato + j].pericolo);

            // Stampa Ciccio in verde se è presente
            if (simbolo_ciccio == 'C') {
                printf(ANSI_COLOR_CICCIO "%c" ANSI_COLOR_DEFAULT ",", simbolo_ciccio);
            } else {
                printf("%c,", simbolo_ciccio);
            }

            // Stampa Ninja in rosso se è presente
            if (simbolo_ninja == 'N') {
                printf(ANSI_COLOR_NINJA "%c" ANSI_COLOR_DEFAULT, simbolo_ninja);
            } else {
                printf("%c", simbolo_ninja);
            }

            printf("] ");
        }
        printf("\n"); // Nuova riga per ogni riga della scacchiera
    }

    // Mantieni il colore azzurro al termine
    printf(ANSI_COLOR_DEFAULT);
}

void gioca(void) {
    if (scacchiera == NULL) {
        printf("\n\t\t\tDEVI PRIMA CREARE LA MAPPA!\n");
        return;
    }

    printf("IL GIOCO INIZIA!\n\n");
    while (1) {
        struct giocatore *giocatore_attuale;
        if (turno % 2 == 0) {
            giocatore_attuale = &ninja;
            printf("TURNO DI NINJA (N).\n\n");



        } else {
            giocatore_attuale = &ciccio;
            printf("TURNO DI CICCIO (C).\n\n");



        }
        printf("\nSCACCHIERA ATTUALE:\n");
        for (int i = 0; i < lato; i++) {
            for (int j = 0; j < lato; j++) {
                char simbolo_ciccio = '-';
                char simbolo_ninja = '-';

                // Controlla se Ciccio è su questa cella
                if (ciccio.x == j && ciccio.y == i) {
                    simbolo_ciccio = 'C';
                }

                // Controlla se Ninja è su questa cella
                if (ninja.x == j && ninja.y == i) {
                    simbolo_ninja = 'N';
                }

                // Stampa la cella con colori
                printf("[");
                printf("%d,%d,", scacchiera[i * lato + j].oggetto, scacchiera[i * lato + j].pericolo);

                // Colore per Ciccio
                if (simbolo_ciccio == 'C') {
                    printf(ANSI_COLOR_CICCIO "%c" ANSI_COLOR_DEFAULT ",", simbolo_ciccio);
                } else {
                    printf("%c,", simbolo_ciccio);
                }

                // Colore per Ninja
                if (simbolo_ninja == 'N') {
                    printf(ANSI_COLOR_NINJA "%c" ANSI_COLOR_DEFAULT, simbolo_ninja);
                } else {
                    printf("%c", simbolo_ninja);
                }

                printf("] ");
            }
            printf("\n");
        }
        printf("\n");




        // Stampa lo stato attuale del giocatore
        printf("Stato attuale: %s\n", (giocatore_attuale->stato == solo_vita) ? "Solo Vita \n" :
                                          (giocatore_attuale->stato == solo_scudo) ? "Solo Scudo\n" :
                                          (giocatore_attuale->stato == scudo_vita) ? "Scudo e Vita\n" : "Vulnerabile\n");

        // Menu di opzioni
        int scelta;
        printf("1. Muovi\n\n2. Usa Oggetto\n\n3. Esci\n\nScelta: ");
        while (scanf("%d", &scelta) != 1 || scelta < 1 || scelta > 3) {
            printf("Scelta non valida. Riprova: ");
            printf("/n ");
            while (getchar() != '\n'); // Pulisce il buffer
        }

        // Gestione delle scelte
        if (scelta == 3) {
            printf("Partita terminata.\n");
            terminagioco();
            break; // Termina il gioco
        }

        if (scelta == 2) {
            usa_oggetto(giocatore_attuale); // Usa un oggetto dallo zaino
        } else if (scelta == 1) {
            muovi(giocatore_attuale); // Muovi il giocatore
        }

        turno++; // Passa il turno all'altro giocatore
    }
}



// Funzione per muovere il giocatore

void muovi(struct giocatore *g) {
    int direzione;
    printf("\n");
    printf("                1...NORD                  \n");
    printf("\n");
    printf("       4...OVEST   +    3...EST               \n");
    printf("\n");
    printf("                 2...SUD                  \n");
    while (scanf("%d", &direzione) != 1 || direzione < 1 || direzione > 4) {
        printf("Scelta non valida. Riprova: ");
        while (getchar() != '\n'); // Pulisce il buffer
    }

    int nuova_x = g->x;
    int nuova_y = g->y;

    switch (direzione) {
        case 1: nuova_y--; break; // Nord
        case 2: nuova_y++; break; // Sud
        case 3: nuova_x++; break; // Est
        case 4: nuova_x--; break; // Ovest
    }

    // Controlla se il movimento è valido
    if (nuova_x < 0 || nuova_x >= lato || nuova_y < 0 || nuova_y >= lato) {
        printf("Movimento non valido! Sei fuori dalla scacchiera. Perdi il turno.\n");
        return;
    }

    g->x = nuova_x;
    g->y = nuova_y;

    struct cella *cella_corrente = &scacchiera[nuova_y * lato + nuova_x];

    // Gestione pericoli
    if(ciccio.x == ninja.x && ciccio.y == ninja.y){
       scontro();
    } else if (cella_corrente->pericolo == trappola) {
        printf("Sei finito in una trappola!\n");
        printf("Sei morto!\n");
        terminagioco();
    } else if (cella_corrente->pericolo == alieno) {
        printf("\033[1;34m");
        printf("HAI TROVATO UN ALIENO! VUOI COMBATTERE?\n");
        printf("\t\t    1......SI                 \n");
        printf("\t\t    0......NO                 \n");
        printf("\033[1;36m");
        int combatti;
        while (scanf("%d", &combatti) != 1 || (combatti != 0 && combatti != 1)) {
            printf("Scelta non valida. Riprova (1 per sì, 0 per no): ");
            while (getchar() != '\n'); // Pulisce il buffer
        }

        if (combatti == 0) {
            printf("Hai scelto di scappare. Non puoi raccogliere l'oggetto in questa cella.\n");
            return;
        }

        printf("Hai scelto di combattere! L'alieno è stato ucciso.\n");
        g->ufokill++; // Incrementa il numero di uccisioni

        // Calcolo probabilità di essere colpito
        int probabilita = rand() % 100;
        if (probabilita < 35) {
            printf("L'alieno ti ha colpito!\n");

            // Logica per cambiare lo stato del giocatore
            if (g->stato == solo_vita) {
                g->stato = vulnerabile;
                printf("Hai perso la vita! Ora sei vulnerabile.\n");
            } else if (g->stato == scudo_vita) {
                g->stato = solo_scudo;
                printf("Hai perso la vita! Ti rimane lo scudo.\n");
            } else if (g->stato == solo_scudo) {
                g->stato = vulnerabile;
                printf("Hai perso lo scudo! Ora sei vulnerabile.\n");
            } else if (g->stato == vulnerabile) {
                printf("Sei morto!\n");
                terminagioco();
            }
        } else {
            printf("L'alieno non ti ha colpito. Sei salvo!\n");
        }
    }

    // Gestione oggetti
    if (cella_corrente->oggetto != nessun_oggetto) {
        printf("Hai trovato un oggetto: %s!\n", nomi_oggetti[cella_corrente->oggetto]);
        g->zaino[cella_corrente->oggetto]++;
        g->contaoggetti++;
        cella_corrente->oggetto = nessun_oggetto; // Rimuove l'oggetto dalla cella
    }
}



void usa_oggetto(struct giocatore *giocatore) {
    printf("Contenuto dello zaino:\n");
    printf("1. Medikit: %d\n", giocatore->zaino[medikit]);
    printf("2. Pozione: %d\n", giocatore->zaino[pozione]);

    int scelta;
    printf("Scegli un oggetto da usare (0 per annullare): ");
    while (scanf("%d", &scelta) != 1 || scelta < 0 || scelta > 2) {
        printf("Scelta non valida. Riprova: ");
        while (getchar() != '\n'); // Pulisce il buffer
    }

    if (scelta == 0) {
        printf("Nessun oggetto usato.\n");
        return;
    }

    if (giocatore->zaino[scelta - 1] <= 0) {
        const char* nomi_oggetti[] = {"Medikit", "Pozione"};
        printf("Non hai abbastanza %s!\n", nomi_oggetti[scelta - 1]);
        return;
    }

    giocatore->zaino[scelta - 1]--; // Usa l'oggetto, decrementando la quantità

    switch (scelta - 1) {
        case medikit:
            if (giocatore->stato == vulnerabile) {
                giocatore->stato = solo_vita;
                printf("Medikit usato! Stato aggiornato: da vulnerabile a solo vita.\n");
            } else if (giocatore->stato == solo_scudo) {
                giocatore->stato = scudo_vita;
                printf("Medikit usato! Stato aggiornato: da solo scudo a scudo vita.\n");
            } else {
                printf("Non hai bisogno di un Medikit al momento.\n");
            }
            break;

        case pozione:
            if (giocatore->stato == vulnerabile) {
                giocatore->stato = solo_scudo;
                printf("Pozione usata! Stato aggiornato: da vulnerabile a solo scudo.\n");
            } else if (giocatore->stato == solo_vita) {
                giocatore->stato = scudo_vita;
                printf("Pozione usata! Stato aggiornato: da solo vita a scudo vita.\n");
            } else {
                printf("Non hai bisogno di una Pozione al momento.\n");
            }
            break;

        default:
            printf("Errore: oggetto non valido!\n");
    }
}


static void scontro(void){

int count=0;
int count1=0;
sleep(1);
system("clear");
printf("\033[1;33m");
printf("-------SCONTRO FINALE-------\n");
printf("\n");
Pianoptr headptr=NULL;
Pianoptr tailptr=NULL;

Pianoptr headptr1=NULL;
Pianoptr tailptr1=NULL;


int val=ninja.zaino[materiale];
int cal=ciccio.zaino[colpi];
                                                 //assegno a val e cal il numero dei materiali dei 2 giocatori
int val1=ciccio.zaino[materiale];
int cal1=ninja.zaino[colpi];


printf("Ninja cerca di contruire dei piani con i suoi materiali\n");
printf("...\n");
sleep(1);
printf("Ciccio cerca di contruire dei piani con i suoi materiali\n");
printf("...\n");
sleep(1);
printf("TORRE DI NINJA\n");
printf("\n");

while(ninja.zaino[materiale]>0){
val=ninja.zaino[materiale];
                                                 //costruisco i piani in base al numero materiali con un ciclo while e chiamando l apposita funzione
inseriscip(&headptr,&tailptr,val);

ninja.zaino[materiale]--;

}
print(headptr);



printf("\t\t\tTORRE DI CICCIO\n");
printf("\n");

while(ciccio.zaino[materiale]>0){
val1=ciccio.zaino[materiale];

inseriscip1(&headptr1,&tailptr1,val1);

ciccio.zaino[materiale]--;

}
print1(headptr1);

printf("Ciccio cerca di distruggere la torre nemica con i suoi razzi\n");
printf("...\n");
sleep(1);
printf("Ninja cerca di distruggere la torre nemica con i suoi razzi\n");
printf("...\n");
sleep(1);
printf("\n");
printf("TORRE DI NINJA DOPO\n");
printf("\n");
while(ciccio.zaino[colpi]>0){

if(!vuota(headptr)){
cal=rimuovip(&headptr,&tailptr);                    //per ogni colpo rimuovo un piano dell avversario chiamando l apposita funzione in while
printf("Piano :%d rimosso\n",cal);
ciccio.zaino[colpi]--;
}
else{

  break;
}

}
count=print(headptr);




printf("\n");
printf("\t\t\tTORRE DI CICCIO DOPO\n");
printf("\n");
while(ninja.zaino[colpi]>0){

if(!vuota1(headptr1)){
cal1=rimuovip1(&headptr1,&tailptr1);
printf("\t\t\tPiano :%d rimosso\n",cal1);
ninja.zaino[colpi]--;

}
else{

  break;
}

}
count1=print1(headptr1);

//controllo chi vince tramite la torre e tramite i stati e poi chiammo reset per azzerare i valori

if (count>count1){
  printf("VINCE NINJA...aveva piu piani per proteggersi\n");
  vittninja++;terminagioco();
}
else if (count<count1){
  printf("VINCE CICCIO...aveva piu piani per protteggersi\n");
  vittciccio++;terminagioco();
}
else if (count==count1){
  printf("continua il combattimento...\n");
  sleep(1);
  printf("...\n");
            //controllo lo stato dei giocatori per decretare il vincitore
           if (ninja.stato==solo_vita && ciccio.stato==vulnerabile){
               printf("VINCE NINJA...aveva piu vita\n");vittninja++;terminagioco();
           }else if(ninja.stato==solo_scudo && ciccio.stato==vulnerabile){
               printf("VINCE NINJA...aveva piu vita\n");vittninja++;terminagioco();
           }else if(ninja.stato==scudo_vita && ciccio.stato!=scudo_vita){
               printf("VINCE NINJA...aveva piu vita\n");vittninja++;terminagioco();
           }else if(ninja.stato==scudo_vita && ciccio.stato==scudo_vita){
               printf("PAREGGIO!!...avevano la stessa vita\n");terminagioco();
           }else if(ninja.stato==solo_vita && ciccio.stato==solo_vita){
               printf("PAREGGIO!!...avevano la stessa vita\n");terminagioco();
           }else if(ninja.stato==vulnerabile && ciccio.stato==vulnerabile){
               printf("PAREGGIO!!...avevano la stessa vita\n");terminagioco();
           }else if(ninja.stato==solo_scudo && ciccio.stato==solo_scudo){
               printf("PAREGGIO!!...avevano la stessa vita\n");terminagioco();
           }else if (ciccio.stato==solo_vita && ninja.stato==vulnerabile){
               printf("VINCE CICCIO...aveva piu vita\n");vittciccio++;terminagioco();
           }else if(ciccio.stato==solo_scudo && ninja.stato==vulnerabile){
               printf("VINCE CICCIO...aveva piu vita\n");vittciccio++;terminagioco();
           }else if(ciccio.stato==scudo_vita && ninja.stato!=scudo_vita){
               printf("VINCE CICCIO...aveva piu vita\n");vittciccio++;terminagioco();}

 }

printf("\033[1;36m");

return;

}

static void inseriscip(Pianoptr *headptr, Pianoptr *tailptr,int val){

Pianoptr newptr=malloc(sizeof(Piano));

if(newptr != NULL){                                                     //funzione che aggiunge i piani alla coda riferita ad un unico giocatore
  if(val>0){
  newptr->npiano= val;
  newptr->prossimo =NULL;
  if(vuota(*headptr)){
    *headptr = newptr;
  }
  else{
    (*tailptr)->prossimo=newptr;
  }
  *tailptr=newptr;

}else{

  printf("Piano numero:%d non costruito! Non hai Materiali\n",val);
}

}
}

static void inseriscip1(Pianoptr *headptr1, Pianoptr *tailptr1,int val1){

Pianoptr newptr1=malloc(sizeof(Piano));

if(newptr1 != NULL){
  if(val1>0){
  newptr1->npiano= val1;
  newptr1->prossimo =NULL;
  if(vuota1(*headptr1)){
    *headptr1 = newptr1;
  }
  else{
    (*tailptr1)->prossimo=newptr1;
  }
  *tailptr1=newptr1;

}else{

  printf("\t\t\tPiano numero:%d non costruito! Non hai Materiali\n",val1);
}

}
}

int rimuovip(Pianoptr *headptr, Pianoptr *tailptr){

int valu = (*headptr)->npiano;
Pianoptr temptr=*headptr;
*headptr=(*headptr)->prossimo;

if(*headptr==NULL){                                       //funzione che rimuove i piani in coda
  *tailptr=NULL;
}
free(temptr);
temptr=NULL;
return valu;

}


int rimuovip1(Pianoptr *headptr1, Pianoptr *tailptr1){

int valu1 = (*headptr1)->npiano;
Pianoptr temptr1=*headptr1;
*headptr1=(*headptr1)->prossimo;

if(*headptr1==NULL){
  *tailptr1=NULL;
}
free(temptr1);
temptr1=NULL;
return valu1;

}


int print(Pianoptr currentptr){
int count=0;
if(currentptr==NULL){

  puts("Nessun Piano \n");
}else{                                                   //funzione che stampa i piani del giocatore finche non si raggiunge la fine della lista

  while(currentptr != NULL){

printf("#---%d----#\n", currentptr->npiano);

    currentptr=currentptr->prossimo;
    count++;
  }

}
return count;
}

int print1(Pianoptr currentptr1){
 int count1=0;
if(currentptr1==NULL){

  puts("\t\t\tNessun Piano \n");
}else{

  while(currentptr1 != NULL){

printf("\t\t\t#---%d----#\n", currentptr1->npiano);

    currentptr1=currentptr1->prossimo;
    count1++;
  }

}
return count1;
}



int vuota(Pianoptr headptr){                      //funzione per restituire un valore se la lista e vuota o no
  return headptr==NULL;
}
int vuota1(Pianoptr headptr1){
  return headptr1==NULL;
}

void terminagioco(void) {
    // Mostra statistiche di fine gioco
    printf("Statistiche di fine gioco:\n");
    printf("Ciccio: %d alieni uccisi\n", ciccio.ufokill);
    printf("Ninja: %d alieni uccisi\n", ninja.ufokill);

    // Determina il vincitore
    if (ciccio.ufokill > ninja.ufokill) {
        printf("Ciccio vince!\n");
    } else if (ninja.ufokill > ciccio.ufokill) {
        printf("Ninja vince!\n");
    } else {
        printf("Pareggio!\n");
    }

    // Azzera la scacchiera
    if (scacchiera != NULL) {
        free(scacchiera); // Libera la memoria allocata
        scacchiera = NULL; // Reimposta il puntatore
        printf("La scacchiera è stata azzerata.\n");
    } else {
        printf("La scacchiera era già vuota.\n");
    }

ciccio.stato=solo_vita;
    ninja.stato=solo_vita;


  // inizializzo le uccisioni dell alieno
    ciccio.ufokill=0;
    ninja.ufokill=0;

    ninja.zaino[medikit]=0;
    ninja.zaino[pozione]=0;
    ninja.zaino[materiale]=0;
    ninja.zaino[colpi]=0;

    ciccio.zaino[medikit]=0;
    ciccio.zaino[pozione]=0;
    ciccio.zaino[materiale]=0;
    ciccio.zaino[colpi]=0;


    // Termina il gioco
    exit(0);
}
