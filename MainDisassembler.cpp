#include "Disassembler.c"

int main ()
{
    size_t length = 0;
    char  *codes  = read_codes(&length);

    FILE *dis = fopen(DISASSEMBLING_FILE, "wb");

    if (codes)
    {
       long long *labels = NULL;

       long long num_labels = count_labels(dis, codes, length, &labels);
       write_labels(dis, codes, length, labels);
       disassembling(dis, codes, length, labels, num_labels);

       free(labels);
    }

    free(codes);
    fclose(dis);

    return 0;
}
