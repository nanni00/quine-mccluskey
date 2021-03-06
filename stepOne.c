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

Implicant *copyImplicantsVector(size_t numInputVar, size_t numImplicants, Implicant *v)
{
	Implicant *new_v = NULL;
	if (v != NULL) {
		new_v = malloc(numImplicants * sizeof(Implicant));
		for (size_t k = 0; k < numImplicants; ++k) {
			new_v[k].n_precedenti = abs(v[k].n_precedenti);

			new_v[k].index = calloc((size_t) pow(2, (double) numInputVar), sizeof(size_t));
			new_v[k].config = malloc((numInputVar + 1) * sizeof(int));

			new_v[k].n_Hamming = v[k].n_Hamming;

			memcpy(new_v[k].index, v[k].index, (size_t) pow(2, (double) numInputVar) * sizeof(size_t));
			strcpy(new_v[k].config, v[k].config);
		}
	}
	return new_v;
}


bool repetition(size_t numImplicants, Implicant *v)
{
	for (int k = 0; k < numImplicants; ++k) {
		if (strcmp(v[numImplicants].config, v[k].config) == 0)
			return true;
	}
	return false;
}

/* Controllare che la distanza di Hamming effettiva sia 1 */
int hammingDistance(const char *config1, const char *config2)
{
	int i = 0, d = 0;
	while (config1[i] != 0 && d <= 1) {
		if (config1[i] != config2[i]) {
			/* 
			if (0) {
				if ((config1[i] != -1) && (config2[i] != -1)) {
					++d;
				}
				else if ((config1[i] == -1) || (config2[i] == -1)) {
					d = 2;
					break;
				}
			}*/
			++d;
			if (1) {
				if (d > 1)
					break;
			}
		}
		++i;
	}
	return d;
}

Implicant *stepOne(size_t *general_n, Implicant *minterms, bool ShowWork)
{
	bool finished = false;
	bool firstImplicant;
	Implicant *v_base = copyImplicantsVector(general_n[0], general_n[3], minterms);
	Implicant *v_next = NULL;

	int i, j, k;
	size_t n_implic_base = general_n[3], n_implic_next;

	while (!finished) {
        finished = true;
		n_implic_next = 0;
		v_next = NULL;

		for (i = 0; i < n_implic_base; ++i) {

            firstImplicant = true;
			for (j = i + 1; j < n_implic_base; ++j) {	// confronto tra i-esimo e j-esimo implicante

				/* 
				 * Se la somma di n_precedenti dell'i-esimo e del j-esimo implicante ?? 0 allora questi due non coprono
				 * un mintermine dell'ON-set e si possono trascurare; in caso contrario bisogna controllare se hanno Distanza
				 * di Hamming pari a 1 e se sono anche compatibili.
				 */
				size_t z = abs(v_base[i].n_precedenti) + abs(v_base[j].n_precedenti);
				if (z >= 1) {
					if (abs(v_base[i].n_Hamming - v_base[j].n_Hamming) == 1) {
						if (hammingDistance(v_base[i].config, v_base[j].config) == 1) {
                            finished = false;
                            firstImplicant = false;

							/* marchiare gli implicanti i e j */
							if (v_base[i].n_precedenti > 0)	
								v_base[i].n_precedenti *= -1;

							if (v_base[j].n_precedenti > 0)
								v_base[j].n_precedenti *= -1;

							v_next = realloc(v_next, (n_implic_next + 1) * sizeof(Implicant));

							v_next[n_implic_next].config = malloc((general_n[0] + 1) * sizeof(char));
							for (k = 0; k < general_n[0]; ++k) {
								char ki = v_base[i].config[k];
								char kj = v_base[j].config[k];

								if (ki != kj)		
									v_next[n_implic_next].config[k] = -1;
								else
									v_next[n_implic_next].config[k] = ki;
							}
							v_next[n_implic_next].config[general_n[0]] = 0;

							/* Se c'?? una ripetizione allora si elimina */
							if (repetition(n_implic_next, v_next)) {
								free(v_next[n_implic_next].config);
								v_next = realloc(v_next, n_implic_next * sizeof(Implicant));
							}

							else {
								v_next[n_implic_next].n_precedenti = (int) z;

								v_next[n_implic_next].index = calloc(general_n[6], sizeof(size_t));

								for (k = 0; k < general_n[6]; ++k)
									if (v_base[i].index[k] != 0)
										v_next[n_implic_next].index[k] = v_base[i].index[k];
									else if (v_base[j].index[k] != 0)
										v_next[n_implic_next].index[k] = v_base[j].index[k];

								/* Il numero di Hamming dell'implicante figlio ?? il minimo tra i */
								/* numeri di Hamming degli implicanti genitori. 				 */
								if (v_base[i].n_Hamming > v_base[j].n_Hamming || v_base[j].n_Hamming < 0)
									v_next[n_implic_next].n_Hamming = v_base[j].n_Hamming;
								else
									v_next[n_implic_next].n_Hamming = v_base[i].n_Hamming;

								++n_implic_next;
							}
						}
					}
				}
			}

			if (firstImplicant && v_base[i].n_precedenti > 0) {		/* se l'implicante ?? gi?? primo lo copio nel vettore */
				v_next = realloc(v_next, (n_implic_next + 1) * sizeof(Implicant));
				
				v_next[n_implic_next].n_Hamming = v_base[i].n_Hamming;
				v_next[n_implic_next].n_precedenti = v_base[i].n_precedenti;
				
				v_next[n_implic_next].index = calloc(general_n[6], sizeof(size_t));
				memcpy(v_next[n_implic_next].index, v_base[i].index, general_n[6] * sizeof(size_t));
				
				v_next[n_implic_next].config = malloc((general_n[0] + 1) * sizeof(char));
				strcpy(v_next[n_implic_next].config, v_base[i].config);

				++n_implic_next;
			}
		}

		if (!finished) {
			if (ShowWork) {
                writeImplicant(n_implic_next, general_n, v_next, stdout);
			}
			v_base = copyImplicantsVector(general_n[0], n_implic_next, v_next);
            deleteImplicantsVector(n_implic_next - 1, v_next);
			n_implic_base = n_implic_next;
		}
	}

    deleteImplicantsVector(n_implic_base, v_base);
	general_n[4] = n_implic_next;

	return v_next;
}
