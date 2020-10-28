#define READ_MODE         \
    mode = *program_copy; \
    program_copy++;       \
    rip++;

#define READ_VALUE(value);                           \
    for (int i = 0; i < 8; i++)                      \
    {                                                \
        ((char*)(&value))[i] = *program_copy++;      \
    }                                                \
    rip += 8;

#define JUMP                                         \
    program_copy = program_copy - (rip - jump) - 1;  \
    rip = jump - 1;

DEFINE_COMMANDS ( HLT, 0, 0,
{
    stack_destruct(&proc.funcs);
    stack_destruct(&proc.stack);
    free(program);
    return;
})

DEFINE_COMMANDS ( PUSH, 1, 2,
{
    READ_MODE

    if ((int)mode && (int)mode < 5)
    {
        stack_push(&proc.stack, proc.registers[(int)mode - 1]);
    }
    else if ((int)mode == 6)
    {
        READ_VALUE(val_last);

        stack_pop(&proc.stack, &val_earl);

        proc.ram[(int)val_last] = val_earl;
    }
    else
    {
        READ_VALUE(val_last);

        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( ADD, 2, 0,
{
    stack_pop(&proc.stack, &val_last);
    stack_pop(&proc.stack, &val_earl);

    stack_push(&proc.stack, val_earl + val_last);
})

DEFINE_COMMANDS ( SUB, 3, 0,
{
    stack_pop(&proc.stack, &val_last);
    stack_pop(&proc.stack, &val_earl);

    stack_push(&proc.stack, val_last - val_earl);
})

DEFINE_COMMANDS ( MUL, 4, 0,
{
    stack_pop(&proc.stack, &val_last);
    stack_pop(&proc.stack, &val_earl);

    stack_push(&proc.stack, val_last * val_earl);
})

DEFINE_COMMANDS ( OUT, 5, 0,
{
    stack_pop(&proc.stack, &val_last);
    printf("out %lg\n", val_last);

    system("pause");
})

DEFINE_COMMANDS ( SIN, 6, 0,
{
    stack_pop(&proc.stack, &val_last);

    stack_push(&proc.stack, sin(val_last));
})

DEFINE_COMMANDS ( COS, 7, 0,
{
    stack_pop(&proc.stack, &val_last);

    stack_push(&proc.stack, cos(val_last));
})

DEFINE_COMMANDS ( POP, 8, 1,
{
    READ_MODE

    if ((int)mode && (int)mode < 5)
    {
        stack_pop(&proc.stack, &val_last);
        proc.registers[(int)mode - 1] = val_last;
    }
    else if ((int)mode == 6)
    {
        READ_VALUE(val_last);

        stack_push(&proc.stack, proc.ram[(int)val_last]);
    }
    else
    {
        stack_pop(&proc.stack, &val_last);
        printf("%lg\n", val_last);
    }
})

DEFINE_COMMANDS ( SQRT, 9, 0,
{
    stack_pop(&proc.stack, &val_last);

    stack_push(&proc.stack, sqrt(val_last));
})

DEFINE_COMMANDS ( IN, 10, 0,
{
    {
        printf("Write your value: ");
        int prob = scanf("%lg", &val_last);

        if (prob != 1)
        {
            return;
        }

        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( DUMP, 11, 0,
{
    stack_dump(proc.stack, STACK_OK, "PROCESSOR");
})

DEFINE_COMMANDS ( JMP, 12, 1,
{
    READ_VALUE(jump);

    program_copy = program_copy - (rip - jump) - 1;
    rip = jump - 1;
})

DEFINE_COMMANDS ( JAE, 13, 1,
{
    READ_VALUE(jump);

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last >= val_earl)
            JUMP

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JA, 14, 1,
{
    READ_VALUE(jump);

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last > val_earl)
            JUMP

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JB, 15, 1,
{
    READ_VALUE(jump);

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last < val_earl)
            JUMP

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JBE, 16, 1,
{
    READ_VALUE(jump);

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last <= val_earl)
            JUMP

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JE, 17, 1,
{
    READ_VALUE(jump);

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last == val_earl)
            JUMP

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JNE, 18, 1,
{
    READ_VALUE(jump);

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last != val_earl)
            JUMP

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JM, 19, 1,
{
    {
        READ_VALUE(jump);

        struct tm *local;
        time_t timer = time(NULL);

        local = localtime(&timer);

        if (local->tm_wday == 1)
            JUMP
    }
})

DEFINE_COMMANDS ( CALL, 20, 1,
{
    READ_VALUE(jump);

    stack_push(&proc.funcs, rip);

    JUMP(1);
})

DEFINE_COMMANDS ( REV, 21, 0,
{
    double jmp = 0;
    stack_pop(&proc.funcs, &jmp);

    program_copy = program_copy - (rip - (long long)jmp);
    rip = (long long)jmp;
})

