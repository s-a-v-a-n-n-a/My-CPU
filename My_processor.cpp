#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Consts.h"
#include "PolyStack.h"
#include "Assembler.c"
#include "Disassembler.h"

#define COMPILATING

#define DEFINE_COMMANDS(name, number, args, coding, discoding) \
        case number:                                           \
                                                               \
            coding;                                            \
                                                               \
            break;


typedef struct Processor_on_stack Processor;

struct Processor_on_stack{
    Stack  *stack;
    Stack  *funcs;
    double  registers[REGISTER_NUM];
    double *ram;
};

char *read_program     (size_t *length);

void  start_processing (char *program, size_t length);

int main (int argc, const char **argv)
{
    int        com   = 0;
    assembl_er error = ASM_OK;

    size_t length = 0;
    char  *prog   = read_program(&length);
    if (prog)
        start_processing(prog, length);

    system("pause");
    return 0;
}

char *read_program (size_t *length)
{
    *length        = 0;
    size_t n_lines = 0;
    char *program  = (char*) reading_file(EXECUTABLE_FILE, length, &n_lines, 0);
    if (!program)
    {
        printf("No memory access denied\n");
        return NULL;
    }

    return program;
}

void start_processing (char *program, size_t length)
{
    char *program_copy = program;

    Processor proc = { NULL, NULL, { 0, 0, 0, 0 }, NULL };
    proc.stack     = stack_new(START_NUMBER);
    proc.funcs     = stack_new(START_NUMBER);
    proc.ram       = (double*) calloc(RAM_MEMORY, sizeof(double));

    for (int i = 0; i < REGISTER_NUM; i++)
    {
        proc.registers[i] = 0;
    }

    for (long long rip = 0; rip < (long long)(length/sizeof(char)) - 1; rip++)
    {
        char val = *program_copy;

        char mode = 0;

        double val_earl = 0;
        double val_last = 0;

        long long jump  = 0;

        program_copy++;

        switch ((int)val)
        {
            #include "Commands.h"

            default:

                break;
        }
    }

    stack_destruct(&proc.funcs);
    stack_destruct(&proc.stack);
    free(program);
}

#undef DEFINE_COMMANDS
