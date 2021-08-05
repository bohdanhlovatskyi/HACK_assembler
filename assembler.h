#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_
#include <stdlib.h>
#include <stdio.h>

typedef struct mapentry {
    char *label;
    int address;
} MapEntry;

// Functions to work with the table directly
void preprocess_table(void);
MapEntry *create_entry(char *label, int address);
void printTable(MapEntry **table, size_t table_size);

// processes the file, basing on the processment verbose,
// which specifies how each line should be processed
char *iter_over_lines(const char *path, int pass);

// Main function that proccess the file's source code
int get_line(char *buf, size_t bufsize, FILE *fptr);
int process_line(char *buffer, char *line, int *iter);
int iterate_over(char *buffer, int symbols_written, char *line, int handle_vars);
void process_label(char *buf, int *symbol, int *iter);

// helper functions (working with the lookup table)
int find_address(char *target);
void get_available_address(void);
int in_arr(int *arr, int arr_size, int target);

// file names creation (automatisation of saving data in files
// and then deleting them)
char *get_path_to_write(const char *path);
char *get_result_path(char *path);

#endif // ASSEMBLER_H_