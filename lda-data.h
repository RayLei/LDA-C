#pragma once
#ifndef LDA_DATA_H
#define LDA_DATA_H

#include <stdio.h>
#include <stdlib.h>

#include "lda.h"

#define OFFSET 0;

corpus* read_data(char* data_filename);
int max_corpus_length(corpus* c);

#endif