#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

// comp
#define ZERO "101010"
#define ONE  "111111"
#define MINUS_ONE "111010"
#define D_COMP "001100"
#define A_COMP "110000"
#define NOT_D "001101"
#define NOT_A "110001"
#define MINUS_D "001111"
#define MINUS_A "110011"
#define D_PLUS_ONE "011111"
#define A_PLUS_ONE "110111"
#define D_MINUS_ONE "001110"
#define A_MINUS_ONE "110010"
#define D_PLUS_A "000010"
#define D_MINUS_A "010011"
#define A_MINUS_D "000111"
#define D_AND_A "000000"
#define D_OR_A "010101"


// dest
#define null "000"
#define M "001"
#define D "010"
#define MD "011"
#define A "100"
#define AM "101"
#define AD "110"
#define AMD "111"

// jump
#define null "000"
#define JGT "001"
#define JEQ "010"
#define JGE "011"
#define JLT "100"
#define JNE "101"
#define JLE "110"
#define JMP "111"

#define MAXSIZE 64
#define DEBUG 0
#define NUM(a) sizeof(a) / sizeof(a[0])

/*
int main(void) {
    char *test_lines[] = {
        "@0",
        "D=M",
        "@24",
        "D;JLE",
        "@17",
        "M=D",
        "@16",
        "D=A"
    };

    for (int i = 0; i < 8; i++) {
        parse_line(test_lines[i]);
    }

}*/

void parse_line(char *buffer, FILE *res) {

    char out[MAXSIZE] = {0};
    int out_iter = 0;
    char dest[MAXSIZE] = {0};
    char comp[MAXSIZE] = {0};
    char jump[MAXSIZE] = {0};
    int jump_iter = 0;
    char ch = 0; // character that will be used to iterate over the string
    int current_char = 0; // int to iterate over the buffer

    if (buffer[0] == '@') {
        proccess_A_cmd(buffer, res);
    } else {
        char temp_buf[MAXSIZE] = {0};
        int temp_buf_iter;
        while ((ch = buffer[current_char++]) != '\0') {
            switch (ch)
            {
            case '=':
                temp_buf[temp_buf_iter] = '\0';
                strcpy(dest, temp_buf);
                memset(temp_buf, 0, temp_buf_iter);
                temp_buf_iter = 0;
                break;
            case ';':
                temp_buf[temp_buf_iter] = '\0';
                strcat(comp, temp_buf);
                memset(temp_buf, 0, temp_buf_iter);
                temp_buf_iter = 0;
                while ((ch = buffer[current_char++]) != '\0') {
                    jump[jump_iter++] = ch;
                }
                goto END;
                break;
            default:
                temp_buf[temp_buf_iter++] = ch;
                break;
            }
        }
        if (temp_buf_iter != 0) {
            strcat(comp, temp_buf);
            temp_buf_iter = 0;
        }
        END:;

        if (DEBUG) {
            printf("%s | %s | %s\n", dest, comp, jump);
            printf("%s | %s | %s\n", decode_dest(dest), decode_comp(comp), decode_jump(jump));
        }
        fprintf(res, "111%s%s%s\n", decode_dest(dest), decode_comp(comp), decode_jump(jump));
    }
}


const char * decode_comp(const char *const input)
{
    if (DEBUG)
        printf("comp input: %s\n", input);
    if (strcmp(input, "0") == 0) return ZERO;
    if (strcmp(input, "1") == 0) return ONE;
    if (strcmp(input, "-1") == 0) return MINUS_ONE;
    if (strcmp(input, "D") == 0) return D_COMP;
    if (strcmp(input, "A") == 0) return A_COMP;
    if (strcmp(input, "M") == 0) return A_COMP; // a=1
    if (strcmp(input, "!D") == 0) return NOT_D;
    if (strcmp(input, "!A") == 0) return NOT_A;
    if (strcmp(input, "!M") == 0) return NOT_A; // a=1
    if (strcmp(input, "-D") == 0) return MINUS_D;
    if (strcmp(input, "-A") == 0) return MINUS_A;
    if (strcmp(input, "D+A") == 0) return D_PLUS_A;
    if (strcmp(input, "D+M") == 0) return D_PLUS_A; // a=1
    if (strcmp(input, "A+D") == 0) return D_PLUS_A;
    if (strcmp(input, "D-A") == 0) return D_MINUS_A;
    if (strcmp(input, "D-M") == 0) return D_MINUS_A; // a=1
    if (strcmp(input, "A-D") == 0) return A_MINUS_D;
    if (strcmp(input, "M-D") == 0) return A_MINUS_D; // a=1
    if (strcmp(input, "D+1") == 0) return D_PLUS_ONE;
    if (strcmp(input, "A+1") == 0) return A_PLUS_ONE;
    if (strcmp(input, "M+1") == 0) return A_PLUS_ONE; // a=1
    if (strcmp(input, "D-1") == 0) return D_MINUS_ONE;
    if (strcmp(input, "A-1") == 0) return A_MINUS_ONE;
    if (strcmp(input, "M-1") == 0) return A_MINUS_ONE; // a=1
    if (strcmp(input, "D&A") == 0) return D_AND_A;
    if (strcmp(input, "D&M") == 0) return D_AND_A; // a=1
    if (strcmp(input, "A&D") == 0) return D_AND_A;
    if (strcmp(input, "D|A") == 0) return D_OR_A;
    if (strcmp(input, "D|M") == 0) return D_OR_A; // a=1
    if (strcmp(input, "D|A") == 0) return D_OR_A;
    if (strcmp(input, "M|D") == 0) return D_OR_A; // a=1

    return NULL;
}

const char * decode_dest(const char *const input)
{
    if (DEBUG)
        printf("dest input: %s\n", input);
    if (strcmp(input, "") == 0) return null;
    if (strcmp(input, "M") == 0) return M;
    if (strcmp(input, "D") == 0) return D;
    if (strcmp(input, "MD") == 0) return MD;
    if (strcmp(input, "A") == 0) return A;
    if (strcmp(input, "AM") == 0) return AM;
    if (strcmp(input, "AD") == 0) return AD;
    if (strcmp(input, "AMD") == 0) return AMD;

    return NULL;
}

const char * decode_jump(const char *const input)
{
    if (DEBUG)
        printf("jumpu input: %s\n", input);
    if (strcmp(input, "") == 0) return null;
    if (strcmp(input, "JGT") == 0) return JGT;
    if (strcmp(input, "JEQ") == 0) return JEQ;
    if (strcmp(input, "JGE") == 0) return JGE;
    if (strcmp(input, "JLT") == 0) return JLT;
    if (strcmp(input, "JNE") == 0) return JNE;
    if (strcmp(input, "JLE") == 0) return JLE;
    if (strcmp(input, "JMP") == 0) return JMP;

    return NULL;
}

int convert_to_bin(int number) {
    int binary = 0, counter = 0;
    while(number > 0){
        int remainder = number % 2;
        number /= 2;
        binary += pow(10, counter) * remainder;
        counter++;
    }   

    return binary;
}

int int_len(int integer){
    int a;
    for(a = 1; integer /= 10; a++);
    return a;
}


void proccess_A_cmd(char *buffer, FILE *outfile) {
    char temp_str_int[MAXSIZE];
    int temp_str_iter = 0;
    char address_str[15];
    char ch;

    // this fills up the buffer with string representation of the integer
    int i = 1;
    while ((ch = buffer[i++]) != '\0')
        temp_str_int[temp_str_iter++] = ch;
    temp_str_int[temp_str_iter] = '\0';

    // type conversion
    int address = atoi(temp_str_int); // address in int
    int bin_repr = convert_to_bin(address); // bin address in int: `11010`
    sprintf(address_str, "%d", bin_repr); // bin address in str

    int len;
    if ((len = 15 - int_len(bin_repr)) > 0) {
        // creates a string of zeroes to add them to the beginning of binary string,
        // so to its size was 15 digits
        char zeroes_to_add[len];
        memset(zeroes_to_add, '0', len - 1);
        zeroes_to_add[len - 1] = '\0';

        // prints the A cmd to the file
        fprintf(outfile, "0%s%s\n", zeroes_to_add, address_str);
    } else {
        fprintf(outfile, "0%s\n", address_str);
    }
    // cleaning up
    memset(temp_str_int, 0, temp_str_iter);
}
