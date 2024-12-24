#ifndef GAMELIB_H
#define GAMELIB_H
#define N_OGGETTI 5
#define N_PERICOLI 3



//void gioca();

void crea_mappa(void);
void gioca(void);
void terminagioco(void);
int leggi_scelta();
void stampa_scacchiera(void);


//void stampa_scacchiera();

static const char *nomi_oggetti[N_OGGETTI] = {"medikit", "pozione", "materiale", "colpi", "nessun oggetto"};
static const char *nomi_pericoli[N_PERICOLI] = {"nessun pericolo", "trappola", "alieno"};

enum Tipo_oggetto {
	medikit,
	pozione,
	materiale,
	colpi,
	nessun_oggetto
};


enum Tipo_pericolo{
	nessun_pericolo=5,
	trappola,
	alieno

};

enum Stato_giocatore {
	vulnerabile,
	scudo_vita,
	solo_vita,
	solo_scudo
};

struct giocatore {
	enum Stato_giocatore stato;
    int zaino[N_OGGETTI];
    int x;
    int y;
	int contaoggetti;
	int ufokill;
};


struct cella{
	enum Tipo_pericolo pericolo;
	enum Tipo_oggetto oggetto;

};

struct piano{
unsigned int npiano;
struct piano* prossimo;

};

typedef struct piano Piano;
typedef Piano *Pianoptr;

#endif