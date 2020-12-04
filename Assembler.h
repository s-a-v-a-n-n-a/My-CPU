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

//#include "Enums.h"
#include "ReadingFromFile.h"
#include "Consts.h"

const int MAX_SYMB      = 256;
const int MAX_REG       = 4;

const int BEGINNING     = 0;
const int MIDDLE        = 1;

const int PUSH_VAL      = 0;
const int PUSH_ADDRESS  = 1;

const int STRING        = 0;
const int ERROR_READ    = 1;
const int ADDRESS       = 2;

const int FIND_MARKS    = 0;
const int CHECK_MARKS   = 1;
const int FINAL_WRITE   = 2;

const int ONE_ARG       = 1;
const int TWO_ARGS      = 2;
const int TWO_ARGS_JUMP = 9;
const int THREE_ARGS    = 10;

typedef enum assembler_errors
{
    ASM_OK,
    ASM_WRONG_NUM,
    ASM_WRONG_COMMAND,
    ASM_FILE_ERROR,
    ASM_MEMORY_ERROR,
    ASM_NO_MARKS
} assembl_er;

struct Assembling_label
{
    int        num;
    long long  where;
    char      *label_name;
};

typedef struct Assembling_label The_label;

struct All_labels
{
    The_label*   labels;
    long long amount;
};
typedef struct All_labels Labels;

struct Current_command
{
    char*     command;
    char*     reg;
    char      code;
    char      mode;
    int       args;
    long long directory;
    double    ram_address;
    double    value;
    int       flag;
};
typedef struct Current_command Command;

struct Assembl_stat
{
    FILE*     out;
    FILE*     list_file;
    char*     input;
    char*     input_cpy;
    size_t    n_lines;
    long long address;
    Labels*   labels;
};
typedef struct Assembl_stat Stat;

void       list_header         (FILE *list_file);
void       listing             (FILE *list_file, long long address, Command *cmd);
void       writing_and_listing (Stat *asb, Command *cmd);

int        read_string         (char **str, char *res, int flag);
assembl_er read_value          (char **str, double *value, int code_call);
assembl_er read_val_for_push   (Stat *asb, int mode_push, int just_check);
assembl_er read_label          (Stat *asb, Command *cmd, int just_check);

double     check_reg           (char *reg);
assembl_er check_label         (char *str, long long *add, Labels *labels);
assembl_er check_trash         (char **str);

assembl_er translate_arg       (Stat *asb, Command *cmd, int just_check);

assembl_er label_construct     (char *command, int len, long int amount, long long address, Labels *labels);
assembl_er label_find          (Stat* asb);
assembl_er label_parse         (Stat *asb);

assembl_er assembling          (Stat *asb);

assembl_er stat_construct      (Stat** asb, const char* file_name);
void       stat_destruct       (Stat** asb);

assembl_er assembling_control  (const char *file_name);

void       print_error         (const assembl_er error);
