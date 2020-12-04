#include "Assembler.c"

int main (int argc, const char **argv)
{
    assembl_er error = ASM_OK;

    if (argc > 1)
    {
        error = assembling_control(argv[1]);
    }
    else
    {
        error = assembling_control(ASSEMBLING_FILE_NAME);
    }

    print_error(error);

    return 0;
}
