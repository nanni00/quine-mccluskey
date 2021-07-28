#ifndef QMC_H
#define QMC_H

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS


typedef struct Implic {
	int n_precedenti;
	size_t *index;
	char *config;
	int n_Hamming;
} Implic;


extern Implic *readMinterms(size_t *index, const size_t *mintermini, size_t *general_n);
extern int FindConfig_nHamming(size_t i, size_t n_var, char *i_config);
extern void QuickSort(size_t *v, int first, int last);
extern void writeImplicant(size_t n_implic, const size_t *general_n, Implic *v, FILE *f);
extern void deleteImplicantsVector(size_t n_implic, Implic *v);

extern Implic *stepTwo(size_t *general_n, size_t *mintermini, Implic *primi, bool ShowWork);
extern Implic *stepOne(size_t *general_n, size_t *indifferenze, Implic *mintermini, bool ShowWork);

#endif