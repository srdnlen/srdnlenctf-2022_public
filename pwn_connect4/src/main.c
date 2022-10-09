#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define FORZA_QUANTO 4

#define NESSUNA_PEDINA ' '
#define PEDINA_GIOCATORE 'X'
#define PEDINA_COMPUTER 'O'
#define PEDINA_CORRENTE (turno_giocatore ? PEDINA_GIOCATORE : PEDINA_COMPUTER)

#define INIT_RIGHE_SCACCHIERA 6
#define INIT_COLONNE_SCACCHIERA 7

int setup(){
	setvbuf(stdin, NULL, 2, NULL);
	setvbuf(stdout, NULL, 2, NULL);
	setvbuf(stderr, NULL, 2, NULL);
	return 0;
}

int main(){
    /* Dichiarazione variabili */
    int riga; /* Utilizzato nel ciclo di stampa */
    int colonna_scelta = 0;
    bool input_valido;

    /* Variabili per controllo vincita */
    int contatore_controllo;
    int fila_orizzontale;
    int fila_verticale;
    int fila_diagonale1;
    int fila_diagonale2;

    bool fine_gioco = false;
    int vincitore = NESSUNA_PEDINA; /* Nessuna pedina = pareggio, altrimenti il vincitore è il giocatore corrispondente */
    int ultima_colonna_giocatore;
    int ultima_colonna_computer;

    bool turno_giocatore = true; /* Il giocatore è il primo a giocare */

    char flag[64];

    char scacchiera[INIT_RIGHE_SCACCHIERA][INIT_COLONNE_SCACCHIERA];

    int RIGHE_SCACCHIERA = INIT_RIGHE_SCACCHIERA;
    int COLONNE_SCACCHIERA = INIT_COLONNE_SCACCHIERA;

    /* Inizializzazione seed srand */
    srand(time(NULL));

    /* Se il computer non ha ancora mosso, l'ultima mossa è indefinita;
       generiamo un valore casuale come placeholder */
    ultima_colonna_computer = rand() % COLONNE_SCACCHIERA;
    
    setup();

    printf("CONNECT FOUR\n");

    int i_init, j_init;
    for(i_init = 0; i_init < RIGHE_SCACCHIERA; i_init++){
        for(j_init = 0; j_init < COLONNE_SCACCHIERA; j_init++){
            scacchiera[i_init][j_init] = NESSUNA_PEDINA;
        }
    }

    /* Carica flag */
    int i_flag = 0;
    FILE *fp = fopen("./flag.txt", "rb");

    if (fp != NULL){
        while(fread(&flag[i_flag], 1, 1, fp) == 1 && i_flag < sizeof(flag)){
            i_flag++;
        };    
        fclose(fp);
    }
    else {
        printf("ERROR: Flag not found. Contact the admin.\n");
        exit(1);
    }

    while(1) {
        bool almeno_una_cella_libera = false;

        printf(" ////////////////////////////\n");
        for (riga = 0; riga < RIGHE_SCACCHIERA; riga++){
            int colonna;

            printf("| ");
            for (colonna = 0; colonna < COLONNE_SCACCHIERA; colonna++){
                switch(scacchiera[riga][colonna]){
                    case PEDINA_COMPUTER:
                        putchar(PEDINA_COMPUTER);
                        break;
                    case PEDINA_GIOCATORE:
                        putchar(PEDINA_GIOCATORE);
                        break;
                    case NESSUNA_PEDINA:
                    default:
                        almeno_una_cella_libera = true;
                        putchar(scacchiera[riga][colonna]);
                        break;
                }

                printf(" | ");
            }
            
            printf("\n|---|---|---|---|---|---|---|\n");
        }
        printf("|---------------------------| \n");

        if (!almeno_una_cella_libera)
            fine_gioco = true;

        if(fine_gioco) {
            switch(vincitore){
                default:
                case NESSUNA_PEDINA:
                    printf("It's a draw!\n");
                    break;
                case PEDINA_GIOCATORE:
                    printf("You win!\n");
                    break;
                case PEDINA_COMPUTER:
                    printf("The CPU wins!\n");
                    break;
            }

            break;
        }

        if(turno_giocatore){
            input_valido = false;

            while(!input_valido){
                printf("Choose a column [0-%d]: ", COLONNE_SCACCHIERA-1);
                scanf("%d", &colonna_scelta);
                getchar();

                /* Note: vuln here, can input negative column */
                if(colonna_scelta >= COLONNE_SCACCHIERA){
                    printf("The column is not valid.\n");
                    continue;
                }

                if(scacchiera[0][colonna_scelta] == PEDINA_COMPUTER || scacchiera[0][colonna_scelta] == PEDINA_GIOCATORE){
                    printf("The column is full.\n");
                    continue;
                }

                input_valido = true;

                ultima_colonna_giocatore = colonna_scelta;
            }
        } else {
            /* Il computer sceglie tra mettere una pedina a fianco o sopra
               o all'ultima pedina dell'avversario (per tentare di fermarlo),
               o all'ultima propria (per provare a vincere) */
            bool interrompi_giocatore;
            bool a_fianco;

            interrompi_giocatore = rand() % 2;
            if(interrompi_giocatore)
                colonna_scelta = ultima_colonna_giocatore;
            else
                colonna_scelta = ultima_colonna_computer;

            a_fianco = rand() % 2;
            /* Se non c'è spazio per mettere sopra o abbiamo deciso a caso di mettere
               a fianco, facciamolo */
            if(a_fianco || scacchiera[0][colonna_scelta] != NESSUNA_PEDINA){
                int metti_a_destra = rand() % 2;

                /* Controlliamo che non siamo nell'ultima colonna; se lo siamo mettiamo a sinistra;
                   se no, se non siamo nella prima (in quel caso mettiamo a destra) decidiamo a caso */
                if(colonna_scelta + 1 < COLONNE_SCACCHIERA && (colonna_scelta - 1 < 0 || metti_a_destra)){
                    colonna_scelta = colonna_scelta + 1;
                } else {
                    colonna_scelta = colonna_scelta - 1;
                }
            }

            /* Se la colonna scelta non aveva spazi liberi, scegline un'altra a caso */
            while(scacchiera[0][colonna_scelta] != NESSUNA_PEDINA){
                colonna_scelta = rand() % COLONNE_SCACCHIERA; /* Numero da 0 a (COLONNE_SCACCHIERA - 1) */
            }

            /* Salviamo la mossa (serve a decidere la prossima) */
            ultima_colonna_computer = colonna_scelta;

            printf("The CPU chooses column %d.\n", colonna_scelta);
        }

        /* Inserimento pedina nella prima cella disponibile della colonna */
        /* Iniziamo dalla riga più in alto */
        for(riga = 0; riga < RIGHE_SCACCHIERA; riga++){
            /* Se siamo arrivati alla riga più in basso o la riga
               successiva è occupata, collochiamo la pedina; questo
               avrà sempre successo perché abbiamo controllato di
               avere spazio libero prima */
            if (riga == RIGHE_SCACCHIERA - 1 || scacchiera[riga + 1][colonna_scelta] != NESSUNA_PEDINA){
                scacchiera[riga][colonna_scelta] = PEDINA_CORRENTE;
                break;
            }
        }

        /* Controllo condizione di vincita (conto le pedine consecutive,
           resettando la variabile se si interrompe la fila) */
        fila_orizzontale = 0;
        fila_verticale = 0;
        fila_diagonale1 = 0;
        fila_diagonale2 = 0;

        /* Controlliamo intorno alla pedina che è stata inserita */
        for(contatore_controllo = -3; contatore_controllo <= 3; contatore_controllo++){
            /* Controllo orizzontale (stessa riga, scorri colonna) */
            if(colonna_scelta + contatore_controllo >= 0 && colonna_scelta + contatore_controllo < COLONNE_SCACCHIERA){
                if(scacchiera[riga][colonna_scelta + contatore_controllo] == PEDINA_CORRENTE){
                    fila_orizzontale++;

                    if(fila_orizzontale == 4){
                        fine_gioco = true;
                        break;
                    }
                } else {
                    fila_orizzontale = 0;
                }
            }

            /* Controllo verticale (stessa colonna, scorri riga) */
            if(riga + contatore_controllo >= 0 && riga + contatore_controllo < RIGHE_SCACCHIERA){
                if(scacchiera[riga + contatore_controllo][colonna_scelta] == PEDINA_CORRENTE){
                    fila_verticale++;

                    if(fila_verticale == 4){
                        fine_gioco = true;
                        break;
                    }
                } else {
                    fila_verticale = 0;
                }
            }

            /* Controllo diagonale \ (scorre colonna e riga normalmente) */
            if((colonna_scelta + contatore_controllo >= 0 && colonna_scelta + contatore_controllo < COLONNE_SCACCHIERA) &&
               (riga + contatore_controllo >= 0 && riga + contatore_controllo < RIGHE_SCACCHIERA)){
                if(scacchiera[riga + contatore_controllo][colonna_scelta + contatore_controllo] == PEDINA_CORRENTE){
                    fila_diagonale1++;

                    if(fila_diagonale1 == 4){
                        fine_gioco = true;
                        break;
                    }
                } else {
                    fila_diagonale1 = 0;
                }
            }

            /* Controllo diagonale / (scorre colonna normalmente e riga al contrario) */
            if((colonna_scelta + contatore_controllo >= 0 && colonna_scelta + contatore_controllo < COLONNE_SCACCHIERA) &&
               (riga - contatore_controllo >= 0 && riga - contatore_controllo < RIGHE_SCACCHIERA)){
                if(scacchiera[riga - contatore_controllo][colonna_scelta + contatore_controllo] == PEDINA_CORRENTE){
                    fila_diagonale2++;

                    if(fila_diagonale2 == 4){
                        fine_gioco = true;
                        break;
                    }
                } else {
                    fila_diagonale2 = 0;
                }
            }
        }

        /* Se il gioco è terminato a questo punto, il giocatore corrente ha vinto */
        if(fine_gioco){
            vincitore = PEDINA_CORRENTE;
        }

        turno_giocatore = !turno_giocatore;
    }
    
}
