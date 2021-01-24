#include "Assembler.h"

#define DEFINE_COMMANDS(name, number, arg, coding, discoding)                                                    \
    if (!strcmp(com->command, #name))                                                                            \
    {                                                                                                            \
        com->code      = COM_##name;                                                                             \
        com->mode      = 0;                                                                                      \
        com->args      = arg;                                                                                    \
        com->directory = -1;                                                                                     \
                                                                                                                 \
        BINARY_WRITE                                                                                             \
                                                                                                                 \
        if (arg > 0)                                                                                             \
        {                                                                                                        \
            error = translate_arg(asb, com, just_check);                                                         \
            if (error != ASM_OK)                                                                                 \
                break;                                                                                           \
        }                                                                                                        \
        else                                                                                                     \
        {                                                                                                        \
            LISTING                                                                                              \
            asb->address += ONE_ARG;                                                                             \
        }                                                                                                        \
        CHECK_END                                                                                                \
                                                                                                                 \
        com->flag = 0;                                                                                           \
    }

#define ALL_ASSERTIONS                \
    assert(asb             != NULL);  \
    assert(asb->input      != NULL);  \
    assert(asb->out        != NULL);  \
    assert(asb->list_file  != NULL);

#define ALL_WRITE(all_args);                \
    if (just_check == FINAL_WRITE)          \
        writing_and_listing(asb, cmd);      \
    asb->address += all_args;

void list_header (FILE *list_file)
{
    assert(list_file != NULL);

    fprintf(list_file, "%42cLISTING%41c\n", ' ', ' ');
    for (int i = 0; i < 95; i++) fputc('_', list_file);
    fputc('\n', list_file);

    fprintf(list_file, "ADDRS|%3cSIMPLE CODE%3c|%21cBYTE CODE%22c|%4cCODE%5c\n", ' ', ' ', ' ', ' ', ' ', ' ');

    for (int i = 0; i < 95; i++) fputc('_', list_file);
    fputc('\n', list_file);
}

void listing (FILE *list_file, long long address, Command *cmd)
{
    assert(list_file != NULL);
    assert(cmd       != NULL);

    char space = ' ';

    long long long_code = (long long)cmd->code;

    fprintf(list_file, "%04x | %2d ", (unsigned int)address, (unsigned)cmd->code);
    if (cmd->mode >= ADDRSS_RAX && cmd->mode <= ADDRSS_RDX)
    {
        fprintf(list_file, "%d %10c | %16llx %33c | %5s [%d]\n",
                       cmd->mode, space, long_code, space, cmd->command, cmd->mode);
    }
    else if (cmd->code == 0)
    {
        fprintf(list_file, "%12c | %16llx %33c | %5s\n",
                       space, long_code, space, cmd->command);
    }
    else if (!cmd->args)
    {
        fprintf(list_file, "%12c | %14c%02x %33c | %5s\n",
                       space, space, cmd->code, space, cmd->command);
    }
    else if (cmd->args == ONE_ARG)
    {
        long long long_mode = (long long)cmd->mode;
        if (cmd->directory > 0)
        {
            long long long_dir = (long long)cmd->directory;

            fprintf(list_file, "%d %10lld | %14c%02x %14c%02x %016llx | %5s %4lld\n",
                        cmd->mode, cmd->directory, space, cmd->code, space, cmd->mode, long_dir, cmd->command, cmd->directory);
        }
        else
        {
            fprintf(list_file, "%d %10c | %14c%02x %14c%02x %16c | %5s %4s\n",
                        cmd->mode, space, space, cmd->code, space, cmd->mode, space, cmd->command, cmd->reg);
        }
    }
    else
    {
        long long long_mode = (long long)cmd->mode;
        fprintf(list_file, "%d %10lg | %14c%02x %14c%02x %016llx | %5s %4c %lg\n",
                        cmd->mode, cmd->value, space, cmd->code, space, cmd->mode, cmd->value, cmd->command, (char)space, cmd->value);
    }
}

void writing_and_listing (Stat *asb, Command *cmd)
{
    ALL_ASSERTIONS

    if (cmd->mode >= ADDRSS_RAX && cmd->mode <= ADDRSS_RDX)
    {
        fwrite(&cmd->mode, sizeof(char), 1, asb->out);

        listing(asb->list_file, asb->address, cmd);
    }
    else if (cmd->mode == NO_REG_JUMP)
    {
        fwrite(&cmd->directory, sizeof(long long), 1, asb->out);
        listing(asb->list_file, asb->address, cmd);
    }
    else if (cmd->mode == ADDRSS)
    {
        fwrite(&cmd->mode,  sizeof(char), 1, asb->out);
        fwrite(&cmd->value, sizeof(double), 1, asb->out);

        cmd->args = TWO_ARGS;
        listing(asb->list_file, asb->address, cmd);
    }
    else if (cmd->mode == NOTHING)
    {
        cmd->mode = ONLY_VAL;

        fwrite(&cmd->mode, sizeof(char), 1, asb->out);
        listing(asb->list_file, asb->address, cmd);
    }
    else if (cmd->mode > ONLY_VAL && cmd->mode < NO_REG_JUMP)
    {
        fwrite(&cmd->mode, sizeof(char), 1, asb->out);
        listing(asb->list_file, asb->address, cmd);
    }
    else if (cmd->mode == ONLY_VAL)
    {
        fwrite(&cmd->mode,  sizeof(char), 1, asb->out);
        fwrite(&cmd->value, sizeof(double), 1, asb->out);

        listing(asb->list_file, asb->address, cmd);
    }
}

int read_string (char **str, char *res, int flag)
{
    assert(str  != NULL);
    assert(*str != NULL);
    assert(res  != NULL);

    int counter = 0;

    if (flag)
        while (**str == ' ')
            (*str)++;
    else
        while (isspace(**str) || (**str) == ';')
        {
            while (**str != '\n')
                (*str)++;
            (*str)++;
        }

    char symbol = **str;
    if (symbol == '[')
        return ADDRESS;

    while (counter < MAX_SYMB && (isalnum(**str) || (**str == ':') || (**str == '#') || (**str == '_')))
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

int read_register_for_ram(char **str)
{
    assert(str  != NULL);
    assert(*str != NULL);

    while (isspace(**str))
    {
        (*str)++;
    }
    (*str)++;

    char registerr[MAX_REG] = "";

    sscanf(*str, "%3s", registerr);

    //Тут подразумевается, что закрывающая скобка идёт СРАЗУ после регистра
    (*str) += MAX_REG;

    return check_reg(registerr);
}

assembl_er read_val_for_push (Stat *asb, int mode_push, int just_check)
{
    ALL_ASSERTIONS

    double value = 0;

    assembl_er read_val = read_value(&(asb->input_cpy), &value, mode_push);

    char   mode = ONLY_VAL;
    char   code = COM_PUSH;
    double ram  = 0;

    if (mode_push == PUSH_ADDRESS)
    {
        mode = ADDRSS;
        ram  = value;
    }

    if (read_val != ASM_OK)
    {
        return read_val;
    }

    char *put  = "PUSH";

    if (just_check == FINAL_WRITE)
    {
        Command* cmd     = (Command*) calloc(1, sizeof(Command));
        cmd->command     = put;
        cmd->reg         = NULL;
        cmd->code        = code;
        cmd->mode        = mode;
        cmd->args        = ONE_ARG + TWO_ARGS;
        cmd->directory   = 0;
        cmd->ram_address = ram;
        cmd->value       = value;
        cmd->flag        = 0;
        writing_and_listing(asb, cmd);
    }
    asb->address += THREE_ARGS;

    free(put);

    return read_val;
}

assembl_er read_label (Stat *asb, Command *cmd,  int just_check)
{
    ALL_ASSERTIONS

    assembl_er checking_label = ASM_OK;

    if (just_check == FINAL_WRITE)
        checking_label = check_label(cmd->reg, &cmd->directory, asb->labels);

    if (checking_label == ASM_OK)
    {
        ALL_WRITE(TWO_ARGS_JUMP);
    }
    else
        return ASM_WRONG_COMMAND;

    return checking_label;
}

assembl_er read_value  (char **str, double *value, int code_call)
{
    assert(str  != NULL);
    assert(*str != NULL);

    int code_val       = STRING;
    int trailing_index = 0;

    char symbol = EOF;

    if (code_call)
    {
        while (**str == ' ')
            (*str)++;
        sscanf(*str, "%c", &symbol);
        if (symbol != '[')
            return ASM_WRONG_NUM;
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
        return ASM_WRONG_NUM;

    int counter = 0;

    /*while(**str != ';' && **str != '\n' && **str != '\0' && **str != '\r')
    {
        if (**str == ']' && !counter)
        {
            counter++;
        }
        else if (**str == ']' && counter)
        {
            code_val = ERROR_READ;
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
    }  */

    return ASM_OK;
}

int check_reg (char *reg)
{
    assert(reg != NULL);

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

assembl_er check_label  (char *str, long long *add, Labels *labels)
{
    assert(str   != NULL);
    assert(labels != NULL);

    char *copy = (char*) calloc(MAX_SYMB, sizeof(char));
    for (int j = 0; j < MAX_SYMB; j++)
    {
        copy[j] = str[j + 1];
        if (str[j + 1] == '\0')
            break;
    }

    int flag_label = 0;

    for (long int j = 0; j < labels->amount; j++)
    {
        if (!strcmp(copy, labels->labels[j].label_name))
        {
            flag_label++;
            *add = labels->labels[j].where;
            break;
        }
    }

    free(copy);

    if (flag_label)
        return ASM_OK;
    else
        return ASM_WRONG_COMMAND;
}

assembl_er check_trash (char **str)
{
    assert(str  != NULL);
    assert(*str != NULL);

    while ((**str != ';') && (**str != '\r') && (**str != '\n') && (**str != '\0'))
    {
        if (!isspace(**str))
            return ASM_WRONG_COMMAND;
        (*str)++;
    }

    return ASM_OK;
}

assembl_er translate_arg (Stat *asb, Command *cmd, int just_check)
{
    ALL_ASSERTIONS
    assert(cmd != NULL);

    int trailing_index = 0;
    sscanf(asb->input_cpy, "%lg%n", &cmd->value, &trailing_index);

    assembl_er result = ASM_OK;

    if (!trailing_index)
    {
        cmd->reg = (char*) calloc(MAX_SYMB, sizeof(char));
        if (!(cmd->reg))
            return ASM_MEMORY_ERROR;

        cmd->mode = ONLY_VAL;

        int read_reg = read_string(&(asb->input_cpy), cmd->reg, MIDDLE);
        if (read_reg == STRING)
        {
            cmd->mode = (char)check_reg(cmd->reg);
            if (cmd->mode > MAX_REG)
                if (cmd->reg[0] == '#' && cmd->code >= COM_JMP)
                    result = read_label(asb, cmd, just_check);
                else
                    result = ASM_WRONG_COMMAND;
            else
            {
                cmd->args = ONE_ARG;
                ALL_WRITE(TWO_ARGS);
            }
        }
        else if (read_reg == ADDRESS)
        {
            double value;
            int read = sscanf(asb->input_cpy, "%lg", &value);
            if (read)
            {
                result = read_val_for_push(asb, PUSH_ADDRESS, just_check);
            }
            else
            {
                int return_reg = read_register_for_ram(&(asb->input_cpy));
                cmd->mode = return_reg + MINUS;
                //cmd->args = ONE_ARG;
                ALL_WRITE(TWO_ARGS);
            }
        }
        else if (cmd->code == COM_POP)
        {
            if (just_check == FINAL_WRITE)
            {
                cmd->mode = NOTHING;
                cmd->reg  = "";
                writing_and_listing(asb, cmd);
            }
            asb->address += TWO_ARGS;
        }
        else
            result = ASM_WRONG_COMMAND;

        free(cmd->reg);
     }
     else if (cmd->code == COM_PUSH)
        result = read_val_for_push(asb, PUSH_VAL, just_check);
     else
        result = ASM_WRONG_COMMAND;

     return result;
}

assembl_er label_construct(char *command, int len, long int amount, long long address, Labels *labels)
{
    labels->labels[amount].num        = amount;
    labels->labels[amount].where      = address;
    labels->labels[amount].label_name = (char*) calloc(len + 1, sizeof(char));

    if (labels->labels[amount].label_name == NULL)
        return ASM_MEMORY_ERROR;

    for (int l = 0; l < len; l++)
        labels->labels[amount].label_name[l] = command[l];
    labels->labels[amount].label_name[len] = '\0';

    return ASM_OK;
}

assembl_er label_find(Stat *asb)
{
    ALL_ASSERTIONS

    assembl_er error = ASM_OK;

    long int amount  = 0;

    Command *com = (Command*) calloc(1, sizeof(Command));
    if (!com)
        return ASM_MEMORY_ERROR;
    com->command = (char*)    calloc(MAX_SYMB, sizeof(char));
    if (!com)
    {
        free(com);
        return ASM_MEMORY_ERROR;
    }

    asb->input_cpy = asb->input;

    for (unsigned int i = 0; i < asb->n_lines; i++)
    {
        int read_com = read_string(&(asb->input_cpy), com->command, 0);

        if (read_com == 0)
            for (int k = 0; k < MAX_SYMB; k++)
                if (com->command[k] == ':' && com->command[k + 1] == '\0')
                {
                    amount++;
                    break;
                }
    }
    free(com->command);
    free(com);

    if (amount)
    {
        asb->labels         = (Labels*)calloc(1, sizeof(Labels));
        asb->labels->labels = (The_label*)calloc(amount, sizeof(The_label));
    }
    else
        return ASM_NO_MARKS;

    return error;
}

assembl_er label_parse(Stat *asb)
{
    ALL_ASSERTIONS

    assembl_er error = ASM_OK;

    Command *com = (Command*) calloc(1, sizeof(Command));
    if (!com)
        return ASM_MEMORY_ERROR;
    com->command = (char*)    calloc(MAX_SYMB, sizeof(char));
    if (!com)
    {
        free(com);
        return ASM_MEMORY_ERROR;
    }

    asb->input_cpy = asb->input;

    long int amount  = 0;
    asb->address     = 0;

    int just_check = CHECK_MARKS;

    #define BINARY_WRITE
    #define LISTING
    #define CHECK_END

    for (unsigned int i = 0; i < asb->n_lines; i++)
    {
        int read_com = read_string(&(asb->input_cpy), com->command, BEGINNING);

        if (read_com == 0)
        {
            com->flag = 1;
            #include "Commands.h"
            long int flag_label = 0;
            if (com->flag)
            {
                for (int k = 0; k < MAX_SYMB; k++)
                {
                    if (com->command[k] == ':')
                    {
                        flag_label = 1;
                        error = label_construct(com->command, k, amount, asb->address, asb->labels);

                        if (error != ASM_OK)
                            return error;

                        amount++;
                        break;
                    }
                }
            }
            if (com->flag && !flag_label)
                return ASM_WRONG_COMMAND;
        }
    }

    #undef BINARY_WRITE
    #undef LISTING
    #undef CHECK_END

    free(com->command);
    free(com);

    asb->labels->amount = amount;

    return error;
}

assembl_er assembling (Stat *asb)
{
    ALL_ASSERTIONS

    asb->address   = 0;

    int just_check = FINAL_WRITE;

    #define BINARY_WRITE fwrite(&com->code, sizeof(char), 1, asb->out);

    #define LISTING                                                                                      \
        com->directory = -1;                                                                             \
        listing(asb->list_file, asb->address, com);

    #define CHECK_END                                                                                        \
        error = check_trash(&(asb->input_cpy));                                                              \
        if (error != ASM_OK)                                                                                 \
            break;

    assembl_er error = ASM_OK;

    Command *com = (Command*) calloc(1, sizeof(Command));
    if (!com)
        return ASM_MEMORY_ERROR;
    com->command = (char*)    calloc(MAX_SYMB, sizeof(char));
    if (!com)
    {
        free(com);
        return ASM_MEMORY_ERROR;
    }

    asb->input_cpy = asb->input;

    if (!(com->command))
        return ASM_MEMORY_ERROR;

    for (unsigned int i = 0; i < asb->n_lines; i++)
    {
        int read_com = read_string(&(asb->input_cpy), com->command, BEGINNING);

        if (read_com == STRING)
        {
            #include "Commands.h"
        }
    }

    #undef BINARY_WRITE
    #undef LISTING
    #undef CHECK_END

    free(com->command);
    free(com);

    return error;
}

assembl_er stat_construct(Stat **asb, const char* file_name)
{
    assert(asb != NULL);

    size_t length = 0;
    size_t n_lines = 0;

    char* first = reading_file(file_name, &length, &n_lines, 1);
    if (!first)
        return ASM_FILE_ERROR;

    FILE* out = fopen(EXECUTABLE_FILE, "wb");
    if (!out)
        return ASM_FILE_ERROR;

    FILE* list_file = fopen(LISTING_FILE, "wb");
    if (!list_file)
        return ASM_FILE_ERROR;

    *asb = (Stat*)calloc(1, sizeof(Stat));
    if (!(*asb))
        return ASM_MEMORY_ERROR;

    Labels* labels = NULL;

    list_header(list_file);

    (*asb)->out       = out;
    (*asb)->list_file = list_file;
    (*asb)->input     = first;
    (*asb)->n_lines   = n_lines;
    (*asb)->address   = 0;
    (*asb)->labels    = labels;

    return ASM_OK;
}

void stat_destruct (Stat** asb)
{
    fclose((*asb)->out);
    fclose((*asb)->list_file);
    if ((*asb)->input)
        free((*asb)->input);
    if ((*asb)->labels)
    {
        if ((*asb)->labels->labels)
            free((*asb)->labels->labels);
        free((*asb)->labels);
    }

    free(*asb);
}

assembl_er assembling_control (const char *file_name)
{
    Stat* current = NULL;
    assembl_er err = stat_construct(&current, file_name);

    if (err == ASM_OK)
    {
        err = label_find(current);
    }

    if (err == ASM_NO_MARKS)
    {
        err = assembling(current);
    }
    else if (err == ASM_OK)
    {
        err = label_parse(current);
        if (err == ASM_OK)
        {
            err = assembling(current);
        }
    }

    stat_destruct(&current);

    return err;
}

void print_error (const assembl_er error)
{
    switch (error)
    {
        case ASM_OK:

            break;

        case ASM_NO_MARKS:

            break;

        case ASM_WRONG_NUM:

            printf("Undefined value\n");
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

#undef DEFINE_COMMANDS
