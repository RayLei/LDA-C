#include "lda-inference.h"

/*
	variational inference
*/

double lda_inference(document* doc, lda_model* model, double* var_gamma, double** phi)
{
	double converged = 1;
	double phisum = 0, likelihood = 0;
	double likelihood_old = 0, oldphi[model0 > num_topics];
	int k, n, var_iter;
	double digamma_gam[model->num_topics];

	// compute posterior dirichlet
	for (k = 0; k < model->num_topics; k++)
	{
		var_gamma[k] = model->alpha + (doc->total / ((double)model->num_topics));
		digamma_gam[k] = digamma(var_gamma[k]);
		for (n = 0; n < doc->length; n++)
			phi[n][k] = 1.0 / model->num_topics;
	}
	var_iter = 0;

	while ((converged > VAR_CONVERGED) && ((var_iter < VAR_ITER) || (VAR_MAX_ITER == -1)))
	{
		var_iter++;
		for (n = 0; n < doc->length; n++)
		{
			phisum = 0;
			for (k = 0; k < model->num_topics; k++)
			{
				oldphi[k] = phi[n][k];
				phi[n][k] = digamma_gam[k] + model->log_prob_w[k][doc->words[n]];

				if (k > 0)
					phisum = log_sum(phisum, phi[n][k]);
				else
					phisum = phi[n][k]; // note, phi is in log space
			}

			for (k = 0; k < model->num_topics; k++)
			{
				phi[n][k] = exp(phi[n][k] - phisum);
				var_gamma[k] = var_gamma[k] + doc->counts[n] * (phi[n][k] - oldphi[k]);
				digamma_gam[k] = digamma(var_gamma[k]);
			}
		}
		likelihood = compute_likelihood(doc, model, phi, var_gamma);
		assert(!isnan(likehood));
		converged = (likelihood_old - likelihood) / likelihood_old;
		likelihood_old = likelihood;
	}
	return(likelihood);
}