DEFINE_COMMANDS ( HLT, 0, 0,
{
    stack_destruct(&proc.funcs);
    stack_destruct(&proc.stack);
    free(program);
    return;
})

DEFINE_COMMANDS ( PUSH, 1, 2,
{
    mode = *program_copy;
    program_copy++;
    rip++;

    if ((int)mode)
    {
        stack_push(&proc.stack, proc.registers[(int)mode - 1]);
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            ((char*)(&val_last))[i] = *program_copy++;
        }

        rip += 8;

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
    mode = *program_copy;
    program_copy++;
    rip++;

    if ((int)mode)
    {
        stack_pop(&proc.stack, &val_last);
        proc.registers[(int)mode - 1] = val_last;
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
    for (int i = 0; i < 8; i++)
    {
        ((char*)(&jump))[i] = *program_copy++;
    }
    rip += 8;

    program_copy = program_copy - (rip - jump) - 1;
    rip = jump - 1;
})

DEFINE_COMMANDS ( JAE, 13, 1,
{
    for (int i = 0; i < 8; i++)
    {
        ((char*)(&jump))[i] = *program_copy++;
    }
    rip += 8;

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last >= val_earl)
        {
            program_copy = program_copy - (rip - jump) - 1;
            rip = jump - 1;
        }

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JA, 14, 1,
{
    for (int i = 0; i < 8; i++)
    {
        ((char*)(&jump))[i] = *program_copy++;
    }
    rip += 8;

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last > val_earl)
        {
            program_copy = program_copy - (rip - jump) - 1;
            rip = jump - 1;
        }

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JB, 15, 1,
{
    for (int i = 0; i < 8; i++)
    {
        ((char*)(&jump))[i] = *program_copy++;
    }
    rip += 8;

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last < val_earl)
        {
            program_copy = program_copy - (rip - jump) - 1;
            rip = jump - 1;
        }

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JBE, 16, 1,
{
    for (int i = 0; i < 8; i++)
    {
        ((char*)(&jump))[i] = *program_copy++;
    }
    rip += 8;

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last <= val_earl)
        {
            program_copy = program_copy - (rip - jump) - 1;
            rip = jump - 1;
        }

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JE, 17, 1,
{
    for (int i = 0; i < 8; i++)
    {
        ((char*)(&jump))[i] = *program_copy++;
    }
    rip += 8;

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last == val_earl)
        {
            program_copy = program_copy - (rip - jump) - 1;
            rip = jump - 1;
        }

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( JNE, 18, 1,
{
    for (int i = 0; i < 8; i++)
    {
        ((char*)(&jump))[i] = *program_copy++;
    }
    rip += 8;

    if (proc.stack->stack->length > 1)
    {
        stack_pop(&proc.stack, &val_last);
        stack_pop(&proc.stack, &val_earl);

        if (val_last != val_earl)
        {
            program_copy = program_copy - (rip - jump) - 1;
            rip = jump - 1;
        }

        stack_push(&proc.stack, val_earl);
        stack_push(&proc.stack, val_last);
    }
})

DEFINE_COMMANDS ( CALL, 19, 1,
{
    for (int i = 0; i < 8; i++)
    {
        ((char*)(&jump))[i] = *program_copy++;
    }
    rip += 8;

    stack_push(&proc.funcs, rip);

    program_copy = program_copy - (rip - jump) - 1;
    rip = jump - 1;
})

DEFINE_COMMANDS ( REV, 20, 0,
{
    double jmp = 0;
    stack_pop(&proc.funcs, &jmp);

    program_copy = program_copy - (rip - (long long)jmp) - 1;
    rip = (long long)jmp - 1;
})
