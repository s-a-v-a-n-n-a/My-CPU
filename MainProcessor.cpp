#include "Processor.c"

int main (int argc, const char **argv)
{
    size_t length = 0;
    char  *prog   = NULL;

    if (argc > 1)
    {
        prog = read_program(&length, argv[1]);
    }
    else
    {
        prog = read_program(&length, EXECUTABLE_FILE);
    }

    if (prog)
        start_processing(prog, length);

    system("pause");
    return 0;
}
