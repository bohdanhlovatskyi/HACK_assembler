#ifndef PARSER_H_
#define PARSER_H_
#include <stdio.h>

void parse_line(char *buffer, FILE *outfile);
const char * decode_comp(const char *const input);
const char * decode_dest(const char *const input);
const char * decode_jump(const char *const input);
int convert_to_bin(int number);
int int_len(int integer);

void proccess_A_cmd(char *buffer, FILE *outfile);

#endif // PARSER_H_

