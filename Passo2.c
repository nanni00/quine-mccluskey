/* RICERCA PRIMI ESSENZIALI */

#include "QMC.h"
 /*
	 Tabella dei vari n_qualcosa;
	 0 --> n_variabili in ingresso
	 1 --> n_mintermini ON_SET escluse le indifferenze
	 2 --> n_indifferenze
	 3 --> n_mintermini totali
	 4 --> n_implicanti primi
	 5 --> n_implicanti essenziali
	 6 --> pow(2, n_variabili in ingresso)
	 7 --> mintermine di indice più alto
*/

void StampaLinea(size_t *general_n)
{
	printf("\t");
	for (size_t i = 0; i < general_n[1]; ++i)
		printf("+-----");
	printf("+\n");
	return;
}

void StampaTabella(size_t *general_n, size_t *mintermini, Implic *primi)
{
	size_t i, j, k;
	printf("\nTabella di Copertura:\n\n\t");
	for (i = 0; i < general_n[7]; ++i) {
		if (mintermini[i] == 1) {
			if (i < 10) { printf("|    %d", i); }
			else if (10 <= i && i < 100) { printf("|   %d", i); }
			else if (100 <= i  && i < 1000) { printf("|  %d", i); }
			else { printf("|%d", i); }
		}
	}
	printf("|\n");
	for (i = 0; i < general_n[4]; ++i) {
		StampaLinea(general_n);
		printf("p%d\t", i);
		for (j = 0; j < general_n[6]; ++j) {
			if (mintermini[j] == 1) {
				if (primi[i].index[j] != 2)
					printf("|  0  ");
				else
					printf("|  2  ");
			}
		}
		printf("|\n");
	}
	StampaLinea(general_n);

	return;
}

int ContaPrimiScelti(int *vcurr, size_t n_primi)
{
	int x = 0;
	for (size_t j = 0; j < n_primi; ++j)
		if (vcurr[j] == 1 || vcurr[j] == 2)		
			++x;
		
	if (x == 0)
		return INT_MAX;
	return x;
}

bool CoperturaTotale(int *vcurr, size_t *general_n, Implic *primi, size_t *mintermini)
{
	size_t non_coperti = general_n[1], m = general_n[7];
	size_t check_mint[1024] = { 0, };
	memcpy(check_mint, mintermini, m * 4);
	size_t j, k;

	for (j = 0; j < general_n[4]; ++j) {
		if (vcurr[j] == 1) {
			for (k = 0; k < m; ++k)
				if ((check_mint[k] == 1) && (primi[j].index[k] == 2)) {
					check_mint[k] = 2;
					--non_coperti;
				}
		}
	}
	if (1) {
		if (non_coperti == 1) {
			printf("\n");
			for (j = 0; j < general_n[4]; ++j)
				printf("%d ", vcurr[j]);
			printf("\n");
			for (k = 0; k < general_n[7]; ++k)
				if (check_mint[k] == 1)
					printf("manca il mintermine %d\n", k);
		}
	}

	if (non_coperti == 0)
		return true;
	return false;
}

void BackTrack(int i, int *vcurr, int *vbest, size_t *general_n, Implic *primi, size_t *mintermini, size_t sum)
{
	int x;
	if (i == general_n[4]) {
		if (sum >= general_n[1]) {
			if ((x = ContaPrimiScelti(vcurr, general_n[4])) < general_n[5]) {
				if (CoperturaTotale(vcurr, general_n, primi, mintermini)) {
					memcpy(vbest, vcurr, general_n[4] * sizeof(int));

					printf("\n\nPossibile soluzione trovata.\n\n");
					general_n[5] = x;	/* aggiorno il numero di essenziali */
				}
			}
		}
		return;
	}
	if (vcurr[i] != 2) {			/* se vcurr[i] == 2 allora è già stato rilevato come essenziale e quindi va preso in ogni caso */
		
		vcurr[i] = 0;
		BackTrack(i + 1, vcurr, vbest, general_n, primi, mintermini, sum);

		vcurr[i] = 1;
		BackTrack(i + 1, vcurr, vbest, general_n, primi, mintermini, sum + primi[i].n_precedenti);
	}
	else 
		BackTrack(i + 1, vcurr, vbest, general_n, primi, mintermini, sum);

	return;
}

void BTiterativo(int *vcurr, int *vbest, size_t *general_n, Implic *primi, size_t *mintermini)
{
	size_t i, j, k, sum = 0;
	
	for (i = 0; i < general_n[4]; ++i) {
		for (j = 0; j < 2; ++j) {
			vcurr[i] = j;
			if (j == 1 && i < general_n[4]) {
				sum += primi[i].n_precedenti;
			}
			
			if (i == general_n[4]) {
				if (sum >= general_n[1]) { 
					if ((x = ContaPrimiScelti(vcurr, general_n[4])) < general_n[5]) {
						if (CoperturaTotale(vcurr, general_n, primi, mintermini)) {
							memcpy(vbest, vcurr, general_n[4] * sizeof(int));

							printf("\n\nPossibile soluzione trovata.\n\n");
							general_n[5] = x;	/* aggiorno il numero di essenziali */
						}
					}				
				}
			}		
		}
		
		quando ridurre sum???????
		
	}
	
	
	
	
	
	
	return;
}



/*
 * Si può fare una prima passata per applicare il Criterio di Essenzialità 
 * Il BackTracking con un numero di ramificazioni alto impiega molto tempo, così si eliminano alcuni rami 
 * Nel vettore 'mintermini' alla posizione i-esima se c'è 0 allora quel valore rientra nell'ON-set, altrimenti no 
 */
void CriterioEssenzialita(size_t *general_n, int *vcurr, int *vbest, size_t *mintermini, Implic *primi)
{
	size_t i, j, pos_essenziale;
	int s; /* --> indica quanti implicanti primi coprono l'i-esimo mintermine --> se s == 1 allora c'è un implicante essenziale */
	for (i = 0; i < general_n[7]; ++i) {
		if (mintermini[i] == 1) {
			s = 0;
			for (j = 0; j < general_n[4]; ++j) {
				if (primi[j].index[i] == 2) {
					pos_essenziale = j;
					++s;
				}
				if (s > 1)
					break;
			}

			if (s == 1) {
				vcurr[pos_essenziale] = 2;
				vbest[pos_essenziale] = 2;
				++general_n[5];
				for (j = 0; j < general_n[7]; ++j) {
					if (primi[pos_essenziale].index[j] == 2 && mintermini[j] == 1) {
						general_n[1]--;
						mintermini[j] = 2;
					}
				}
			}
		}
	}
	return;
}

/* 
 * Si considerano due implicanti primi: se imp1.n_precedenti > imp2.n_predenti allora forse imp1 copre imp2 per il criterio di
 * dominanza di riga; 
 */
void CriterioDominanzaRiga(size_t *general_n, Implic *primi)
{
	size_t i, j, k;

	for (i = 0; i < general_n[4]; ++i) {
		int n_prec_i = primi[i].n_precedenti;

		if (n_prec_i > 0) {		/* Se è negativo vuol dire che è già coperto e si può anche non controllare */
			for (j = 0; j < general_n[4]; ++j) {
				int n_prec_j = primi[j].n_precedenti;
				if (i != j && n_prec_i >= n_prec_j && n_prec_j > 0) {	/* Come sopra, se negativo allora si può non controllare */

					for (k = 0; k < general_n[7]; ++k) {
						if (primi[i].index[k] == 2 && primi[j].index[k] == 2)
							--n_prec_j;
					}

					if (n_prec_j == 0) {	/* Quindi primi[i] copriva tutti i mintermini di primi[j] e almeno 1 in più */
						/* Si marchia n_precedenti con -1 --> alla fine con un controllo */
						/* quelli con n_precedenti negativo saranno eliminati 			 */
						primi[j].n_precedenti *= -1;
					}
				}
			}
		}
	}
	j = general_n[4];
	for (i = 0; i < general_n[4]; ++i) {
		if (primi[i].n_precedenti < 0) {
			/* 
			 * Se l'implicante primo è coperto da un altro, copiamo su di esso l'implicante nell'ultima 
			 * posizione del vettore *primi, poi deallochiamo la memoria in questa posizione e decrementiamo
			 * il numero degli implicanti primi; si applica la realloc una volta conclusa la funzione.
			 */
			primi[i].n_Hamming = primi[general_n[4] - 1].n_Hamming;
			primi[i].n_precedenti = primi[general_n[4] - 1].n_precedenti;
			strcpy(primi[i].config, primi[general_n[4] - 1].config);
			memcpy(primi[i].index, primi[general_n[4] - 1].index, general_n[6] * sizeof(size_t));

			free(primi[general_n[4] - 1].config);
			free(primi[general_n[4] - 1].index);

			general_n[4]--;
			--i;
		}
	}

	return;
}

/*
 * Si costruisce un vettore che tiene traccia di quanti implicanti coprono l'i-esimo mintermine:
 * se il mintermine i ha tale numero minore del mintermine j allora si confrontano; se tutti gli implicanti che 
 * coprono mj coprono anche mi allora mj si può togliere.
 */
void CriterioDominanzaColonna(size_t *general_n, Implic *primi, size_t *mintermini)
{
	int i, j, k;
	size_t n_coperture[1024] = { 0, };

	for (i = 0; i < general_n[7]; ++i) {
		if (mintermini[i] == 1) {
			for (j = 0; j < general_n[4]; ++j) {
				if (primi[j].index[i] == 2)
					++n_coperture[i];
			}
		}
	}

	for (i = 0; i < general_n[7]; ++i) {
		if (mintermini[i] == 1) {
			for (j = 0; j < general_n[7]; ++j) {
				if (i != j && mintermini[j] == 1) {
					if (n_coperture[i] < n_coperture[j]) {
						size_t ni = n_coperture[i], nj = n_coperture[j];

						for (k = 0; k < general_n[4]; ++k) {
							if (primi[k].index[i] != 0 && primi[k].index[j] != 0) {
								if (primi[k].index[i] == primi[k].index[j])
									--ni;

								if (ni == 0)
									break;
							}
						}

						if (ni == 0) {
							mintermini[j] = 2;	/* quindi il mintermine mj sarà di sicuro coperto nel momento in cui si copre mi */
							--general_n[1];
						}
					}
				}
			}
		}
	}

	return;
}


Implic *Passo2(size_t *general_n, size_t *mintermini, Implic *primi, bool ShowWork)
{
	int i, j, k;
	size_t x, tot_mint = general_n[1], non_coperti;

	int *vcurr = calloc(general_n[4], sizeof(int));
	int *vbest = calloc(general_n[4], sizeof(int));
	i = 0;
	
	CriterioDominanzaRiga(general_n, primi);
	primi = realloc(primi, general_n[4] * sizeof(Implic));

	if (ShowWork) {
		printf("\nRiduzione implicanti con il criterio di dominanza per riga:\n");
		ScriviImplicante(general_n[4], general_n, primi, stdout);
		StampaTabella(general_n, mintermini, primi);
	}

	CriterioDominanzaColonna(general_n, primi, mintermini);
	if (ShowWork) {
		printf("\nMintermini totali (con indifferenze):\t%d\n", general_n[3]);
		printf("Mintermini totali (senza indifferenze):\t%d\n\n", tot_mint);
		
		printf("n_ mintermini coperti con il criterio di dominanza per colonna:\t%d\n", tot_mint - general_n[1]);
	}

	non_coperti = general_n[1];
	CriterioEssenzialita(general_n, vcurr, vbest, mintermini, primi);

	if (ShowWork) {
		printf("n_ mintermini coperti con il criterio di essenzialita':\t%d\n", non_coperti - general_n[1]);
		printf("n_ mintermini rimanenti da coprire:\t%d\n", general_n[1]);
		if (general_n[1] == 0)
			printf("Riduzione alla forma minima completata.\n");
		else
			printf("Inizio ricerca soluzione ottima...\n");
	}

	
	/* Se si è già arrivati a coprire interamente l'ON-set con i tre criteri non c'è bisogno 						*/
	/* di considerare anche il BackTrack e il vettore vbest riporta già la configurazione di implicanti essenziali. */
	if (general_n[1] != 0) {
		general_n[5] = INT_MAX;
		BackTrack(i, vcurr, vbest, general_n, primi, mintermini, 0);
	}

	Implic *essenziali;
	if (general_n[5] != INT_MAX) {
		essenziali = malloc(general_n[5] * sizeof(Implic));

		k = 0;
		for (j = 0; j < general_n[4]; ++j) {
			if (vbest[j] == 1 || vbest[j] == 2) {
				x = essenziali[k].n_precedenti = primi[j].n_precedenti;
				essenziali[k].index = malloc(general_n[6] * sizeof(size_t));
				memcpy(essenziali[k].index, primi[j].index, general_n[6] * sizeof(size_t));

				essenziali[k].config = malloc(general_n[0] * sizeof(char));
				memcpy(essenziali[k].config, primi[j].config, general_n[0] * sizeof(char));
				++k;
			}
		}
	}

	else {
		essenziali = NULL;
		general_n[5] = 0;
		printf("\nERRORE: non e' stata trovata alcuna configurazione di implicanti primi accettabile.\n");
		printf("Controllare algoritmo.\n");
	}


	free(vcurr);
	free(vbest);
	return essenziali;
}