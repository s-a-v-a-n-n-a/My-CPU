#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "Reading_from_file.h"

typedef enum assembler_errors { ASM_OK_00,
                                ASM_WRONG_NUM_01,
                                ASM_WRONG_COMMAND_02,
                                ASM_FILE_ERROR_03,
                                ASM_MEMORY_ERROR_04   } assembl_er;

#define MAX_COM_LEN "5"
const int MAX_COM = 5;
const int MAX_REG = 4;

#define CHECKING(mode, first, error)                 \
    int read_reg = read_string(&first, registr, 1); \
    if (!read_reg)                                  \
    {                                               \
        mode = read_regis(registr);                 \
        int check = check_trash(&first);            \
        if (mode > MAX_REG || check)                \
        {                                           \
            error = ASM_WRONG_COMMAND_02;           \
            break;                                  \
        }                                           \
     }                                              \
    else if (read_reg == 1)                         \
    {                                               \
        error = ASM_WRONG_COMMAND_02;               \
        break;                                      \
    }

int        read_string (char **str, char *res, int flag);

double     read_regis  (char *reg);

assembl_er read_value  (char **str, double &value);

int        check_trash (char **str);

assembl_er assembling  (FILE *out, char *first, const size_t n_lines);

assembl_er processing  (const char *file_name);


int read_string (char **str, char *res, int flag)
{
    int counter = 0;

    if (flag)
        while (**str == ' ')
            *(*str)++;
    else
        while (isspace(**str) || **str == ';')
        {
            while (**str != '\n')
            {
                *(*str)++;
            }
            *(*str)++;
        }

    while (counter < MAX_COM && isalpha(**str))
    {
        *res = **str;
        res++;
        (*str)++;
        counter++;
    }
    *(res++) = '\0';

    if (!counter)
        return 2;

    return 0;
}

double read_regis (char *reg)
{
    if (!strcmp(reg, "RAX"))
        return 1;
    else if (!strcmp(reg, "RBX"))
        return 2;
    else if (!strcmp(reg, "RCX"))
        return 3;
    else if (!strcmp(reg, "RDX"))
        return 4;
    else
        return 5;
}

assembl_er read_value  (char **str, double *value)
{
    int code_val = 0;
    int trailing_index = 0;

    sscanf(*str, "%lg%n", value, &trailing_index);

    int count = 0;
    *str += trailing_index;

    while(**str != ';' && **str != '\n' && **str != '\0' && **str != '\r')
    {
        if (!isspace(**str))
        {
            code_val = 1;
            break;
        }
            (*str)++;
    }

    if (code_val)
    {
        return ASM_WRONG_NUM_01;
    }

    return ASM_OK_00;
}

int check_trash (char **str)
{
    int counter = 0;

    if (!isspace(**str) && (**str != ';') && (**str != '\r') && (**str != '\n') && (**str != '\0'))
    {
        return 1;
    }

    return 0;
}

assembl_er assembling (FILE *out, char *first, const size_t n_lines)
{
    assert(first != NULL);
    assert(out   != NULL);

    FILE *list = fopen("listing.txt", "wb");
    long int address = 0;

    assembl_er error = ASM_OK_00;

    char symb = EOF;

    char *command = (char*) calloc(MAX_COM, sizeof(char));
    char *registr = (char*) calloc(MAX_REG, sizeof(char));
    if (command == NULL)
    {
        error = ASM_MEMORY_ERROR_04;
        return error;
    }

    for (long int i = 0; i < n_lines - 1; i++)
    {
        int read_com = read_string(&first, command, 0);

        double code  = 0;
        double mode  = 0;
        double value = 0;

        if (read_com == 0)
        {
            if (!strcmp(command, "PUSH"))
            {
                code = 1;
                fwrite(&code, sizeof(double), 1, out);

                int trailing_index = 0;
                sscanf(first, "%lg%n", &value, &trailing_index);

                if (!trailing_index)
                {
                    CHECKING(mode, first, error);
                    fwrite(&mode, sizeof(double), 1, out);

                    fprintf(list, "%04x | %lg %lg            | %016llx %016llx                  | %5s REG %lg\n",
                                   address, code, mode, code, mode, command, mode);
                    address += 2;
                }
                else
                {
                    assembl_er read_val = read_value(&first, &value);

                    if (read_val != ASM_OK_00)
                    {
                        error = read_val;
                        break;
                    }
                    mode = 0;
                    fwrite(&mode,  sizeof(double), 1, out);
                    fwrite(&value, sizeof(double), 1, out);

                    fprintf(list, "%04x | %lg %lg %10lg | %016llx %016llx %016llx | %5s REG %lg\n",
                                        address, code, mode, value, code, mode, value, command, value);
                    address += 3;
                }
            }
            else if (!strcmp(command, "IN"))
            {
                printf("Write your value: ");

                CHECKING(mode, first, error);
                if (mode)
                    code = 8;
                else
                    code = 1;

                double value = 0;
                int problem = scanf("%lg", &value);

                if (problem != 1)
                {
                    error = ASM_WRONG_NUM_01;
                    break;
                }

                fwrite(&code,  sizeof(double), 1, out);
                fwrite(&mode,  sizeof(double), 1, out);
                fwrite(&value, sizeof(double), 1, out);

                if (code == 8)
                    fprintf(list, "%04x | %lg %lg %10lg | %016llx %016llx %016llx | %5s REG %lg\n",
                                    address, code, mode, value, code, mode, value, command, value);
                else
                    fprintf(list, "%04x | %lg %lg %10lg | %016llx %016llx %016llx | %5s %lg\n",
                                    address, code, mode, value, code, mode, value, command, value);
                address += 3;
            }
            else
            {
                if (!strcmp(command, "ADD"))
                {
                    code = 2;
                    //CHECKING(mode, first, error);
                }
                else if (!strcmp(command, "SUB"))
                {
                    code = 3;
                    //CHECKING(mode, first, error);
                }
                else if (!strcmp(command, "MUL"))
                {
                    code = 4;
                    //CHECKING(mode, first, error);
                }
                else if (!strcmp(command, "OUT"))
                {
                    code = 5;
                    //CHECKING(mode, first, error);
                }
                else if (!strcmp(command, "SIN"))
                    code = 6;
                else if (!strcmp(command, "COS"))
                    code = 7;
                else if (!strcmp(command, "HLT"))
                {
                    code = 0;
                    fprintf(list, "%04x | %lg              | %016llx                                   | %5s\n",
                                    address, code, code, command);
                    fwrite(&code, sizeof(double), 1, out);
                    break;
                }
                else
                {
                    error = ASM_WRONG_COMMAND_02;
                    break;
                }
                CHECKING(mode, first, error);

                fwrite(&code, sizeof(double), 1, out);
                fwrite(&mode, sizeof(double), 1, out);

                if (!mode)
                    fprintf(list, "%04x | %lg %lg            | %016llx %016llx                  | %5s\n",
                                    address, code, mode, code, mode, command);
                else
                    fprintf(list, "%04x | %lg %lg            | %016llx %016llx                  | %5s REG %lg\n",
                                    address, code, mode, code, mode, command, mode);

                address += 2;
            }

            /*do
            {
                symb = *first;
                first++;
            }while(symb != '\n');*/  //!!!!!!!!!!!!!!!!1доделать регистры везде!!!!!!!!!!!!!!!
        }
    }

    free(command);
    free(registr);

    fclose(list);

    return error;
}

assembl_er processing (const char *file_name)
{
    size_t length  = 0;
    size_t n_lines = 0;

    char *first = reading_file(file_name, &length, &n_lines);
    if (!first)
        return ASM_FILE_ERROR_03;

    size_t     length_second = 0;
    size_t     len_code      = 0;

    FILE      *out = fopen("second.xex", "wb");
    assembl_er err = assembling(out, first, n_lines);

    if (err != ASM_OK_00)
    {
        switch (err)
        {
            case ASM_WRONG_NUM_01:

                printf("Undefined input\n");
                break;

            case ASM_WRONG_COMMAND_02:

                printf("Undefined command\n");
                break;

            case ASM_FILE_ERROR_03:

                printf("No file found\n");
                break;

            case ASM_MEMORY_ERROR_04:

                printf("Memory access denied\n");
                break;

            default:

                break;
        }
        return err;
    }

    free(first);
    fclose(out);
    return ASM_OK_00;
}
