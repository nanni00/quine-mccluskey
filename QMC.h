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


extern Implic *leggiMintermini(size_t *index, size_t *mintermini, size_t *general_n);
extern int FindConfig_nHamming(size_t i, size_t n_var, char *i_config);
extern void QuickSort(size_t *v, int first, int last);
extern void ScriviImplicante(size_t n_implic, size_t *general_n, Implic *v, FILE *f);
extern void DeleteVett_Implic(size_t n_implic, Implic *v);

extern Implic *Passo2(size_t *general_n, size_t *mintermini, Implic *primi, bool ShowWork);
extern Implic *Passo1(size_t *general_n, size_t *indifferenze, Implic *mintermini, bool ShowWork);

#endif QMC_H