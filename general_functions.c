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

    /*
     * int values for loops variables should not be converted into unsigned
     */

void deleteImplicantsVector(size_t numImplicants, Implicant *v)
{
	for (size_t i = 0; i < numImplicants; ++i) {
		free(v[i].index);
		free(v[i].config);
	}
	free(v);
}

void writeImplicant(size_t numImplicants, const size_t *general_n, Implicant *v, FILE *f)
{
	fprintf(f, "\n");
	for (int i = 0; i < numImplicants; ++i) {
		for (int j = general_n[0] - 1; j >= 0; --j) {
			char z = v[i].config[j];
			if (z == 1)
				fprintf(f, "1 ");
			else if (z == 8)
				fprintf(f, "0 ");
			else
				fprintf(f, "- ");
		}
		fprintf(f, "\t");

		for (size_t j = 0; j < general_n[6]; ++j) {
			if (v[i].index[j] == 2) //|| v[i].index[j] == 1) 
				fprintf(f, "%d, ", j);			
		}
		fprintf(f, "\n");
	}
}

void QuickSort(size_t *v, int first, int last)
{
	if (first < last) {
		int i, j, pivot;
		i = first;
		j = last;
		pivot = v[(first + last) / 2];
		do {
			while (v[i] < pivot) ++i;
			while (v[j] > pivot) --j;
			if (i <= j) {
				size_t tmp = v[i];
				v[i] = v[j];
				v[j] = tmp;
				++i;	--j;
			}
		} while (i <= j);
		QuickSort(v, first, j);
		QuickSort(v, i, last);
	}
}

/* 
 * Usando un ptr a char per la configurazione nasce un dilemma: preservare il concetto di 0-terminato 
 * sostituendo allo 0 solito un valore a caso come 8? 
 * O mantenere lo 0 nelle configurazioni creando delle stringhe ad es. 8-terminate?
 * Per ora cambiamo lo 0 dentro le configurazioni.
 */
int FindConfig_nHamming(size_t i, size_t n_var, char *i_config)
{
	int i_n_Hamming = 0;
	for (int j = n_var - 1; j >= 0; --j) {
		if (i - pow(2, j) >= 0) {
			i_config[j] = 1;
			i_n_Hamming++;
			i -= pow(2, j);
		}
		else
			i_config[j] = 8;
	}
	i_config[n_var] = 0;
	return i_n_Hamming;
}

Implicant *readMinterms(size_t *index, const size_t *minterms, size_t *general_n)
{
	Implicant *v_minterms = malloc(general_n[3] * sizeof(Implicant));

	if (general_n[3] != 0 && index != NULL) {
		for (size_t i = 0; i < general_n[3]; ++i) {
			
			size_t *i_index = calloc(general_n[6], sizeof(size_t));

			char *i_config = calloc((general_n[0] + 1), sizeof(char));
            v_minterms[i].n_Hamming = FindConfig_nHamming(index[i], general_n[0], i_config);
            v_minterms[i].config = i_config;

			/* 
			 * Nel vettore index di ogni implicante:
			 *	--> se index[i] == 0 allora index[i] è un maxtermine
			 *	--> se index[i] == 1 allora index[i] è una indifferenza
			 *	--> se index[i] == 2 allora index[i] è un mintermine 
			 *
			 *	Se index[i] è un mintermine allora il relativo n_precedenti è settato a 1
			 *	altrimenti a 0.
			 */
			if (minterms[index[i]] == 1) {
                v_minterms[i].n_precedenti = 1;
				i_index[index[i]] = 2;
			}
			else {
                v_minterms[i].n_precedenti = 0;
				i_index[index[i]] = 1;
			}

            v_minterms[i].index = i_index;
		}
	}

	return v_minterms;
}

