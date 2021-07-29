#ifndef QMC_H
#define QMC_H

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS

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

typedef struct Implicant {
	int n_precedenti;
	size_t *index;
	char *config;
	int n_Hamming;
} Implicant;


// SUPPORT FUNCTIONS
extern Implicant *readMinterms(size_t *index, const size_t *minterms, size_t *general_n);
extern int FindConfig_nHamming(size_t i, size_t n_var, char *i_config);
extern void QuickSort(size_t *v, int first, int last);
extern void writeImplicant(size_t numImplicants, const size_t *general_n, Implicant *v, FILE *f);
extern void deleteImplicantsVector(size_t numImplicants, Implicant *v);

// STEP ONE FUNCTION
extern Implicant *stepOne(size_t *general_n, Implicant *minterms, bool ShowWork);

// STEP TWO FUNCTION
extern Implicant *stepTwo(size_t *general_n, size_t *minterms, Implicant *first, bool ShowWork);

#endif