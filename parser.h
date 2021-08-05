#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

void parse_line(char *buffer);
const char * decode_comp(const char *const input);
const char * decode_dest(const char *const input);
const char * decode_jump(const char *const input);
int convert_to_bin(int number);
int int_len(int integer);

void proccess_A_cmd(char *buffer);