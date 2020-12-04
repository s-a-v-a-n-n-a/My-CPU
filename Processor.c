#include "Processor.h"

#define DEFINE_COMMANDS(name, number, args, coding, discoding) \
        case number:                                           \
                                                               \
            coding;                                            \
                                                               \
            break;

char *read_program (size_t *length, const char *file_name)
{
    *length        = 0;
    size_t n_lines = 0;
    char *program  = (char*) reading_file(file_name, length, &n_lines, 0);
    if (!program)
    {
        printf("No memory access denied\n");
        return NULL;
    }

    return program;
}

Processor *processor_construct()
{
    Processor *proc = (Processor*)calloc(1, sizeof(Processor));
    proc->stack     = stack_new(START_NUMBER);
    proc->funcs     = stack_new(START_NUMBER);
    proc->ram       = (double*) calloc(RAM_MEMORY, sizeof(double));

    return proc;
}

void processor_destruct(Processor *proc)
{
    stack_destruct(&proc->funcs);
    stack_destruct(&proc->stack);
    free(proc->ram);

    free(proc);
}

void start_processing (char *program, size_t length)
{
    char *program_copy = program;

    Processor *proc = processor_construct();

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

    processor_destruct(proc);

    free(program);
}

#undef DEFINE_COMMANDS

