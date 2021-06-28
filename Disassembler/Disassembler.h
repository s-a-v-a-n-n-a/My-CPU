#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "../FileWork/ReadingFromFile.h"
//#include "Enums.h"
#include "../Common/Consts.h"

const int FINDING       = 0;
const int WRITING       = 1;
const int DISASSEMBLING = 2;

char     *read_codes     (size_t *length);

void      disassembling  (FILE *dis, char *program, size_t length);

long long count_labels   (FILE *dis, char *program, size_t length, long long **labels);

void      write_labels   (FILE *dis, char *program, size_t length, long long *labels);

void      disassembling  (FILE *dis, char *program, size_t length, long long *labels, long long n_labels);

void      start_dis      ();
