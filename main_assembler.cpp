#include "Assembler.c"

int main (int argc, const char **argv)
{
    assembl_er error = ASM_OK;

    if (argc > 1)
    {
        error = processing(argv[1]);
    }
    else
    {
        error = processing(ASSEMBLING_FILE_NAME);
    }

    switch (error)    //Надо вынести в функцию
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

    return 0;
}
