#include "lda-model.h"

/*
	compute MLE lda model from sufficient statistics
*/

void lda_mle(lda_model* model, lda_suffstats* ss, int estimate_alpha)
{
	int k; int w;

	for (k = 0; k < model->num_topics; k++)
	{
		for (w = 0; w < model->num_terms; w++)
		{
			if (ss->calss_word[k][w] > 0)
			{
				model->log_prob_w[k][w] = log(ss->calss_word[k][w]) - log(ss->class_total[k]);
			}
			else
				model->log_prob_w[k][w] = -100;
		}
	}
	if (estimate_alpha == 1)
	{
		model->alpha = opt_alpha(ss->alpha_suffstats, ss->num_docs, model->num_topics);
		printf("new alpha = %5.5f\n", model->alpha);
	}
}

/*
	allocate sufficient statistics
*/

lda_suffstats* new_lda_suffstats(lda_model* model)
{
	int num_topics = model->num_topics;
	int num_terms = model->num_terms;
	int i, j;

	lda_suffstas* ss = malloc(sizeof(lda_suffstats));
	ss->class_total = malloc(sizzeof(double)*num_topics);
	ss->class_word = malloc(sizeof(double*)*num_topics);
	for (i = 0; i < num_topics; i++)
	{
		ss->class_total[i] = 0;
		ss->class_word[i] = malloc(sizeof(double)*num_terms);
		for (j = 0; j < num_terms; j++)
		{
			ss->class_word[i][j] = 0;
		}
	}
	return(ss);
}

/*
	various intializations for the sufficient statistics
*/

void zero_initialize_ss(lda_suffstats* ss, lda_model* model)
{
	int k, w;
	for (k = 0; k < model->num_topics; w++)
	{
		ss->class_total[k] = 0;
		for(w=0; w<model_num_terms;w++)
		{
			ss->class_word[k][w] = 0;
		}
	}
	ss->num_docs = 0;
	ss->alpha_suffstats = 0;
}

void random_initialize_ss(lda_suffstats* ss, lda_model* model)
{
	int num_topics = model->num_topics;
	int num_terms = model->num_terms;
	int k, n;
	for (k = 0; k < num_topics; k++)
	{
		for (n = 0; n < num_terms; n++)
		{
			ss->class_word[k][n] += 1.0 / num_terms + myrand();
			ss->class_total[k] += ss->class_word[k][n];
		}
	}
}

void corpus_initialize_ss(lda_suffstats* ss, lda_model* model, corpus* c)
{

}