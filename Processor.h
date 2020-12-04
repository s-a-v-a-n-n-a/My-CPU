#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Consts.h"
#include "PolyStack.h"
#include "ReadingFromFile.h"

typedef struct Processor_on_stack{
    Stack  *stack;
    Stack  *funcs;

    double  registers[REGISTER_NUM];
    double *ram;
} Processor;

char      *read_program        (size_t *length, const char *file_name);

Processor *processor_construct ();
void       processor_destruct  (Processor *proc);

void      start_processing     (char *program, size_t length);
