#include "lda-data.h"

corpus* read_data(char* data_filename)
{
	FILE *fileptr;
	int length, count, word, n, nd, nw;
	corpus* c;

	printf("reading data from %s\n", data_filename);
	c = malloc(sizeof(corpus));
	c->docs = 0;
	c->num_terms = 0;
	c->num_docs = 0;
	fileptr = fopen(data_filename, "r");
	nd = 0; nw = 0;
	while ((fscanf(fileptr, "%10d", &length) != EOF))
	{
		c->docs = (document*)realloc(c->docs, sizeof(document)*(nd + 1)); // what is the meaning
		c->docs[nd].length = length;
		c->docs[nd].total = 0;
		c->docs[nd].words = malloc(sizeof(int)*length);
		c->docs[nd].counts = malloc(sizeof(int)*length);
		for (n = 0; n < length; n++)
		{
			fscanf(fileptr, "%10d:%10d", &word, &count);
			word = word - OFFSET;
			c->docs[nd].words[n] = word;		// doc nd 's nth word
			c->docs[nd].counts[n] = count;      // doc nd 's nth count, e.g. how many time it appears
			c->docs[nd].total += count;         // doc nd total words
			if (word >= nw) { nw = word + 1; }
		}
		nd++;
	}
	fclose(fileptr);
	c->num_docs = nd;
	c->num_terms = nw;
	printf("number of docs    : %d\n", nd);     // nd is the total num of docs 
	printf("number of terms   : %d\n", nw);     // nw is corpus vocab size
	return(c);
}

int max_corpus_length(corpus* c)  // max number of unique words per doc in the whole corpus
{
	int n, max = 0;
	for (n = 0; n < c->num_docs; n++)
		if (c->docs[n].length > max) max = c->docs[n].length;
	return(max)
}