#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>

/* Main function, is used to parse an instruction, given
as a strings (can be within a buffer) and writes the mapped
instruction to the given outfile */
void parse_line(char *buffer, FILE *outfile);

/* Function that map the input string to the
binary string that will be outputed. Uses series
of if statements, as it is the easiest implementation,
which allows to handle all the cases without bugs */
const char * decode_comp(const char *const input);
const char * decode_dest(const char *const input);
const char * decode_jump(const char *const input);

/* Helper functions that will be used by proccess_A_cmd,
that allow to convert decimal integer to decimal representation
of its binary form, that can be then written to a string,
as well as determine length of this binary strings (which will
allow one to add zeroes to its beginning, so to match 15-bit
wide A instruction) */
int convert_to_bin(int number);
int int_len(int integer);

/* Helper function for parse_line to process A-incstruction
(Example: @34)*/
void proccess_A_cmd(char *buffer, FILE *outfile);

#endif // PARSER_H_

