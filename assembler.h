#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>

typedef struct mapentry {
    char *label;
    int address;
} MapEntry;



MapEntry *create_entry(char *label, int address);

// void iter_over_lines(const char *path, void (*f)(*char))
void iter_over_lines(const char *path, int here_should_go_func);
int get_line(char *buf, size_t bufsize, FILE *fptr);

FILE *get_file_to_write(const char *path);
void printTable(MapEntry **table, size_t table_size);
int process_line(char *buffer, char *line, int *iter);


void process_label(char *buf, int *symbol, int *iter);
int find_address(char *target);
void preprocess_table(void);
void get_available_address(void);
int in_arr(int *arr, int arr_size, int target);

int iterate_over(char *buffer, int symbols_written, char *line, int handle_vars);