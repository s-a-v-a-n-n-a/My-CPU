#ifndef PROCESSOR
#define PROCESSOR

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../Common/Consts.h"
#include "../FileWork/ReadingFromFile.h"
#include "PolyStack.h"

typedef struct Processor_on_stack{
    Stack  *stack;
    Stack  *funcs;

    //double  registers[REGISTER_NUM];
    //double *ram;
    long long  registers[REGISTER_NUM];
    long long *ram;
} Processor;

char      *read_program        (size_t *length, const char *file_name);

Processor *processor_construct ();
void       processor_destruct  (Processor *proc);

void      start_processing     (char *program, size_t length);

#endif
