#include "QMC.h"

    /*
       GENERAL_NUMBERS TABLE
        0 --> number input variables
        1 --> number minterms ON_SET (without dontcares)
        2 --> number dontcares
        3 --> number total minterms (with dontcares)
        4 --> number first implicants
        5 --> number essential implicants
        6 --> 2^number-input-variables
        7 --> minterm with higher index
    */

void printLine(const size_t *general_n)
{
	printf("\t");
	for (size_t i = 0; i < general_n[1]; ++i)
		printf("+-----");
	printf("+\n");
}

void printTable(size_t *general_n, const size_t *minterms, Implicant *first)
{
	size_t i, j;
	printf("\nCoverage Table:\n\n\t");
	for (i = 0; i < general_n[7]; ++i) {
		if (minterms[i] == 1) {
			if (i < 10) { printf("|    %zu", i); }
			else if (10 <= i && i < 100) { printf("|   %zu", i); }
			else if (100 <= i  && i < 1000) { printf("|  %zu", i); }
			else { printf("|%zu", i); }
		}
	}
	printf("|\n");
	for (i = 0; i < general_n[4]; ++i) {
        printLine(general_n);
		printf("p%zu\t", i);
		for (j = 0; j < general_n[6]; ++j) {
			if (minterms[j] == 1) {
				if (first[i].index[j] != 2)
					printf("|  0  ");
				else
					printf("|  2  ");
			}
		}
		printf("|\n");
	}
    printLine(general_n);

}

int countFirstChosen(const int *vcurr, size_t numFirst)
{
	int x = 0;
	for (size_t j = 0; j < numFirst; ++j)
		if (vcurr[j] == 1 || vcurr[j] == 2)		
			++x;
		
	if (x == 0)
		return INT_MAX;
	return x;
}

bool fullCoverage(int *vcurr, const size_t *general_n, Implicant *first, size_t *minterms)
{
	size_t numNotCovered = general_n[1], m = general_n[7];
	size_t check_mint[1024] = { 0, };
	memcpy(check_mint, minterms, m * 4);
	size_t j, k;

	for (j = 0; j < general_n[4]; ++j) {
		if (vcurr[j] == 1) {
			for (k = 0; k < m; ++k)
				if ((check_mint[k] == 1) && (first[j].index[k] == 2)) {
					check_mint[k] = 2;
					--numNotCovered;
				}
		}
	}
	if (1) {
		if (numNotCovered == 1) {
			printf("\n");
			for (j = 0; j < general_n[4]; ++j)
				printf("%d ", vcurr[j]);
			printf("\n");
			for (k = 0; k < general_n[7]; ++k)
				if (check_mint[k] == 1)
					printf("misses minterm %zu\n", k);
		}
	}

	if (numNotCovered == 0)
		return true;
	return false;
}

void BackTrack(int i, int *vcurr, int *vbest, size_t *general_n, Implicant *first, size_t *minterms, size_t sum)
{
	int x;
	if (i == general_n[4]) {
		if (sum >= general_n[1]) {
			if ((x = countFirstChosen(vcurr, general_n[4])) < general_n[5]) {
				if (fullCoverage(vcurr, general_n, first, minterms)) {
					memcpy(vbest, vcurr, general_n[4] * sizeof(int));

					printf("\n\nPossible solution found.\n\n");
					general_n[5] = x;	/* update essential number */
				}
			}
		}
		return;
	}
	if (vcurr[i] != 2) {			/* se vcurr[i] == 2 allora è già stato rilevato come essenziale e quindi va preso in ogni caso */
		
		vcurr[i] = 0;
		BackTrack(i + 1, vcurr, vbest, general_n, first, minterms, sum);

		vcurr[i] = 1;
		BackTrack(i + 1, vcurr, vbest, general_n, first, minterms, sum + first[i].n_precedenti);
	}
	else {
	    BackTrack(i + 1, vcurr, vbest, general_n, first, minterms, sum);
	}
}


/*
 * Si pu� fare una prima passata per applicare il Criterio di Essenzialit� 
 * Il BackTracking con un numero di ramificazioni alto impiega molto tempo, cos� si eliminano alcuni rami 
 * Nel vettore 'minterms' alla posizione i-esima se c'� 0 allora quel valore rientra nell'ON-set, altrimenti no
 */
void essentialityCriteria(size_t *general_n, int *vcurr, int *vbest, size_t *minterms, Implicant *first)
{
	size_t i, j, essentialPosition;
	int s; /* --> indica quanti implicanti first coprono l'i-esimo mintermine --> se s == 1 allora c'� un implicante essenziale */
	for (i = 0; i < general_n[7]; ++i) {
		if (minterms[i] == 1) {
			s = 0;
			for (j = 0; j < general_n[4]; ++j) {
				if (first[j].index[i] == 2) {
                    essentialPosition = j;
					++s;
				}
				if (s > 1)
					break;
			}

			if (s == 1) {
				vcurr[essentialPosition] = 2;
				vbest[essentialPosition] = 2;
				++general_n[5];
				for (j = 0; j < general_n[7]; ++j) {
					if (first[essentialPosition].index[j] == 2 && minterms[j] == 1) {
						general_n[1]--;
                        minterms[j] = 2;
					}
				}
			}
		}
	}
}

/* 
 * Si considerano due implicanti first: se imp1.n_precedenti > imp2.n_predenti allora forse imp1 copre imp2 per il criterio di
 * dominanza di riga; 
 */
void rowDominanceCriteria(size_t *general_n, Implicant *first)
{
	size_t i, j, k;

	for (i = 0; i < general_n[4]; ++i) {
		int n_prec_i = first[i].n_precedenti;

		if (n_prec_i > 0) {		/* Se � negativo vuol dire che � gi� coperto e si pu� anche non controllare */
			for (j = 0; j < general_n[4]; ++j) {
				int n_prec_j = first[j].n_precedenti;
				if (i != j && n_prec_i >= n_prec_j && n_prec_j > 0) {	/* Come sopra, se negativo allora si pu� non controllare */

					for (k = 0; k < general_n[7]; ++k) {
						if (first[i].index[k] == 2 && first[j].index[k] == 2)
							--n_prec_j;
					}

					if (n_prec_j == 0) {	/* Quindi first[i] copriva tutti i mintermini di first[j] e almeno 1 in pi� */
						/* Si marchia n_precedenti con -1 --> alla fine con un controllo */
						/* quelli con n_precedenti negativo saranno eliminati 			 */
						first[j].n_precedenti *= -1;
					}
				}
			}
		}
	}
    for (i = 0; i < general_n[4]; ++i) {
		if (first[i].n_precedenti < 0) {
			/* 
			 * Se l'implicante primo � coperto da un altro, copiamo su di esso l'implicante nell'ultima 
			 * posizione del vettore *first, poi deallochiamo la memoria in questa posizione e decrementiamo
			 * il numero degli implicanti first; si applica la realloc una volta conclusa la funzione.
			 */
			first[i].n_Hamming = first[general_n[4] - 1].n_Hamming;
            first[i].n_precedenti = first[general_n[4] - 1].n_precedenti;
			strcpy(first[i].config, first[general_n[4] - 1].config);
			memcpy(first[i].index, first[general_n[4] - 1].index, general_n[6] * sizeof(size_t));

			free(first[general_n[4] - 1].config);
			free(first[general_n[4] - 1].index);

			general_n[4]--;
			--i;
		}
	}
}

/*
 * Si costruisce un vettore che tiene traccia di quanti implicanti coprono l'i-esimo mintermine:
 * se il mintermine i ha tale numero minore del mintermine j allora si confrontano; se tutti gli implicanti che 
 * coprono mj coprono anche mi allora mj si pu� togliere.
 */
void columnDominanceCriteria(size_t *general_n, Implicant *first, size_t *minterms)
{
	int i, j, k;
	size_t n_coperture[1024] = { 0, };

	for (i = 0; i < general_n[7]; ++i) {
		if (minterms[i] == 1) {
			for (j = 0; j < general_n[4]; ++j) {
				if (first[j].index[i] == 2)
					++n_coperture[i];
			}
		}
	}

	for (i = 0; i < general_n[7]; ++i) {
		if (minterms[i] == 1) {
			for (j = 0; j < general_n[7]; ++j) {
				if (i != j && minterms[j] == 1) {
					if (n_coperture[i] < n_coperture[j]) {
						size_t ni = n_coperture[i];

						for (k = 0; k < general_n[4]; ++k) {
							if (first[k].index[i] != 0 && first[k].index[j] != 0) {
								if (first[k].index[i] == first[k].index[j])
									--ni;

								if (ni == 0)
									break;
							}
						}

						if (ni == 0) {
                            minterms[j] = 2;	/* quindi il mintermine mj sar� di sicuro coperto nel momento in cui si copre mi */
							--general_n[1];
						}
					}
				}
			}
		}
	}
}


Implicant *stepTwo(size_t *general_n, size_t *minterms, Implicant *first, bool ShowWork)
{
	int i, j, k;
	size_t tot_mint = general_n[1], non_coperti;

	int *vcurr = calloc(general_n[4], sizeof(int));
	int *vbest = calloc(general_n[4], sizeof(int));
	i = 0;

    rowDominanceCriteria(general_n, first);
    first = realloc(first, general_n[4] * sizeof(Implicant));

	if (ShowWork) {
		printf("\nImplicants reduction with row dominance criteria:\n");
        writeImplicant(general_n[4], general_n, first, stdout);
        printTable(general_n, minterms, first);
	}

    columnDominanceCriteria(general_n, first, minterms);
	if (ShowWork) {
		printf("\nTotal minterms (with dontcares):\t%zu\n", general_n[3]);
		printf("Total minterms (without dontcares):\t%zu\n\n", tot_mint);
		
		printf("n_ minterms covered with column dominance criteria:\t%llu\n", tot_mint - general_n[1]);
	}

	non_coperti = general_n[1];
    essentialityCriteria(general_n, vcurr, vbest, minterms, first);

	if (ShowWork) {
		printf("n_ minterms covered with essentiality criteria:\t%llu\n", non_coperti - general_n[1]);
		printf("n_ minterms still to cover:\t%zu\n", general_n[1]);
		if (general_n[1] == 0)
			printf("Riduzione alla forma minima completata.\n");
		else
			printf("Inizio ricerca soluzione ottima...\n");
	}

	
	/* Se si è già arrivati a coprire interamente l'ON-set con i tre criteri non c'� bisogno 						*/
	/* di considerare anche il BackTrack e il vettore vbest riporta gi� la configurazione di implicanti essentials. */
	if (general_n[1] != 0) {
		general_n[5] = INT_MAX;
		BackTrack(i, vcurr, vbest, general_n, first, minterms, 0);
	}

	Implicant *essentials;
	if (general_n[5] != INT_MAX) {
        essentials = malloc(general_n[5] * sizeof(Implicant));

		k = 0;
		for (j = 0; j < general_n[4]; ++j) {
			if (vbest[j] == 1 || vbest[j] == 2) {
                essentials[k].n_precedenti = first[j].n_precedenti;
                essentials[k].index = malloc(general_n[6] * sizeof(size_t));
				memcpy(essentials[k].index, first[j].index, general_n[6] * sizeof(size_t));

                essentials[k].config = malloc(general_n[0] * sizeof(char));
				memcpy(essentials[k].config, first[j].config, general_n[0] * sizeof(char));
				++k;
			}
		}
	}

	else {
        essentials = NULL;
		general_n[5] = 0;
		printf("\nERROR: any acceptable first implicants configuration found.\n");
		printf("Check algorithm..\n");
	}

	free(vcurr);
	free(vbest);
	return essentials;
}