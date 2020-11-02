/**
    \file
    File with functions for using my stack
    It has constructor, destructor, push, pop, back and resize.
    There is also defend from wrong using of it.
    Thank you for using this program!
    \warning This stack works only with doubles \n
    \authors Anna Savchuk
    \note    If any function gets errors not like STACK_OK, they send it to stack_dump \n
             At the end the last information about stack will be added to log_file
    \date    Last update was 10.24.20 at 22:40
    \todo    add codecopy in Stack_Struct -> change verifier
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum stack_code_errors { STACK_OK,
                                 STACK_NULL,
                                 STACK_SEG_FAULT,
                                 STACK_DELETED,
                                 STACK_NO_CONSTRUCT,
                                 STACK_UNDERFLOW,
                                 STACK_NO_MEMORY,
                                 STACK_TOO_BIG,
                                 STACK_DEAD_CANARY,
                                 STACK_INVADERS,
                                 STACK_TRANSACTION_ERROR,
                                 STACK_TRANSACTION_OK    } stack_code;


const char *STACK_CONSTRUCT     = "CONSTRUCTOR";
const char *STACK_DESTRUCT      = "DESTRUCTOR";
const char *STACK_PUSH          = "PUSHING";
const char *STACK_RESIZE        = "RESIZING";
const char *STACK_POP           = "POPPING";
const char *STACK_BACK          = "LOOKING BACK";

const double EPSILON = 0.000001;

typedef double stack_elem;
#define elem "%lg"

#define ASSERTION(code)                                                   \
    fprintf(stderr, "-----------------!WARNING!----------------\n");      \
    fprintf(stderr, "IN FILE %s\nIN LINE %d\n", __FILE__, __LINE__);      \
    assertion(code);                                                      \


#define VERIFYING(that_stack, mode);                                      \
    stack_code check = stack_verifier(that_stack);                        \
    if (check != STACK_OK)                                                \
    {                                                                     \
        ASSERTION(check);                                                 \
        stack_dump((*that_stack), check, mode);                           \
    }


#define VERIFYING_DESTRUCT(that_stack);                                   \
    stack_code check = stack_verifier(that_stack);                        \
    if (check != STACK_OK)                                                \
    {                                                                     \
        ASSERTION(check);                                                 \
        stack_dump((*that_stack), check, STACK_DESTRUCT);                 \
    }                                                                     \
    else                                                                  \
        stack_dump(*that_stack, STACK_OK, STACK_DESTRUCT);                                                                     \

typedef struct Stack_struct Structure;
typedef struct Defeat_stack Stack;

struct Stack_struct
{
    int         canary_before;
    stack_elem *buffer;
    size_t      length;
    size_t      capacity;
    long int    hash_buffer;
    long int    hash_stack;
};

struct Defeat_stack
{
    int           canary_first;
    Structure    *stack;
    Structure    *stack_copy;
    int           canary_last;
};

//#define COPYING

#ifdef COPYING
static Stack *cage_copy;
#endif

/*!
Prints the errors in the console
@param[in]        code                The identifier of the error
*/
void              assertion             (stack_code code);

/*!
Outputs the information about the current state of the stack into the file
@param[in]       *log                 The file to write in
@param[in]       *that_stack          The pointer on the shell of the stack
*/
void              print_state_stack     (FILE *log, Stack *that_stack);

/*!
Outputs the information about the current state of the stack into "log_file.txt"
@param[in]       *that_stack          The pointer on the shell of the stack
@param[in]        code                The code of the mistake
@param[in]        who                 The code of the function requested for dump
*/
void              stack_dump            (Stack *that_stack, stack_code code, const char *who);

/*!
Checks if the pointer is valid
@param[in]       *that_stack          The pointer on the shell of the stack
Returns  STACK_NULL                   If the pointer does not exists
         STACK_SEG_FAULT              If the pointer points in prohibited for using block of memory
         STACK_OK                     If the pointer is valid
*/
stack_code        is_pointer_valid      (Stack *that_stack);

/*!
Counts hash for the buffer in the stack by algorythm Adler-32
@param[in]       *that_stack          The pointer on the shell of the stack
@param[out]       hash_sum            The hash of the buffer in the stack
*/
static long int   hashing_buffer        (Structure *that_stack);

/*!
Counts hash for the whole stack by algorythm Adler-32
@param[in]       *that_stack          The pointer on the shell of the stack
@param[out]       hash_sum            The hash of the whole stack
*/
static long int   hashing_stack         (Structure *that_stack);

/*!
Checks if the stack was spoiled and returns it to last saved correct version if it was
@param[in]       *that_stack          The pointer on the shell of the stack
Returns  STACK_OK                     If everything is okay
*/
static stack_code reserve_copy          (Structure **that_stack, Structure **copy_stack);

/*!
Frees the spoiled stack and creates new due to the copy
@param[in]      **stack_1             The pointer on pointer on the shell of the spoiled stack
@param[in]       *stack_2             The pointer on the shell of the copy of stack
Returns  STACK_TRANSACTION_OK         If the stack was transacted\n
         STACK_TRANSACTION_ERROR      If it was impossible to transact\n
*/
static stack_code transaction           (Structure **stack_1, Structure *stack_2);

/*!
Checks all of the states of the stack
@param[in]      *that_stack           The pointer on the shell of the stack
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code        stack_verifier        (Stack **that_stack);

/*!
Creates new stack
@param[in]        size                The size of the future buffer of the stack
@param[out]      *stack_tmp           The pointer on the shell of the future stack
*/
Stack            *stack_new             (size_t size);

/*!
Constructs the stack
@param[in]      **that_stack          The stack
Returns  STACK_OK                     If everything is ok
         STACK_NO_CONSTRUCT           If it was impossible to create the stack
*/
static stack_code stack_construct       (Structure **that_stack, size_t stack_size);

/*!
Destructs the shell of the stack
@param[in]      **that_stack          The pointer on pointer on the shell of the stack
Returns  STACK_OK                     If everything is ok\n
*/
stack_code        stack_destruct        (Stack **that_stack);

/*!
Destructs the stack
@param[in]      **that_stack          The stack
Returns  STACK_OK                     If everything is ok\n
*/
stack_code        stack_destruct_inside (Structure **that_stack);

/*!
Changes the capacity of the stack
@param[in]      **that_stack          The pointer on the shell of the stack
@param[in]       *stack               The stack from the shell to resize
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_NO_MEMORY              If there is no memory to resize\n
         STACK_TOO_BIG                If the needed memory is too big\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code        stack_resize          (Stack **that_stack, Structure *stack, const double amount);

/*!
Adds value to the end of the stack
@param[in]      **that_stack          The pointer on pointer on the shell of the stack
@param[in]        value               The value wanted to be pushed
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code        stack_push            (Stack **that_stack, stack_elem value);

/*!
Delets value from the end of the stack
@param[in]      **that_stack          The pointer on pointer on the shell of the stack
@param[in]       *value               The pointer on the value wanted to be pushed
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code        stack_pop             (Stack **that_stack, stack_elem *value);

/*!
Shows value from the end of the stack
@param[in]      **that_stack          The pointer on pointer on the shell of the stack
@param[in]       *value               The pointer on the value wanted to be pushed
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code        stack_back            (Stack **that_stack, stack_elem *value);


void assertion (stack_code code)
{
    switch (code)
    {
        case STACK_OK:
            break;
        case STACK_NULL:
            printf("Error: NO POINTER ON STACK FOUND\n\n");
            break;
        case STACK_SEG_FAULT:
            printf("Error: USAGE OF PROHIBITED PART OF MEMORY\n\n");
            break;
        case STACK_DELETED:
            printf("Error: DEALING WITH NON-EXISTENT UNIT OR THE UNIT WAS DAMAGED\n\n");
            break;
        case STACK_NO_CONSTRUCT:
            printf("Error: NO MEMORY FOR CONSTRUCTION\n\n");
            break;
        case STACK_UNDERFLOW:
            printf("Error: GOING OUT OF BONDS OF STACK\n\n");
            break;
        case STACK_NO_MEMORY:
            printf("Error: NO FREE MEMORY\n\n");
            break;
        case STACK_TOO_BIG:
            printf("Error: TOO BIG CAPACITY REQUIRED\n\n");
            break;
        case STACK_DEAD_CANARY:
            printf("Error: THE UNIT WAS DAMAGED\n\n");
            break;
        case STACK_INVADERS:
            printf("Error: OUTSIDE INVASION INSIDE OF STACK\n\n");
            break;
        case STACK_TRANSACTION_ERROR:
            printf("Error: ERROR OF MAKING A TRANSACTION\n\n");
            break;
        case STACK_TRANSACTION_OK:
            printf("A TRANSACTION WAS MADE\n\n");
            break;
        default:
            break;
    }
}

void print_state_stack(FILE *log, Stack *that_stack)
{
    fprintf(log, "Current capacity: %u\n", that_stack->stack->capacity);
    fprintf(log, "Current size    : %u\n", that_stack->stack->length - 1);
    fprintf(log, "Current address of the stack : %p\n", that_stack->stack);
    fprintf(log, "Current address of the buffer: %p\n", that_stack->stack->buffer);
    for (size_t i = 0; i <= that_stack->stack->length; i++)
    {
        fprintf(log, "[%4u] : " elem "\n", i, that_stack->stack->buffer[i]);
    }
    for (size_t i = that_stack->stack->length + 1; i < that_stack->stack->capacity; i++)
    {
        fprintf(log, "[%4u] : NAN (POISON)\n", i);
    }
    fprintf(log, "Current hash of the buffer: %ld\n", that_stack->stack->hash_buffer);
    fprintf(log, "Current hash of the stack : %ld\n", that_stack->stack->hash_stack);
}

void stack_dump (Stack *that_stack, stack_code code, const char *who)
{
    static long int doing = 0;

    const char *mode =  "";
    if (!doing)
        mode = "wb";
    else
        mode = "ab";
    FILE *log = fopen("log_file.txt", mode);

    fprintf(log, "CURRENT STATE OF STACK\n");
    fprintf(log, "THE NEWS FROM %s\n", who);

    switch (code)
    {
        case STACK_OK:

            fprintf(log, "EVERYTHING IS OKAY\n");
            print_state_stack(log, that_stack);
            break;

        case STACK_NULL:

            fprintf(log, "STACK DOES NOT EXIST\n");
            break;

        case STACK_SEG_FAULT:

            fprintf(log, "MEMORY ACCESS DENIED\n");
            break;

        case STACK_DELETED:

            fprintf(log, "DEALING WITH NON-EXISTENT UNIT OR THE UNIT WAS DAMAGED\n");
            fprintf(log, "FIRST CANARY : %0xd\n", that_stack->canary_first);
            fprintf(log, "LAST  CANARY : %0xd\n", that_stack->canary_last);
            break;

        case STACK_NO_CONSTRUCT:

            fprintf(log, "NO MEMORY FOR CONSTRUCTION\n");
            break;

        case STACK_UNDERFLOW:

            fprintf(log, "NOTHING TO POP\n");
            print_state_stack(log, that_stack);
            break;

        case STACK_NO_MEMORY:

            fprintf(log, "MEMORY ACCESS DENIED\n");
            print_state_stack(log, that_stack);
            break;

        case STACK_TOO_BIG:

            fprintf(log, "TOO BIG CAPACITY IS REQUIRED\n");
            print_state_stack(log, that_stack);
            break;

        case STACK_DEAD_CANARY:

            fprintf(log, "THE UNIT WAS DAMAGED\n");
            fprintf(log, "FIRST CANARY : %0xd\n", that_stack->canary_first);
            fprintf(log, "LAST  CANARY : %0xd\n", that_stack->canary_last);
            print_state_stack(log, that_stack);
            break;

        case STACK_INVADERS:

            fprintf(log, "THE INVASION WAS DETECTED\n");
            fprintf(log, "FIRST CANARY : %0xd\n", that_stack->canary_first);
            fprintf(log, "LAST  CANARY : %0xd\n", that_stack->canary_last);
            print_state_stack(log, that_stack);
            break;

        case STACK_TRANSACTION_ERROR:

            fprintf(log, "ERROR OF MAKING A TRANSACTION\n");
            break;

        case STACK_TRANSACTION_OK:

            fprintf(log, "THE TRANSACTION WAS MADE\n");
            fprintf(log, "FIRST CANARY : %0xd\n", that_stack->canary_first);
            fprintf(log, "LAST  CANARY : %0xd\n", that_stack->canary_last);
            print_state_stack(log, that_stack);
            break;

        default:
            break;
    }

    fprintf(log, "\n");

    fclose(log);

    doing++;
}

stack_code is_pointer_valid (Stack *that_stack)
{
    if (!that_stack || !that_stack->stack || !that_stack->stack->buffer || !that_stack->stack_copy || !that_stack->stack_copy->buffer)
        return STACK_NULL;

    if ((long long)(that_stack->stack) <= 4096 || (long long)(that_stack->stack->buffer) <= 4096 || (long long)(that_stack->stack_copy) <= 4096 || (long long)(that_stack->stack_copy->buffer) <= 4096)
        return STACK_SEG_FAULT;

    if (*(unsigned int *)that_stack != 0x5E7CA6E || *(unsigned int *)that_stack->stack != 0xDEADB14D || !isnan((float)that_stack->stack->buffer[0]) ||
                                                    *(unsigned int *)that_stack->stack_copy != 0xDEADB14D || !isnan((float)that_stack->stack_copy->buffer[0]))
    {
        return STACK_DELETED;
    }

    return STACK_OK;
}

static long int hashing_buffer (Structure *that_stack)
{
    long int hash_sum = 0;

    long int sum        = 1;
    long int prefix_sum = 0;

    for (char* i = (char*)(that_stack->buffer); i < (char*)that_stack->buffer +
                   (that_stack->length) * sizeof(that_stack->buffer[0]); i++)
    {
        sum = (sum + *i) % 65521;
        prefix_sum = (prefix_sum + sum) % 65521;
    }

    hash_sum = (prefix_sum << 16) + sum;

    return hash_sum;
}

static long int hashing_stack (Structure *that_stack)
{
    long int hash_tmp      = that_stack->hash_stack;
    that_stack->hash_stack = 0;

    long int sum         = 0;
    long int coefficient = 1;

    for (char* i = (char*)&(that_stack->canary_before); i < (char*)&(that_stack->hash_stack); i++)
    {
        sum = (sum + *i * coefficient) % 65521;
        coefficient++;
    }

    that_stack->hash_stack = hash_tmp;
    return sum;
}

static stack_code reserve_copy (Structure **that_stack, Structure **copy_stack)
{
    (*that_stack)->length      = (*copy_stack)->length;
    (*that_stack)->capacity    = (*copy_stack)->capacity;

    for (size_t i = 0; i <= (*copy_stack)->length; i++)
    {
        (*that_stack)->buffer[i] = (*copy_stack)->buffer[i];
    }

    (*that_stack)->hash_buffer = hashing_buffer(*that_stack);
    (*that_stack)->hash_stack  = hashing_stack (*that_stack);

    return STACK_OK;
}

static stack_code transaction(Structure **stack_1, Structure *stack_2)
{
    free((*stack_1)->buffer);
    free(*stack_1);

    (*stack_1)         = (Structure*) calloc(1, sizeof(Structure));

    (*stack_1)->canary_before = 0xDEADB14D;
    (*stack_1)->capacity      = stack_2->capacity;
    (*stack_1)->buffer        = (stack_elem*) calloc((*stack_1)->capacity, sizeof(stack_elem));
    if (!(*stack_1)->buffer)
    {
        ASSERTION(STACK_TRANSACTION_ERROR);
        stack_dump(NULL, STACK_TRANSACTION_ERROR, STACK_CONSTRUCT);
        return STACK_TRANSACTION_ERROR;
    }

    reserve_copy(stack_1, &stack_2);
    return STACK_TRANSACTION_OK;
}

stack_code stack_verifier (Stack **that_stack)
{
    stack_code indicator = is_pointer_valid(*that_stack);
    if (indicator != STACK_OK)
    {
        return indicator;
    }

    long int flag_eq              = fabs(((*that_stack)->stack->buffer[(*that_stack)->stack->length] -
                                     (*that_stack)->stack_copy->buffer[(*that_stack)->stack_copy->length])) < EPSILON;

    long int hash_tmp_stack       = hashing_stack ((*that_stack)->stack);
    long int hash_tmp_stack_buf   = hashing_buffer((*that_stack)->stack);

    long int hash_tmp_copy        = hashing_stack((*that_stack)->stack_copy);
    long int hash_tmp_copy_buf    = hashing_buffer((*that_stack)->stack_copy);

    long int flag_hash_stack      = (hash_tmp_stack == (*that_stack)->stack->hash_stack);
    long int flag_hash_stack_buf  = (hash_tmp_stack_buf == (*that_stack)->stack->hash_buffer);

    long int flag_hash_copy       = (hash_tmp_copy  == (*that_stack)->stack_copy->hash_stack);
    long int flag_hash_copy_buf   = (hash_tmp_copy_buf  == (*that_stack)->stack_copy->hash_buffer);

    if ((*that_stack)->canary_first == 0x5E7CA6E && (*that_stack)->canary_last == 0x0FFCA6E)
    {
        if (flag_eq && (!isnan((float)(*that_stack)->stack->buffer[(*that_stack)->stack->length]) ||
                        !isnan((float)(*that_stack)->stack_copy->buffer[(*that_stack)->stack_copy->length])))
        {
            return STACK_DELETED;
        }
        else if ((!flag_eq && !isnan((float)(*that_stack)->stack->buffer[(*that_stack)->stack->length])) ||
                 (!flag_hash_stack && flag_hash_copy && !flag_hash_stack_buf && flag_hash_copy_buf))
        {
            printf("Hi, bitch\n");
            stack_code code = transaction(&(*that_stack)->stack, (*that_stack)->stack_copy);
            return code;
        }
        else if ((!flag_eq && !isnan((float)(*that_stack)->stack_copy->buffer[(*that_stack)->stack_copy->length])) ||
                 (!flag_hash_stack && flag_hash_copy && !flag_hash_stack_buf && flag_hash_copy_buf))
        {
            printf("Hello, bitch\n");
            stack_code code = transaction(&(*that_stack)->stack_copy, (*that_stack)->stack);
            return code;
        }
        else if (!(flag_hash_stack && flag_hash_copy && flag_hash_stack_buf) && flag_hash_copy_buf)
        {
            long int i        = 1;
            long long new_len = 0;

            while(!isnan((float)(*that_stack)->stack_copy->buffer[i]))
            {
                new_len++;
                i++;
            }

            (*that_stack)->stack_copy->length     = new_len + 1;
            (*that_stack)->stack_copy->capacity   = new_len + 2;
            (*that_stack)->stack_copy->hash_stack = hash_tmp_copy;

            reserve_copy(&(*that_stack)->stack_copy, &(*that_stack)->stack);
            return STACK_TRANSACTION_OK;
        }
        else if (!(flag_hash_stack && flag_hash_copy && flag_hash_copy_buf) && flag_hash_stack_buf)
        {
            long int  i       = 1;
            long long new_len = 0;

            while(!isnan((float)(*that_stack)->stack->buffer[i]))
            {
                new_len++;
                i++;
            }

            (*that_stack)->stack->length     = new_len + 1;
            (*that_stack)->stack->capacity   = new_len + 2;
            (*that_stack)->stack->hash_stack = hash_tmp_stack;

            reserve_copy(&(*that_stack)->stack, &(*that_stack)->stack_copy);
            return STACK_TRANSACTION_OK;
        }
        else if (!(flag_hash_stack && flag_hash_copy && flag_hash_stack_buf && flag_hash_copy_buf))
        {
            return STACK_INVADERS;
        }
    }
    else
    {
        return STACK_DEAD_CANARY;
    }

    return STACK_OK;
}

Stack *stack_new(size_t size)
{
    Stack *cage = NULL;

    cage = (Stack*) calloc(1, sizeof(Stack));
    if (cage)
    {
        cage->canary_first  = 0x5E7CA6E;
        cage->canary_last   = 0x0FFCA6E;

        if (stack_construct(&(cage->stack), size) == STACK_NO_CONSTRUCT)
        {
            stack_destruct_inside(&(cage->stack));
            free(cage);
            cage = NULL;
        }

        if (stack_construct(&(cage->stack_copy), size) == STACK_NO_CONSTRUCT)
        {
            stack_destruct_inside(&(cage->stack_copy));
            free(cage);
            cage = NULL;
        }
    }
    return cage;
}

static stack_code stack_construct(Structure **that_stack, size_t stack_size)
{
    (*that_stack) = (Structure*) calloc(1, sizeof(Structure));
    if (!(*that_stack))
    {
        ASSERTION(STACK_NO_CONSTRUCT);
        stack_dump(NULL, STACK_NO_CONSTRUCT, STACK_CONSTRUCT);
        return STACK_NO_CONSTRUCT;
    }

    (*that_stack)->canary_before = 0xDEADB14D;
    (*that_stack)->capacity      = stack_size + 2;
    (*that_stack)->buffer        = (stack_elem*) calloc(stack_size + 2, sizeof(stack_elem));
    if (!(*that_stack)->buffer)
    {
        ASSERTION(STACK_NO_CONSTRUCT);
        stack_dump(NULL, STACK_NO_CONSTRUCT, STACK_CONSTRUCT);
        return STACK_NO_CONSTRUCT;
    }
    (*that_stack)->buffer[0]   = NAN;
    (*that_stack)->buffer[1]   = NAN;
    (*that_stack)->length      = 1;
    (*that_stack)->hash_buffer = hashing_buffer(*that_stack);
    (*that_stack)->hash_stack  = hashing_stack (*that_stack);

    return STACK_OK;
}

stack_code stack_destruct(Stack **that_stack)
{
    VERIFYING_DESTRUCT(that_stack);

    stack_destruct_inside(&(*that_stack)->stack);
    stack_destruct_inside(&(*that_stack)->stack_copy);

    if (*that_stack)
        free(*that_stack);

    return STACK_OK;
}

stack_code stack_destruct_inside(Structure **that_stack)
{
    if (*that_stack)
    {
        if ((*that_stack)->buffer)
        {
            free((*that_stack)->buffer);
        }
        free(*that_stack);
    }

    return STACK_OK;
}

stack_code stack_resize(Stack **that_stack, Structure *stack, const double amount)
{
    if (stack == (*that_stack)->stack)
    {
        VERIFYING(that_stack, STACK_RESIZE);
    }

    if (stack->capacity > ((size_t)-1)/2)
    {
        ASSERTION(STACK_TOO_BIG);
        stack_dump((*that_stack), STACK_TOO_BIG, STACK_RESIZE);
    }

    size_t new_capacity = (size_t)(stack->capacity * amount) + 2;

    stack_elem *ptr = (stack_elem*)realloc(stack->buffer, sizeof(stack_elem) * new_capacity);

    if (!ptr)
    {
        ASSERTION(STACK_NO_MEMORY);
        stack_dump((*that_stack), STACK_NO_MEMORY, STACK_RESIZE);
    }

    stack->capacity = new_capacity;
    stack->buffer   = ptr;

    return STACK_OK;
}

stack_code stack_push(Stack **that_stack, const stack_elem value)
{
    VERIFYING(that_stack, STACK_PUSH);

    if ((*that_stack)->stack->length + 1 >= (*that_stack)->stack->capacity)
    {
        stack_resize(that_stack, (*that_stack)->stack,      2);
        stack_resize(that_stack, (*that_stack)->stack_copy, 2);
    }

    (*that_stack)->stack->buffer[(*that_stack)->stack->length++] = value;
    (*that_stack)->stack->buffer[(*that_stack)->stack->length]   = NAN;

    (*that_stack)->stack->hash_buffer = hashing_buffer((*that_stack)->stack);
    (*that_stack)->stack->hash_stack  = hashing_stack ((*that_stack)->stack);


    (*that_stack)->stack_copy->buffer[(*that_stack)->stack_copy->length++] = value;
    (*that_stack)->stack_copy->buffer[(*that_stack)->stack_copy->length]   = NAN;

    (*that_stack)->stack_copy->hash_buffer = hashing_buffer((*that_stack)->stack_copy);
    (*that_stack)->stack_copy->hash_stack  = hashing_stack ((*that_stack)->stack_copy);


    return STACK_OK;
}

stack_code stack_pop(Stack **that_stack, stack_elem *value)
{
    VERIFYING(that_stack, STACK_POP);

    if ((long long int)((*that_stack)->stack->length) - 1 <= 0)
    {
        ASSERTION(STACK_UNDERFLOW);
        stack_dump((*that_stack), STACK_UNDERFLOW, STACK_POP);
        return STACK_UNDERFLOW;
    }

    if ((*that_stack)->stack->length <= (*that_stack)->stack->capacity/2)
    {
        stack_resize(that_stack, (*that_stack)->stack,      0.5);
        stack_resize(that_stack, (*that_stack)->stack_copy, 0.5);
    }

    *value = (*that_stack)->stack->buffer[--(*that_stack)->stack->length];
    (*that_stack)->stack->buffer[(*that_stack)->stack->length] = NAN;

    (*that_stack)->stack->hash_buffer = hashing_buffer((*that_stack)->stack);
    (*that_stack)->stack->hash_stack  = hashing_stack((*that_stack)->stack);


    (*that_stack)->stack_copy->buffer[--(*that_stack)->stack_copy->length] = NAN;

    (*that_stack)->stack_copy->hash_buffer = hashing_buffer((*that_stack)->stack_copy);
    (*that_stack)->stack_copy->hash_stack  = hashing_stack((*that_stack)->stack_copy);

    return STACK_OK;
}

stack_code stack_back(Stack **that_stack, stack_elem *value)
{
    VERIFYING(that_stack, STACK_BACK);

    if ((long long int)((*that_stack)->stack->length) - 1 <= 0)
    {
        ASSERTION(STACK_UNDERFLOW);
        stack_dump((*that_stack), STACK_UNDERFLOW, STACK_BACK);
        return STACK_UNDERFLOW;
    }

    *value = (*that_stack)->stack->buffer[(*that_stack)->stack->length - 1];

    return STACK_OK;
}

