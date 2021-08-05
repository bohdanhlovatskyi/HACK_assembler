#include "assembler.h"


int convert_to_bin(int number);

int main(void) {
    // char *test_arr = "001";
    // FILE *test_file;

    // test_file = fopen("Test.bin", "wb");
    // fprintf(test_file, "%s\n", test_arr);

    int test[15];

    printf("%d\n", convert_to_bin(24));
}

int convert_to_bin(int number){
    int binary = 0, counter = 0;
    while(number > 0){
        int remainder = number % 2;
        number /= 2;
        binary += pow(10, counter) * remainder;
        counter++;
    }   

    return binary;
}