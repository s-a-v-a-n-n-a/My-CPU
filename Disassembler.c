#include "Disassembler.h"

#define DEFINE_COMMANDS(name, number, args, coding, discoding) \
        case number:                                           \
                                                               \
            discoding;                                         \
                                                               \
            break;

char *read_codes   (size_t *length)
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

long long count_labels (FILE *dis, char *program, size_t length, long long **labels_null)
{
    char *program_copy = program;

    long long  n_labels = 0;
    long long *labels   = *labels_null;

    int i = FINDING;

    #define COUNT_JUMPS   \
        n_labels++;

    #define PRINT_DISASM_JUMP

    #define PRINT_DISASM(text);

    #define PRINT_REG

    for (int rip = 0; rip < (int)(length/sizeof(char)) - 1; rip++)
    {
        char val = *program_copy;

        char mode = 0;

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

    *labels_null = (long long*) calloc((size_t)n_labels + 1, sizeof(long long));
    for (long long j = 0; j <= n_labels; j++)
        (*labels_null)[j] = -1;

    #undef COUNT_JUMPS
    #undef PRINT_DISASM_JUMPS
    #undef PRINT_DISASM
    #undef PRINT_REG

    return n_labels;
}

void write_labels (FILE *dis, char *program, size_t length, long long *labels)
{
    char *program_copy = program;

    long long  n_labels = 0;

    int i = WRITING;

    #define COUNT_JUMPS                                         \
        n_labels++;                                             \
        labels[n_labels - 1] = jump;

    #define PRINT_DISASM_JUMP

    #define PRINT_DISASM(text);

    #define PRINT_REG

    for (int rip = 0; rip < (int)(length/sizeof(char)) - 1; rip++)
    {
        char val = *program_copy;

        char mode = 0;

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

    #undef COUNT_JUMPS
    #undef PRINT_DISASM_JUMP
    #undef PRINT_DISASM
    #undef PRINT_REG
}

void disassembling (FILE *dis, char *program, size_t length, long long *labels, long long n_labels)
{
    char *program_copy = program;

    unsigned int i = DISASSEMBLING;

    #define COUNT_JUMPS

    #define PRINT_DISASM_JUMP(text);                            \
        long long index = 0;                                    \
        for (; index < n_labels; index++)                       \
            if (labels[index] == jump)                          \
            {                                                   \
                break;                                          \
            }                                                   \
        fprintf(dis, text, (int)index);


    #define PRINT_DISASM(text);    \
        fprintf(dis, text);


    #define PRINT_REG                                           \
        if      ((int)mode == REG_RAX)                          \
            fprintf(dis, "RAX\n");                              \
        else if ((int)mode == REG_RBX)                          \
            fprintf(dis, "RBX\n");                              \
        else if ((int)mode == REG_RBX)                          \
            fprintf(dis, "RCX\n");                              \
        else if ((int)mode == REG_RCX)                          \
            fprintf(dis, "RDX\n");

    for (int rip = 0; rip < (int)(length/sizeof(char)) - 1; rip++)
    {
        for (long int j = 0; j < n_labels; j++)
        {
            if (rip == labels[j])
            {
                fprintf(dis, "\%ld:\n", j);
                break;
            }
        }

        char val = *program_copy;

        char mode = 0;

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

    #undef COUNT_JUMPS
    #undef PRINT_DISASM_JUMP
    #undef PRINT_DISASM
    #undef PRINT_REG
}

#undef DEFINE_COMMANDS
