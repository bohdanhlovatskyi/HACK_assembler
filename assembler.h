#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

typedef struct mapentry {
    char *label;
    int address;
} MapEntry;


MapEntry *create_entry(char *label, int address);

// void iter_over_lines(const char *path, void (*f)(*char))
void iter_over_lines(const char *path, int here_should_go_func);
int get_line(char *buf, size_t bufsize, FILE *fptr);
