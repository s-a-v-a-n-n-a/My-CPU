#include "Assembler.h"

// extern const char *LISTING_FILE         = "listing.txt";
// extern const char *EXECUTABLE_FILE      = "binary.xex";
// extern const char *ASSEMBLING_FILE_NAME = "~/Repositories/My-CPU";
// extern const char *DISASSEMBLING_FILE   = "disassemble.txt";

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
