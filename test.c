#include "assembler.h"


int main(void) {
    char *test_arr = "001";
    FILE *test_file;

    test_file = fopen("Test.bin", "wb");
    fprintf(test_file, "%s\n", test_arr);
}