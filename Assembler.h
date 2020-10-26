/**
    \file
    File with functions for compiling user's code

    Thank you for using this program!
    \warning Assembler works only with documentated code doubles \n
    \authors Anna Savchuk
    \note    All the information about compilled file is in the file "listing.txt"\n
    \date    Last update was 10.22.20 at 14:07
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "Enums.h"
#include "Reading_from_file.h"

struct assembling_mark{
    int       num;
    long long where;
    char     *mark_name;
};

typedef struct assembling_mark Marker;

#define MAX_COM_LEN "5"
const int MAX_COM = 5;
const int MAX_REG = 4;

#define DEFINE_COMMANDS(name, number, arg, coding)                                                 \
    if (!strcmp(command, #name))                                                                   \
    {                                                                                              \
        code = COM_##name;                                                                         \
        if (j == 2)                                                                                \
        {                                                                                          \
            fwrite(&code, sizeof(char), 1, out);                                                   \
        }                                                                                          \
                                                                                                   \
        if (arg > 0)                                                                               \
        {                                                                                          \
            error = translate_arg(&first, &command, &code, out, list, &address, j, marks, amount); \
            if (error != ASM_OK)                                                                   \
            {                                                                                      \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            if (j == 2)                                                                            \
            {                                                                                      \
                listing(list, address, code, mode, 0, 0, command, NULL, -1);                       \
            }                                                                                      \
            address++;                                                                             \
        }                                                                                          \
                                                                                                   \
        if (j == 2)                                                                                \
        {                                                                                          \
            error = check_trash(&first);                                                           \
            if (error != ASM_OK)                                                                   \
            {                                                                                      \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        flag = 0;                                                                                  \
    }

/*!
Reads the text part of code
@param[in]  **str                   The buffer to read from
@param[in]   *res                   The buffer to write in
@param[in]    flag                  The flag that is 0 if the function reads from the beginnig of the line
                                    in the primary code and 1 if else

Returns       0                     If the reading went correctly\n
              1                     If nothing was read
*/
int        read_string (char **str, char *res, int flag);

/*!
Checks what kind of register was read
@param[in]   *reg                   The string with the register

Returns       1                     If it was RAX\n
              2                     If it was RBX\n
              3                     If it was RCX\n
              4                     If it was RDX\n
              5                     If the format of input was wrong
*/
double     read_regis  (char *reg);

/*!
Makes out a header for listing
@param[in]   *list                  A file to write in
*/
void       list_header (FILE *list);

/*!
Makes out a listing for each command
@param[in]   *list                  A file to write in
@param[in]    address               Address of the command in "machine code"
@param[in]    code                  A code of command
@param[in]    mode                  A mode of the command (0 if no registers, 1-4 if there is a register)
@param[in]    args                  Number of arguments for the command
@param[in]    value                 User's value
@param[in]   *command               The name of the command
@param[in]   *reg                   The name of the register
@param[in]    dir                   A directory to jump into
*/
void       listing     (FILE *list, long int address, char code, char mode, int args, double value, char *command, char *reg, long int dir);

/*!
Reads the text part of code
@param[in]  **str                   The buffer to read from
@param[in]   *value                 The variable with future value in it

Returns       ASM_OK                If it was read correctly
              ASM_WRONG_NUM         If the format of input was wrong
*/
assembl_er read_value  (char **str, double *value);

/*!
Checks if the end of the line in primary code is correct
@param[in]  **str                   The buffer to read from

Returns       ASM_OK                If the reading went correctly\n
              ASM_WRONG_COMMAND     If the input was incorrect
*/
assembl_er check_trash (char **str);

/*!
Checks if the string is the mark (label)
@param[in]   *str                   String to check
             *add                   The address to jump into
             *marks                 The array with marks
              amount_marks          The amount of marks in the array of marks

Returns       ASM_OK                If it is a mark
              ASM_WRONG_COMMAND     If it is not a mark
*/
assembl_er check_mark  (char *str, long int *add, Marker *marks, long int amount_marks);

/*!
Checks if after the command goes value, register or mark
@param[in]  **str                   String to check
             *command               The command to check line after
             *code                  A code of the command
             *out                   A file to write "machine code" into
             *list                  A file to write listing into
             *address               Address of the command in "machine code"
              just_check            The indicator if it is just looking for marks or needs listing and machine code
             *marks                 The array with marks
              amount_marks          The amount of marks in the array of marks

Returns       ASM_OK                If everything is read correctly
              ASM_WRONG_NUM         If a value supposed to be written, but it has a mistake
              ASM_WRONG_COMMAND     If the wrong command was inputted
*/
assembl_er translate_arg(char **str, char **command, char *code, FILE *out, FILE *list,
                         long int *address, int just_check, Marker *marks, long int amount_marks);

/*!
Assembles the primary file and makes a binary file for the processor
Reports about all conversions into "listing.txt"
@param[in]  **out                   File to put binary code into
             *first                 The buffer to read from
              n_lines               The number of lines in user's code

Returns       ASM_OK                If it was read correctly
              ASM_WRONG_NUM         If the format of value input is wrong
              ASM_WRONG_COMMAND     If the format of command input is wrong
              ASM_FILE_ERROR        If there is an error with openning the file
              ASM_MEMORY_ERROR      If mamory access denied
*/
assembl_er assembling  (FILE *out, char *input, const size_t n_lines);

/*!
Assembles the primary file and makes a binary file for the processor
Reports about all conversions into "listing.txt"
@param[in]   *file_name             The name of file to read from

Returns       ASM_OK                If it was read correctly
              ASM_WRONG_NUM         If the format of value input is wrong
              ASM_WRONG_COMMAND     If the format of command input is wrong
              ASM_FILE_ERROR        If there is an error with openning the file
              ASM_MEMORY_ERROR      If mamory access denied
*/
assembl_er processing  (const char *file_name);


int read_string (char **str, char *res, int flag)
{
    int counter = 0;

    if (flag)
        while (**str == ' ')
            *(*str)++;
    else
        while (isspace(**str) || (**str) == ';')
        {
            while (**str != '\n')
            {
                *(*str)++;
            }
            *(*str)++;
        }

    while (counter < MAX_COM && (isalnum(**str) || (**str == ':') || (**str == '#')))//убрать константу максимума
    {
        *res = **str;
        res++;
        (*str)++;
        counter++;
    }
    *(res++) = '\0';

    if (!counter)
        return 1;

    return 0;
}

double read_regis (char *reg)
{
    if      (!strcmp(reg, "RAX"))
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

void list_header (FILE *list)
{
    fprintf(list, "LISTING %82c\n", ' ');
    fprintf(list, "ADDRS|%3cSIMPLE CODE%3c|%21cBYTE CODE%22c|%4cCODE%5c\n", ' ', ' ', ' ', ' ', ' ', ' ');
    fprintf(list, "%95c\n", '_');
}

void listing (FILE *list, long int address, char code, char mode, int args, double value, char *command, char *reg, long int dir)
{
    if (code == 0)
    {
        fprintf(list, "%04x | %2d %12c | %016llx %33c | %5s\n",
                       address, code, ' ', (double)code, ' ', command);
    }
    else if (!args)
    {
        fprintf(list, "%04x | %2d %12c | %016llx %33c | %5s\n",
                       address, code, ' ', (double)code, ' ', command);
    }
    else if (args == 1)
    {
        if (dir > 0)
            fprintf(list, "%04x | %2d %ld %10c | %016llx %016llx %16c | %5s %4d\n",
                        address, code, mode, ' ', (double)code, (double)mode, ' ', command, dir);
        else
            fprintf(list, "%04x | %2d %ld %10c | %016llx %016llx %16c | %5s %4s\n",
                        address, code, mode, ' ', (double)code, (double)mode, ' ', command, reg);
    }
    else
    {
        fprintf(list, "%04x | %2d %d %10lg | %016llx %016llx %016llx | %5s %04d %lg\n",
                        address, code, mode, value, (double)code, (double)mode, value, command, mode, value);
    }
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
        return ASM_WRONG_NUM;
    }

    return ASM_OK;
}

assembl_er check_trash (char **str)
{
    int counter = 0;

    if (!isspace(**str) && (**str != ';') && (**str != '\r') && (**str != '\n') && (**str != '\0'))
    {
        return ASM_WRONG_COMMAND;
    }

    return ASM_OK;
}

assembl_er check_mark  (char *str, long int *add, Marker *marks, long int amount_marks)
{
    char *copy = (char*) calloc(MAX_COM, sizeof(char));
    for (int j = 0; j < MAX_COM; j++)
    {
        copy[j] = str[j + 1];
        if (str[j + 1] == '\0')
            break;
    }

    int flag_mark = 0;

    for (long int j = 0; j < amount_marks; j++)
    {
        if (!strcmp(copy, marks[j].mark_name))
        {
            flag_mark = 1;
            *add      = marks[j].where;
            break;
        }
    }

    free(copy);

    if (flag_mark)
    {
        return ASM_OK;
    }
    else
    {
        return ASM_WRONG_COMMAND;
    }
}

assembl_er translate_arg(char **str, char **command, char *code, FILE *out, FILE *list, long int *address, int just_check, Marker *marks, long int amount_marks)
{
    int    mode           = 0;
    double value          = 0;
    int    trailing_index = 0;

    char *reg = (char*) calloc(MAX_COM, sizeof(char));
    if (!reg)
    {
        return ASM_MEMORY_ERROR;
    }

    sscanf(*str, "%lg%n", &value, &trailing_index);
    if (!trailing_index)
    {
        assembl_er error = ASM_OK;

        int read_reg = read_string(str, reg, 1);
        if (!read_reg)
        {
            mode = read_regis(reg);
            if (mode > MAX_REG)
            {
                if (reg[0] == '#' && *code >= 12)
                {
                    long int add  = 0;

                    assembl_er checking_mark = ASM_OK;

                    if (just_check == 2)
                        checking_mark = check_mark(reg, *address, &add, marks, amount_marks);

                    if (checking_mark == ASM_OK)
                    {
                        if (just_check == 2)
                        {
                            fwrite(&add, sizeof(long long), 1, out);
                            listing(list, *address, *code, mode, 1, value, *command, reg, add);
                        }
                        (*address) += 9;
                    }
                    else
                    {
                        free(reg);
                        return ASM_WRONG_COMMAND;
                    }

                    free(reg);
                    return checking_mark;
                }
                else
                {
                    free(reg);
                    return ASM_WRONG_COMMAND;
                }
            }
        }

        if (just_check == 2)
        {
            if (mode > 0 && mode < 12)
            {
                fwrite(&mode, sizeof(char), 1, out);
                listing(list, *address, *code, mode, 1, value, *command, reg, -1);
            }
            else if (!mode)
            {
                listing(list, *address, *code, mode, 1, value, *command, NULL, -1);
            }
        }
        (*address) += 2;
     }
     else if (*code == 1)
     {
        assembl_er read_val = read_value(str, &value);

        if (read_val != ASM_OK)
        {
            free(reg);
            return read_val;
        }
        mode = 0;

        if (just_check == 2)
        {
            fwrite(&mode,  sizeof(char), 1, out);
            fwrite(&value, sizeof(double), 1, out);

            listing(list, *address, *code, mode, 2, value, *command, reg, -1);
        }
        (*address) += 10;
      }
      else
      {
        free(reg);
        return ASM_WRONG_COMMAND;
      }

      free(reg);

      return ASM_OK;
}

assembl_er assembling (FILE *out, char *input, const size_t n_lines)
{
    assert(input != NULL);
    assert(out   != NULL);

    FILE *list = fopen("listing.txt", "wb");
    list_header(list);
    long int address = 0;

    long int amount = 0;

    assembl_er error = ASM_OK;

    char symb = EOF;

    char *command = (char*) calloc(MAX_COM, sizeof(char));
    if (!command)
    {
        error = ASM_MEMORY_ERROR;
        return error;
    }

    Marker *marks = NULL;

    for (int j = 0; j < 3; j++)
    {
        char *first = input;
        for (long int i = 0; i < n_lines - 1; i++)
        {
            int read_com = read_string(&first, command, 0);

            char   code     = 0;
            char   mode     = 0;
            double value    = 0;

            int flag = 1;

            if (read_com == 0)
            {
                #include "Commands.h"
                long int flag_mark = 0;
                if (flag)
                {
                    for (int k = 0; k < MAX_COM; k++)
                    {
                        if (command[k] == ':')
                        {
                            flag_mark = 1;
                            if (j == 1)
                            {
                                command[k] = '\0';

                                marks[amount].num       = amount;
                                marks[amount].where     = address;
                                marks[amount].mark_name = (char*) calloc(k + 1, sizeof(char));
                                int l = 0;
                                for (l = 0; l < k; l++)
                                    marks[amount].mark_name[l] = command[l];
                                marks[amount].mark_name[l] = '\0';
                            }
                            amount++;
                            break;
                        }
                    }

                    if (!flag_mark)
                    {
                        error = ASM_WRONG_COMMAND;
                        break;
                    }
                }
            }
        }
        if (!j)
        {
            if (!amount)
            {
                j++;
            }
            else
            {
                marks = (Marker*) calloc(amount, sizeof(Marker));
            }
        }
        address = 0;

        if (!j)
            amount = 0;
    }

    free(command);
    if (amount)
        free(marks);

    fclose(list);

    return error;
}

assembl_er processing (const char *file_name)
{
    size_t length  = 0;
    size_t n_lines = 0;

    char *first = reading_file(file_name, &length, &n_lines);
    if (!first)
        return ASM_FILE_ERROR;

    size_t     length_second = 0;
    size_t     len_code      = 0;

    FILE      *out = fopen("second.xex", "wb");
    assembl_er err = assembling(out, first, n_lines);

    if (err != ASM_OK)
    {
        switch (err)
        {
            case ASM_WRONG_NUM:

                printf("Undefined input\n");
                break;

            case ASM_WRONG_COMMAND:

                printf("Undefined command\n");
                break;

            case ASM_FILE_ERROR:

                printf("No file found\n");
                break;

            case ASM_MEMORY_ERROR:

                printf("Memory access denied\n");
                break;

            default:

                break;
        }
    }

    free(first);
    fclose(out);

    return err;
}

#undef DEFINE_COMMANDS
