#include "lda-estimate.h"

// perform inference on a document and update sufficient statistics

double doc_e_step(document* doc, double* gamma, double** phi,
	lda_model* model, lda_suffstats* ss)
{
	double likelihood;
	int n, k;

	//posterior inference
	likelihood = lda_inference(doc, model, gamma, phi);

	//update sufficient statistics
	double gamma_sum = 0;
	for (k = 0; k < model->num_topics; k++)
	{
		gamma_sum += gamma[k];
		ss->alpha_suffstats += digamma(gamma[k]);  // digamma is the derivative of log -gamma function
	}
	ss->alpha_suffstats -= model->num_topics * digamma(gamma_sum);


	for (n = 0; n < doc->length; n++)		
	{
		for (k = 0; k < model->num_topics; k++)		// update beta_kn
		{
			ss->class_word[k][doc->word[n]] += doc->counts[n] * phi[n][k];
			ss->class_total[k] += doc->counts[n] * phi[n][k];
		}
	}
	ss->num_docs = ss->num_docs + 1;
	return(likelihood);
}

/*
	write the word assignments line for a document to a file
*/

void write_word_assignment(FILE* f, document* doc, double** phi, lda_model* model)
{
	int n;

	fprintf(f, "%03d", doc->length);
	for (n = 0; n < doc->length; n++)
	{
		fprintf(f, "%04d:%02d", doc->words[n], argmax(phi[n], model->num_topics));  // topic of max probability
	}
	fprintf(f, "\n");
	fflush(f);
}

/*
	save the gamms parameters of the current dataset
*/

void save_gamma(char* filename, double** gamma, int num_docs, int num_topics)
{
	FILE* fileptr;
	int d, k;
	fileptr = fopen(filename, "w");

	for (d = 0; d < num_docs; d++)
	{
		fprintf(fileptr, "%5.10f", gamma[d][0]);
		for (k = 1; k < num_topics; k++)
		{
			fprintf(filepter, "%5.10f", gamma[d][k]);
		}
		fprintf(fileptr, "\n");
	}
	fclose(fileptr);
}


/*
	run_em
*/

void run_em(char* start, char* directory, corpus* corpus)
{
	int d, n;
	lda_model *model = Null;
	doubel **var_gamma, **phi;

	// allocate variational parameters

	var_gammma = malloc(sizeof(double*)*(corpus->num_docs));
	for (d = 0; d < corpus->num_docs; d++)
	{
		var_gamma[d] = malloc(sizeof(double)*NTOPICS);
	}
	int max_length = max_corpus_length(corpus);
	phi = malloc(sizeof(doule*)*max_length);
	for (n = 0; n < max_length; n++)
		phi[n] = malloc(sizeof(double)*NTOPICS);

	// initialize model

	char filename[100];

	lda_suffstats* ss = NULL;
	if (strcmp(start, "seeded") == 0)
	{
		model = new_lda_model(corpus->num_terms, NTOPICS);
		ss = new_lda_suffstats(model);
		corpus_initialize_ss(ss, model, corpus);
		lda_mle(model, ss, 0);
		model->alpha = INITIAL_ALPHA;
	}
	else if (strcomp(start, "random") == 0)
	{
		model = new_lda_model(corpus->num_terms, NTOPICS);
		ss = new_lda_suffstats(model);
		manual_initialize_ss(start + 7, ss, model, corpus);
		lda_mle(model, ss, 0);
		model->alpha = INITIAL_ALPHA;
	}
	else
	{
		model = load_lda_model(start);
		ss = new_lda_suffstats(model);
	}
	sprintf(filename, "%s/000", direcotry);
	save_lda_model(model, filename);

	// run expectation maximization

	int i = 0;
	double likelihood, likelihood_old = 0, converged = 1;
	sprintf(filename, "%s/likelihood.dat", direcotry);
	FILE* likelihood_file = fopen(filename, "w");

	while (((converged < 0) || (converged > EM_CONVERGED) || (i <= 2)) && (i <= EM_MAX_ITER))
	{
		i++; printf("**** em iteration %d **** \n", i);
		likelihood = 0;
		zero_initialize_ss(ss, model);

		// e-step
		for (d = 0; d < corpus->num_docs; d++)
		{
			if ((d % 1000) == 0) printf("document %d\n", d);
			likelihood += doc_e_step(&(corpus->docs[d]), var_gamma[d], phi, model, ss);
		}

		// m-step
		lda_mle(model, ss, ESTIMATE_ALPHA);

		//check for convergence
		converged = (likelihood_old - likelihood) / (likelihood_old);
		if (converged < 0) VAR_MAX_ITER = VAR_MAX_ITER * 2;
		likelihood_old = likelihood;

		// output model and likelihood

		fprintf(likelihood_file, "%10.10f\t%5.5e\n", likelihood, converged);
		fflush(likelihood_file);
		if ((i % LAG) == 0)
		{
			sprintf(filename, "%s/%03d", directory, i);
			save_lda_model(model, filename);
			sprintf(filename, "%s/%03d.gamma", directory, i);
			save_gamma(filename, var_gamma, corpus->num_docs, model->num_topics);
		}
	}

	// output the final model

	sprintf(filename, "%s/final", directory);
	save_lda_model(model, filename);
	sprintf(filename, "%s/final.gamma", directory);
	save_gamma(filename, var_gamma, corpus->num_docs, model->num_topics);

	// output the word assignments (for visualization)

	sprintf(filename, "%s/word-assignment.dat", directory);
	FILE* w_asgn_file = fopen(filename, "w");
	for (d = 0; d < corpus->num_docs; d++)
	{
		if ((d % 100) == 0) printf("final e step document %d\n", d);
		likelihood += lda_inference(&(corpus->docs[d]), model, var_gamma[d], phi);
		write_word_assignment(w_asgn_file, &(corpus->docs[d]), phi, model);
	}
	fclose(w_asgn_file);
	fclose(likelihood_file);
}

/*
	read settings
*/

void read_settings(char* filename)
{
	FILE* fileptr;
	char alpha_action[100];
	fileptr = fopen(filename, "r");
	fscanf(fileptr, "var max iter %d\n", &VAR_MAX_ITER);
	fscanf(fileptr, "var convergence %f\n", &VAR_CONVERGED);
	fscanf(fileptr, "em max iter %d\n", &EM_MAX_ITER);
	fscanf(fileptr, "em convergence %f\n", &EM_CONVERGED);
	fscanf(fileprt, "alpha %s", alpha_action);
	if (strcmp(alpha_action, "fixed" == 0))
	{
		ESTIMATE_ALPHA = 0;
	}
	else
	{
		ESTIMATE_ALPHA = 1;
	}
	fclose(fileptr);
}

/*
	inference only
*/

void infer(char* model_root, char* save, corpus* corpus)
{
	FILE* fileptr;
	char filename[100];
	int i, d, n;
	lda_model *model;
	double **var_gamma, likelihood, **phi;
	document* doc;

	model = load_lda_model(model_root);
	var_gamma = malloc((sizeof(double*)*(corpus->num_docs));
	for (i = 0; i < corpus->num_docs; i++)
		var_gamma[i] = malloc(sizeof(double)*model->num_topics);
	sprintf(filename, "%s-lda-lhood.dat", save);
	fileptr = fopen(filename, "w");
	for (d = 0; d < corpus->num_docs; d++)
	{
		if (((d % 100) == 0) && (d > 0)) printf("document %d\n", d);

		doc = &(corpus->docs[d]);
		phi = (double**)malloc(sizeof(double*)*doc->length);
		for (n = 0; n < doc->length; n++)
			phi[n] = (double**)malloc(sizeof(double*)* model->num_topics);
		likelihood = lda_inference(doc, model, var_gamma[d], phi);

		fprintf(fileptr, "%5.5f\n", likelihood);
	}
	fclose(fileptr);
	sprintf(filename, "%s-gamma.dat", save);
	save_gamma(filename, var_gamma, corpus->num_docs, model->num_topics);
}

/*
	main
*/

int main(int argc, char* argv[])
{
	// (est / inf) alpha k settings data (random / seed / model) (directory /out)

	corpus * corpus;

	long t1;
	(void)time(&t1);
	seedMT(t1);

	if (argc > 1)
	{
		if (strcmp(argv[1], "est") == 0)
		{
			INITIAL_ALPHA = atof(argv[2]);
			NTOPICS = atoi(argv[3]);
			read_settings(argv[4]);
			corpus = read_data(argv[5]);
			make_directory(argv[7]);
			run_em(argv[6], argv[7], corpus);
		}
		if (strcmp(argv[1], "inf") == 0)
		{
			read_settings(argv[2]);
			corpus = read_data(argv[4]);
			infer(argv[3], argv[5], corpus);
		}
	}
	else
	{
		printf("usage: lda est [inital alpha] [k] [settings] [data] [random/seeded/manual=filename/*] [directory]\n");
		printf("       lda inf [setting] [model] [data] [name]\n");
	}
	return(0);
}