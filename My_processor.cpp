#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Stack.h"
#include "Assembler.h"

#define COMPILATING

typedef struct Processor_on_stack Processor;

struct Processor_on_stack{
    Stack *stack;
    double registers[4];
};

typedef enum commands_for_processor { HLT_00,
                                      PUSH_01,
                                      ADD_02,
                                      SUB_03,
                                      MUL_04,
                                      OUT_05,
                                      SIN_06,
                                      COS_07,
                                      IN_08   } commands;

const char *EXECUTABLE_FILE = "second.xex";
const char *NAME            = "first.xax";

void start_perfomance ();

int main (int argc, const char **argv)
{
    int com = 0;
    #ifdef COMPILATING
    if (argc > 1)
    {
        processing(argv[1]);
        com = 1;
    }
    else
        processing(NAME);
    #endif

    start_perfomance();

    if (com)
        system("pause");

    return 0;
}

void start_perfomance ()
{
    Processor proc = {};

    proc.stack = stack_new(2);
    for (int i = 0; i < 4; i++)
    {
        proc.registers[i] = 0;
    }

    size_t length        = 0;
    size_t n_lines       = 0;
    double *program      = (double*) reading_file(EXECUTABLE_FILE, &length, &n_lines);
    if (!program)
    {
        printf("No memory access denied\n");
        return;
    }
    double *program_copy = program;

    for (int line = 0; line < length/sizeof(double); line++)
    {
        double val = *program_copy;

        double mode = 0;

        double val_earl = 0;
        double val_last = 0;
        program_copy++;

        switch ((int)val)
        {
            case 0:

                break;

            case 1:

                mode = *program_copy;
                program_copy++;
                line++;
                if ((int)mode)
                {
                    stack_push(&proc.stack, proc.registers[(int)mode - 1]);
                }
                else
                {
                    val_last = *program_copy;
                    program_copy++;
                    line++;
                    stack_push(&proc.stack, val_last);
                }
                stack_back(&proc.stack, &val_last);
                break;

            case 2:

                mode = *program_copy;
                program_copy++;
                line++;
                if ((int)mode)
                {
                    stack_pop(&proc.stack, &val_last);
                    proc.registers[(int)mode - 1] += val_last;
                }
                else
                {
                    stack_pop(&proc.stack, &val_last);
                    stack_pop(&proc.stack, &val_earl);

                    stack_push(&proc.stack, val_earl + val_last);
                }

                break;

            case 3:

                mode = *program_copy;
                program_copy++;
                line++;
                if ((int)mode)
                {
                    stack_pop(&proc.stack, &val_last);
                    proc.registers[(int)mode - 1] -= val_last;
                }
                else
                {
                    stack_pop(&proc.stack, &val_last);
                    stack_pop(&proc.stack, &val_earl);

                    stack_push(&proc.stack, val_earl - val_last);
                }

                break;

            case 4:

                mode = *program_copy;
                program_copy++;
                line++;
                if ((int)mode)
                {
                    stack_pop(&proc.stack, &val_last);
                    proc.registers[(int)mode - 1] *= val_last;
                }
                else
                {
                    stack_pop(&proc.stack, &val_last);
                    stack_pop(&proc.stack, &val_earl);

                    stack_push(&proc.stack, val_earl * val_last);
                }

                break;

            case 5:

                stack_pop(&proc.stack, &val_last);
                printf("%lg\n", val_last);

                break;
            case 6:

                mode = *program_copy;
                program_copy++;
                line++;
                if ((int)mode)
                {
                    proc.registers[(int)mode - 1] = sin(proc.registers[(int)mode - 1]);
                }
                else
                {
                    stack_pop(&proc.stack, &val_last);

                    stack_push(&proc.stack, sin(val_last));
                }

                break;

            case 7:

                mode = *program_copy;
                program_copy++;
                line++;
                if ((int)mode)
                {
                    proc.registers[(int)mode - 1] = cos(proc.registers[(int)mode - 1]);
                }
                else
                {
                    stack_pop(&proc.stack, &val_last);

                    stack_push(&proc.stack, cos(val_last));
                }

                break;

            case 8:

                mode = *program_copy;
                program_copy++;
                val_last = *program_copy;
                program_copy++;
                line += 2;
                proc.registers[(int)mode - 1] = val_last;

                break;

            default:

                //верни ошибку, пж
                break;
        }
    }

    free(program);
    stack_destruct(&proc.stack);
}
