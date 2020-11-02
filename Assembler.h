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
    long int  where;
    char     *mark_name;
};

typedef struct assembling_mark Marker;

#define MAX_COM_LEN "5"
const int MAX_SYMB      = 256;
const int MAX_REG       = 4;

const int BEGINNING     = 0;
const int MIDDLE        = 1;

const int PUSH_VAL      = 0;
const int PUSH_ADDRESS  = 1;

const int STRING        = 0;
const int ERROR_READ    = 1;
const int ADDRESS       = 2;

const int ONE_ARG       = 1;
const int TWO_ARGS      = 2;
const int TWO_ARGS_JUMP = 9;
const int THREE_ARGS    = 10;

const int FIND_MARKS    = 0;
const int CHECK_MARKS   = 1;
const int FINAL_WRITE   = 2;

const int ONLY_VAL      = 0;
const int REG_RAX       = 1;
const int REG_RBX       = 2;
const int REG_RCX       = 3;
const int REG_RDX       = 4;
const int NO_REG_JUMP   = 5;
const int ADDRSS        = 6;
const int NOTHING       = 7;

#define DEFINE_COMMANDS(name, number, arg, coding, discoding)                                                    \
    if (!strcmp(command, #name))                                                                                 \
    {                                                                                                            \
        char code     = COM_##name;                                                                              \
        char mode     = 0;                                                                                       \
                                                                                                                 \
        if (just_check == FINAL_WRITE)                                                                           \
        {                                                                                                        \
            fwrite(&code, sizeof(char), 1, out);                                                                 \
        }                                                                                                        \
                                                                                                                 \
        if (arg > 0)                                                                                             \
        {                                                                                                        \
            error = translate_arg(out, list_file, &input, &command, &code, &address, just_check, marks, amount); \
            if (error != ASM_OK)                                                                                 \
                break;                                                                                           \
        }                                                                                                        \
        else                                                                                                     \
        {                                                                                                        \
            if (just_check == FINAL_WRITE)                                                                       \
            {                                                                                                    \
                listing(list_file, address, code, mode, 0, 0, command, NULL, -1);                                \
            }                                                                                                    \
            address += ONE_ARG;                                                                                  \
        }                                                                                                        \
                                                                                                                 \
        if (just_check == FINAL_WRITE)                                                                           \
        {                                                                                                        \
            error = check_trash(&input);                                                                         \
            if (error != ASM_OK)                                                                                 \
                break;                                                                                           \
        }                                                                                                        \
                                                                                                                 \
        flag = 0;                                                                                                \
    }

#define ALL_ASSERTIONS           \
    assert(input      != NULL);  \
    assert(out        != NULL);  \
    assert(list_file  != NULL);

/*!
Reads the text part of code
@param[in]  **str                   The buffer to read from
@param[in]   *res                   The buffer to write in
@param[in]    flag                  The flag that is 0 if the function reads from the beginnig of the line
                                    in the primary code and 1 if else

Returns       0                     If the reading went correctly\n
              1                     If nothing was read
*/
int        read_string         (char **str, char *res, int flag);

/*!
Checks what kind of register was read
@param[in]   *reg                   The string with the register

Returns       1                     If it was RAX\n
              2                     If it was RBX\n
              3                     If it was RCX\n
              4                     If it was RDX\n
              5                     If the format of input was wrong
*/
double     check_reg           (char *reg);

/*!
Makes out a header for listing
@param[in]   *list_file                  A file to write in
*/
void       list_header         (FILE *list_file);

/*!
Makes out a listing for each command
@param[in]   *list_file                  A file to write in
@param[in]    address               Address of the command in "machine code"
@param[in]    code                  A code of command
@param[in]    mode                  A mode of the command (0 if no registers, 1-4 if there is a register)
@param[in]    args                  Number of arguments for the command
@param[in]    value                 User's value
@param[in]   *command               The name of the command
@param[in]   *reg                   The name of the register
@param[in]    dir                   A directory to jump into
*/
void       listing             (FILE *list_file, long int address, char code, char mode, int args, double value, char *command, char *reg, long int dir);

/*!
Makes out a listing for each command
@param[in]   *out                   A file to write in "machine code"
@param[in]   *list_file                  A file to write in listing
@param[in]    address               Address of the command in "machine code"
@param[in]    code                  A code of command
@param[in]    mode                  A mode of the command (0 if no registers, 1-4 if there is a register)
@param[in]    args                  Number of arguments for the command
@param[in]    value                 User's value
@param[in]   *command               The name of the command
@param[in]   *reg                   The name of the register
@param[in]    add
*/
void       writing_and_listing (FILE *out, FILE *list_file, long int address, char code, char mode, double value, char *command, char *reg, long int add);

/*!
Reads the text part of code
@param[in]  **str                   The buffer to read from
@param[in]   *value                 The variable with future value in it

Returns       ASM_OK                If it was read correctly
              ASM_WRONG_NUM         If the format of input was wrong
*/
assembl_er read_value          (char **str, double *value, int code_call);

/*!
Checks if the string is the mark (label)
@param[in]   *str                   String to check
             *add                   The address to jump into
             *marks                 The array with marks
              amount_marks          The amount of marks in the array of marks

Returns       ASM_OK                If it is a mark
              ASM_WRONG_COMMAND     If it is not a mark
*/
assembl_er check_mark          (char *str, long int *add, Marker *marks, long int amount_marks);

/*!
Reads the value and checks if it was just a value or an address for ram-array
@param[in]   *out                   The file to write "machine code" into
             *list_file             The file to write listing into
            **str                   The string to read value from
             *address               The address of the command in "machine code"
              mode_push             The indicator of if the address will be read or just value
              just_check            The indicator if it is just looking for marks or needs listing and machine code
Returns       ASM_OK                If the reading went correctly\n
              ASM_WRONG_NUM         If the input of value was incorrect
*/
assembl_er read_val_for_push   (FILE *out, FILE *list_file, char **str, long int *address, int mode_push, int just_check);

/*!
Reads the mark and checks its correctness
@param[in]   *out                   A file to write "machine code" into
             *list_file             A file to write listing into
             *command               The command
             *reg                   The register
             *address               Address of the command in "machine code"
              code                  A code of the command
              mode                  A mode of the command
             *marks                 The array with marks
              amount_marks          The amount of marks in the array of marks
              just_check            The indicator if it is just looking for marks or needs listing and machine code
Returns       ASM_OK                If the reading went correctly\n
              ASM_WRONG_COMMAND     If the input was incorrect
*/
assembl_er read_mark           (FILE *out, FILE *list_file, char *command, char *reg, long int *address, char code, char mode, Marker *marks, long int amount_marks, int just_check);

/*!
Checks if the end of the line in primary code is correct
@param[in]  **str                   The buffer to read from
Returns       ASM_OK                If the reading went correctly\n
              ASM_WRONG_COMMAND     If the input was incorrect
*/
assembl_er check_trash         (char **str);

/*!
Checks if after the command goes value, register or mark
@param[in]   *out                   A file to write "machine code" into
             *list_file             A file to write listing into
            **str                   String to check
             *command               The command to check line after
             *code                  A code of the command
             *address               Address of the command in "machine code"
              just_check            The indicator if it is just looking for marks or needs listing and machine code
             *marks                 The array with marks
              amount_marks          The amount of marks in the array of marks

Returns       ASM_OK                If everything is read correctly\n
              ASM_WRONG_NUM         If a value supposed to be written, but it has a mistake\n
              ASM_WRONG_COMMAND     If the wrong command was inputted
*/
assembl_er translate_arg       (FILE *out, FILE *list_file, char **str, char **command, char *code,
                                long int *address, int just_check, Marker *marks, long int amount_marks);
/*!
Finds the marks and counts them
@param[in]   *out                   A file to write "machine code" into
             *list_file             A file to write listing into
             *input                 The buffer to read from
              n_lines               The number of lines in user's code
             *marks                 The array with marks
             *amount_marks          The amount of marks in the array of marks

Returns       ASM_OK                If everything is read correctly
              ASM_MEMORY_ERROR      If there were problems with memory
              ASM_NO_MARKS          If no marks were detected
*/
assembl_er find_marks          (FILE *out, FILE *list_file, char *input, const size_t n_lines, Marker **marks);

/*!
Constructs a structure of current mark
@param[in]   *command               The command to check line after
              len                   The length of the mark
              amount                The amount of marks in the array of marks
              address               Address of the command in "machine code"
             *marks                 The array with marks

Returns       ASM_OK                If everything is read correctly
*/
assembl_er mark_construct      (char *command, int len, long int amount, long int address, Marker *marks);

/*!
Writes all marks to the array of them
@param[in]   *out                   A file to write "machine code" into
             *list_file             A file to write listing into
             *input                 The buffer to read from
              n_lines               The number of lines in user's code
             *marks                 The array with marks
             *amount_marks          The amount of marks in the array of marks

Returns       ASM_OK                If everything is read correctly
              ASM_MEMORY_ERROR      If there were problems with memory
              ASM_WRONG_COMMAND     If there was wrong command read
*/
assembl_er parse_marks         (FILE *out, FILE *list_file, char *input, const size_t n_lines, Marker *marks, long int *amount_marks);

/*!
Assembles the primary file and makes a binary file for the processor
Reports about all conversions into "listing.txt"
@param[in]   *out                   File to put binary code into
             *list_file             File to write listing into
             *input                 The buffer to read from
              n_lines               The number of lines in user's code
             *marks                 The array with marks
             *amount_marks          The amount of marks in the array of marks

Returns       ASM_OK                If it was read correctly
              ASM_WRONG_NUM         If the format of value input is wrong
              ASM_WRONG_COMMAND     If the format of command input is wrong
              ASM_FILE_ERROR        If there is an error with openning the file
              ASM_MEMORY_ERROR      If mamory access denied
*/
assembl_er assembling          (FILE *out, FILE *list_file, char *input, const size_t n_lines, Marker *marks, long int *amount);

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
assembl_er processing          (const char *file_name);


int read_string (char **str, char *res, int flag)
{
    int counter = 0;

    if (flag)
        while (**str == ' ')
            (*str)++;
    else
        while (isspace(**str) || (**str) == ';')
        {
            while (**str != '\n')
            {
                (*str)++;
            }
            (*str)++;
        }

    char symbol = **str;
    if (symbol == '[')
        return ADDRESS;

    while (counter < MAX_SYMB && (isalnum(**str) || (**str == ':') || (**str == '#')))
    {
        *res = **str;
        res++;
        (*str)++;
        counter++;
    }
    *(res++) = '\0';

    if (!counter)
        return ERROR_READ;

    return STRING;
}

double check_reg (char *reg)
{
    if      (!strcmp(reg, "RAX"))
        return REG_RAX;
    else if (!strcmp(reg, "RBX"))
        return REG_RBX;
    else if (!strcmp(reg, "RCX"))
        return REG_RCX;
    else if (!strcmp(reg, "RDX"))
        return REG_RDX;
    else
        return NO_REG_JUMP;
}

void list_header (FILE *list_file)
{
    fprintf(list_file, "LISTING %82c\n", ' ');
    fprintf(list_file, "ADDRS|%3cSIMPLE CODE%3c|%21cBYTE CODE%22c|%4cCODE%5c\n", ' ', ' ', ' ', ' ', ' ', ' ');
    fprintf(list_file, "%95c\n", '_');
}

void listing (FILE *list_file, long int address, char code, char mode, int args, double value, char *command, char *reg, long int dir)
{
    char space = ' ';

    if (code == 0)
    {
        fprintf(list_file, "%04x | %2d %12c | %016f %33c | %5s\n",
                       (unsigned int)address, (unsigned)code, space, (double)code, space, command);
    }
    else if (!args)
    {
        fprintf(list_file, "%04x | %2d %12c | %016f %33c | %5s\n",
                       (unsigned int)address, (unsigned)code, space, (double)code, space, command);
    }
    else if (args == 1)
    {
        if (dir > 0)
            fprintf(list_file, "%04x | %2d %d %10c | %016f %016f %16c | %5s %4ld\n",
                        (unsigned int)address, code, mode, space, (double)code, (double)mode, space, command, dir);
        else
            fprintf(list_file, "%04x | %2d %d %10c | %016f %016f %16c | %5s %4s\n",
                        (unsigned int)address, code, mode, space, (double)code, (double)mode, space, command, reg);
    }
    else
    {
        fprintf(list_file, "%04x | %2d %d %10lg | %016f %016f %016f | %5s %04d %lg\n",
                        (unsigned int)address, code, mode, value, (double)code, (double)mode, value, command, mode, value);
    }
}

void writing_and_listing (FILE *out, FILE *list_file, long int address, char code, char mode, double value, char *command, char *reg, long int add)
{
    if (mode == NO_REG_JUMP)
    {
        fwrite(&add, sizeof(long long), 1, out);
        listing(list_file, address, code, mode, 1, value, command, reg, add);
    }
    else if (mode == ADDRSS)
    {
        fwrite(&mode,  sizeof(char), 1, out);
        fwrite(&value, sizeof(double), 1, out);

        listing(list_file, address, code, mode, 2, value, command, reg, -1);
    }
    else if (mode == NOTHING)
    {
        mode = 0;

        fwrite(&mode, sizeof(char), 1, out);
        listing(list_file, address, code, mode, 1, value, command, reg, -1);
    }
    else if (mode > ONLY_VAL)
    {
        fwrite(&mode, sizeof(char), 1, out);
        listing(list_file, address, code, mode, 1, value, command, reg, -1);
    }
    else if (mode == ONLY_VAL)
    {
        fwrite(&mode,  sizeof(char), 1, out);
        fwrite(&value, sizeof(double), 1, out);

        listing(list_file, address, code, mode, 2, value, command, reg, -1);
    }
}

assembl_er read_value  (char **str, double *value, int code_call)
{
    int code_val = 0;
    int trailing_index = 0;

    char symbol = EOF;

    if (code_call)
    {
        while (**str == ' ')
            (*str)++;
        sscanf(*str, "%c", &symbol);
        if (symbol != '[')
        {
            return ASM_WRONG_NUM;
        }
        (*str)++;
    }

    sscanf(*str, "%lg%n", value, &trailing_index);

    *str += trailing_index;

    while (**str == ' ')
            (*str)++;

    symbol = **str;
    if (symbol == ']' && code_call && trailing_index)
        (*str)++;
    else if (code_call)
    {
        return ASM_WRONG_NUM;
    }

    int counter = 0;

    while(**str != ';' && **str != '\n' && **str != '\0' && **str != '\r')
    {
        if (**str == ']' && !counter)
        {
            counter++;
        }
        else if (**str == ']' && counter)
        {
            code_val = 1;
            break;
        }

        if (!isspace(**str) && (**str != ']'))
        {
            code_val = ERROR_READ;
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

assembl_er check_mark  (char *str, long int *add, Marker *marks, long int amount_marks)
{
    char *copy = (char*) calloc(MAX_SYMB, sizeof(char));
    for (int j = 0; j < MAX_SYMB; j++)
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

assembl_er read_val_for_push (FILE *out, FILE *list_file, char **str, long int *address, int mode_push, int just_check)
{
    double value = 0;

    assembl_er read_val = read_value(str, &value, mode_push);

    char mode = 0;
    char code = 1;

    if (mode_push == PUSH_ADDRESS)
        mode = 6;

    if (read_val != ASM_OK)
    {
        return read_val;
    }

    char *put = "PUSH";

    if (just_check == FINAL_WRITE)
        writing_and_listing(out, list_file, *address, code, mode, value, put, NULL, 0);
    (*address) += THREE_ARGS;

    return read_val;
}

assembl_er read_mark (FILE *out, FILE *list_file, char *command, char *reg, long int *address, char code, char mode, Marker *marks, long int amount_marks, int just_check)
{
    long int add  = 0;

    assembl_er checking_mark = ASM_OK;

    if (just_check == FINAL_WRITE)
        checking_mark = check_mark(reg, &add, marks, amount_marks);

    if (checking_mark == ASM_OK)
    {
        if (just_check == FINAL_WRITE)
            writing_and_listing(out, list_file, *address, code, mode, 0, command, reg, add);
        (*address) += TWO_ARGS_JUMP;
    }
    else
    {
        free(reg);
            return ASM_WRONG_COMMAND;
    }

    free(reg);
    return checking_mark;
}

assembl_er check_trash (char **str)
{
    if (!isspace(**str) && (**str != ';') && (**str != '\r') && (**str != '\n') && (**str != '\0'))
    {
        return ASM_WRONG_COMMAND;
    }

    return ASM_OK;
}

assembl_er translate_arg(FILE *out, FILE *list_file, char **str, char **command, char *code, long int *address, int just_check, Marker *marks, long int amount_marks)
{
    double value = 0;

    int    trailing_index = 0;
    sscanf(*str, "%lg%n", &value, &trailing_index);

    assembl_er result = ASM_OK;

    if (!trailing_index)
    {
        char *reg = (char*) calloc(MAX_SYMB, sizeof(char));
        if (!reg)
            result = ASM_MEMORY_ERROR;

        char mode = ONLY_VAL;

        int  read_reg = read_string(str, reg, MIDDLE);
        if (read_reg == STRING)
        {
            mode = (char)check_reg(reg);
            if (mode > MAX_REG)
            {
                if (reg[0] == '#' && *code >= COM_JMP)
                    result = read_mark(out, list_file, *command, reg, address, *code, mode, marks, amount_marks, just_check);
                else
                {
                    free(reg);
                    result = ASM_WRONG_COMMAND;
                }
            }
            else
            {
                if (just_check == FINAL_WRITE)
                    writing_and_listing(out, list_file, *address, *code, mode, value, *command, reg, 0);
                (*address) += TWO_ARGS;
            }
        }
        else if (read_reg == ADDRESS)
            result = read_val_for_push(out, list_file, str, address, PUSH_ADDRESS, just_check);
        else if (*code == COM_POP)
        {
            if (just_check == FINAL_WRITE)
                writing_and_listing(out, list_file, *address, *code, NOTHING, value, *command, NULL, -1);
            (*address) += TWO_ARGS;
            free(reg);
        }
        else
            result = ASM_WRONG_COMMAND;
     }
     else if (*code == COM_PUSH)
     {
        result = read_val_for_push(out, list_file, str, address, PUSH_VAL, just_check);
     }
     else
        result = ASM_WRONG_COMMAND;

     return result;
}

assembl_er find_marks(FILE *out, FILE *list_file, char *input, const size_t n_lines, Marker **marks)
{
    ALL_ASSERTIONS

    assembl_er error = ASM_OK;

    long int amount  = 0;

    int just_check = FIND_MARKS;

    char *command = (char*) calloc(MAX_SYMB, sizeof(char));
    if (!command)
        return ASM_MEMORY_ERROR;

    for (unsigned int i = 0; i < n_lines; i++)
    {
        int read_com = read_string(&input, command, 0);

        if (read_com == 0)
        {
            for (int k = 0; k < MAX_SYMB; k++)
            {
                if (command[k] == ':' && command[k + 1] == '\0')
                {
                    amount++;
                    break;
                }
            }
        }
    }
    free(command);

    if (amount)
        *marks = (Marker*) calloc(amount, sizeof(Marker));
    else
        return ASM_NO_MARKS;

    return error;
}

assembl_er mark_construct(char *command, int len, long int amount, long int address, Marker *marks)
{
    command[len] = '\0';
    marks[amount].num       = amount;
    marks[amount].where     = address;
    marks[amount].mark_name = (char*) calloc(len + 1, sizeof(char));

    if (marks[amount].mark_name == NULL)
        return ASM_MEMORY_ERROR;

    int  l = 0;
    for (; l < len; l++)
        marks[amount].mark_name[l] = command[l];
    marks[amount].mark_name[l] = '\0';

    return ASM_OK;
}

assembl_er parse_marks(FILE *out, FILE *list_file, char *input, const size_t n_lines, Marker *marks, long int *amount_marks)
{
    ALL_ASSERTIONS

    assembl_er error = ASM_OK;

    char *command = (char*) calloc(MAX_SYMB, sizeof(char));
    if (!command)
        return ASM_MEMORY_ERROR;

    long int amount  = 0;
    long int address = 0;

    int just_check = CHECK_MARKS;

    for (unsigned int i = 0; i < n_lines; i++)
    {
        int read_com = read_string(&input, command, BEGINNING);

        if (read_com == 0)
        {
            int flag = 1;
            #include "Commands.h"
            long int flag_mark = 0;
            if (flag)
            {
                for (int k = 0; k < MAX_SYMB; k++)
                {
                    if (command[k] == ':')
                    {
                        flag_mark = 1;
                        error = mark_construct(command, k, amount, address, marks);

                        if (error != ASM_OK)
                            return error;

                        amount++;
                        break;
                    }
                }
            }
            if (flag && !flag_mark)
                return ASM_WRONG_COMMAND;
        }
    }
    free(command);

    *amount_marks = amount;

    return error;
}

assembl_er assembling (FILE *out, FILE *list_file, char *input, const size_t n_lines, Marker *marks, long int *amount_marks)
{
    ALL_ASSERTIONS

    long int address = 0;
    long int amount  = *amount_marks;

    int just_check = FINAL_WRITE;

    assembl_er error = ASM_OK;

    char *command = (char*) calloc(MAX_SYMB, sizeof(char));
    if (!command)
        return ASM_MEMORY_ERROR;

    for (unsigned int i = 0; i < n_lines; i++)
    {
        int read_com = read_string(&input, command, BEGINNING);
        printf("%s\n", command);

        if (read_com == 0)
        {
            int flag = 1;
            #include "Commands.h"
        }
    }

    free(command);

    return error;
}

assembl_er processing (const char *file_name)
{
    size_t length  = 0;
    size_t n_lines = 0;

    char *first = reading_file(file_name, &length, &n_lines, 1);
    if (!first)
        return ASM_FILE_ERROR;

    assembl_er err = ASM_OK;

    FILE  *out      = fopen("second.xex", "wb");

    FILE *list_file = fopen("listing.txt", "wb");
    if (!list_file)
        err = ASM_FILE_ERROR;
    list_header(list_file);

    Marker  *marks  = NULL;
    long int amount = 0;

    if (err == ASM_OK)
        err = find_marks(out, list_file, first, n_lines, &marks);

    if (err == ASM_NO_MARKS)
    {
        err = assembling(out, list_file, first, n_lines, marks, &amount);
    }
    else if (err == ASM_OK)
    {
        err = parse_marks(out, list_file, first, n_lines, marks, &amount);
        if (err == ASM_OK)
            err = assembling(out, list_file, first, n_lines, marks, &amount);
    }


    if (err != ASM_OK)
    {
        switch (err)
        {
            case ASM_OK:

                break;

            case ASM_NO_MARKS:

                break;

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

    fclose(list_file);
    fclose(out);

    return err;
}

#undef DEFINE_COMMANDS
