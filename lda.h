#pragma once
#ifndef LDA_H
#define LDA_H

typedef struct
{
	int* words;			// vector containing the word
	int* counts;		// vector containing the counts per word
	int length;			// number of unique words in a doc
	int total;			// total number of words in a doc
} document;

tyedef struct
{
	documents* docs;	
	int num_terms;		// vocab size of the corpus
	int num_docs;		// number of docs
} corpus;

typedef struct
{
	double alpha;
	double** log_prob_w;
	int num_topics;
	int num_terms;
} lda_model;

typedef struct
{
	double** class_word;
	double* class_total;
	double alpha_suffstats;
	int num_docs;
} lda_suffstats;

#endif