#include "assembler.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>


#define NUM(a) sizeof(a) / sizeof(a[0])
#define MAXSIZE 100
#define BUFSIZE 256
#define DEBUG 0

char *symbols[] = {"@R0", "@R1", "@R2", "@R3", "@R4",
                   "@R5", "@R6", "@R7", "@R8", "@R9",
                   "@R10", "@R11", "@R12", "@R13", "@R14",
                   "@R15",
                   "@SCREEN", "@KBD", "@SP", "@LCL",
                   "@ARG", "@THIS", "@THAT"};

int predefinitions[] = {0, 1, 2, 3, 4, 5, 6, 7, 8,
                         9, 10, 11, 12, 13, 14, 15, 16,
                         16384, 24576, 0, 1, 2, 3, 5};

MapEntry *lookup_table[MAXSIZE] = {0};
int table_size = 0;

int current_addres_for_variable = 16;

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Error: usage: a.out path_to_file");
        exit(EXIT_FAILURE);
    }

    // creates pre-define table
    preprocess_table();

    char *next_file, *prev_file;
    prev_file = argv[1];
    
    // conducts three iteration over the file:
    // first one: get rid of comments and white spaces
    // second one: handle labels (jump references)
    // third one: substitute variable names,
    // that was added to the mappign table during second pass
    for (int pass = 0; pass < 3; pass++) {
        next_file = iter_over_lines(prev_file, pass);
        if (DEBUG)
            printf("%s; prev: %s\n", next_file, prev_file);
        if (pass != 0)
            remove(prev_file); 
        prev_file = next_file;
    }

    prev_file = NULL;
    printf("%s; prev: %s\n", next_file, prev_file);

    // reads from a final pre-processed file and writes to
    // binary file
    char line[MAXSIZE];
    int length;

    FILE *read_from;
    FILE *res;
    read_from = fopen(next_file, "r");

    char *write_to = get_result_path(argv[1]);
    res = fopen(write_to, "ab");

    while ((length = get_line(line, MAXSIZE, read_from)) != EOF) {
        parse_line(line, res);
    }

    // cleaning up
    free(write_to);
    free(next_file);
    remove(next_file);
}


/*
 * accepts pointer to a file and an int verbose that determines
 * how to process each line
 */
char *iter_over_lines(const char *path, int pass) {
    FILE *fptr, *fptr_to_write;
    fptr = fopen(path, "r");
    if (fptr == NULL) {
        fprintf(stderr, "Could not open the file for reading\n");
        exit(EXIT_FAILURE);
    }

    // buffer - line will be written in it
    // line - processes line will be placed in it
    char *buffer;
    char *line;
    size_t bufsize = 256;

    buffer = (char *)malloc(bufsize * sizeof(char));
    line = (char *)malloc(bufsize * sizeof(char));
    int length = 0;

    char *path_to_write = get_path_to_write(path);

    fptr_to_write = fopen(path_to_write, "w");
    if (fptr_to_write == NULL) {
        fprintf(stderr, "Could not open the file %s to write\n", path_to_write);
        exit(EXIT_FAILURE);
    } else {
        // printf("FIle at this path was successfully openned: %s\n", new_path);
        ;
    }

    int instructions_written = 0; // keeps treck of the current instruction number
    while ((length = get_line(buffer, bufsize, fptr)) != -1) {
        // there is some check for 13' Ascii carriage return ??
        // iteration over a line, to get rid of comments and white spaces

        int sym_to_write;
        switch (pass)
        {
        case 0:
            sym_to_write = process_line(buffer, line, &instructions_written);
            break;
        case 1:
            sym_to_write = iterate_over(buffer, length, line, 1);
            break;
        case 2:
            sym_to_write = iterate_over(buffer, length, line, 0);
            break;
        default:
            fprintf(stderr, "Pass was not specifed during file processing");
            exit(EXIT_FAILURE);
        }

        if (sym_to_write != 0) {
            fprintf(fptr_to_write, "%s\n", line);
        }

        // fprintf(fptr_to_write, "%s\n", line);
        memset(line, 0, bufsize);
        memset(buffer, 0, bufsize);
    }

    free(buffer);
    free(line);
    if (fclose(fptr) == EOF) {
        fprintf(stderr, "Could not close the file 1\n");
        exit(EXIT_FAILURE);
    }

    if (fclose(fptr_to_write)) {
        fprintf(stderr, "Could not close the file 2 \n");
        exit(EXIT_FAILURE);
    }

    return path_to_write;
}

/* Finds the address of an entry in the lookup table,
 * can be easily improved (TODO), for instance, with
 * binary search */
int find_address(char *target){
    int i = 0;
    int found = 0;

    if (DEBUG)
        printf("Target of find address: %s\n", target);

    for (i = 0; i < table_size; i++) {
        if (strcmp(lookup_table[i]->label, target) == 0){
            if (DEBUG)
                printf("%s found at location %d.\n", target, i);
            found = !found;
            break;
        }
    }

    if (DEBUG)
        printf("Found: %d\n", found);

    if (found) {
        return lookup_table[i]->address;
    }

    return -1;
}

int iterate_over(char *buffer, int symbols_written, char *line, int handle_var) {
    // char label[MAXSIZE] = {0};
    int label_size = 0;
    char ch;
    int i = 0;

    if (buffer[0] == '@') {
        int lookup = find_address(buffer);
        if (lookup == -1) {
            if (isdigit(buffer[1]) != 0) {
                // then it is a simple address that looks like @345,
                // which just loads this to A register, therefore
                // there is no need to handle it somehow
                strcpy(line, buffer);
            } else {
                if (handle_var == 1) {
                    get_available_address();
                    // printf("Received free adress: %d\n", current_addres_for_variable);
                    char *line_to_write;
                    // there is need to allocate this memory, so it would be saved
                    // and wouldn't disapper from the global lookup table
                    line_to_write = (char *) calloc(BUFSIZE, sizeof(char));
                    strcpy(line_to_write, buffer);

                    // printf("ADING: %s %d\n", line_to_write, current_addres_for_variable);
                    // creates an entry to the lookup table, which will handle VARIABLE
                    lookup_table[table_size++] = create_entry(line_to_write, current_addres_for_variable);
                    
                    // substitutes its already in the file with its address, so to make
                    // the other pass less work
                    char lookup_str[MAXSIZE];
                    sprintf(lookup_str, "@%d", current_addres_for_variable);
                    strcpy(line, lookup_str);
                } else {
                    fprintf(stderr, "Error third passing the file\n");
                }
            }
        } else {
            // creates a new command, substituting the (LABEL) with its
            // address
            char lookup_str[MAXSIZE];
            sprintf(lookup_str, "@%d", lookup);   
            // printf("Lookup: %s\n", lookup_str);
            strcpy(line, lookup_str);
        }
    } else {
        strcpy(line, buffer);
    }

    return 1;
}


/* Finds first available address to allocate
 * for the seen variable. By default, it
 * starts searching from address 16 (as it is
 * given in the specification) */
void get_available_address(void) {
    int addresses[MAXSIZE] = {0};
    int adr_counter = 0;

    // gets all the addresses larger than 16,
    // so to avoid collisions later on
    for (int i = 0; i < table_size; i++) {
        addresses[adr_counter++] = lookup_table[i]->address;
    }

    while (in_arr(addresses, NUM(addresses), current_addres_for_variable) != 0)
        current_addres_for_variable += 1;
}


/* Checks whehter target is in array,
 * (helper function that will be used during
 * lookup in the lookup table) */
int in_arr(int *arr, int arr_size, int target) {
    int verbose = 0;
    for (int i = 0; i < arr_size; i++) {
        if (arr[i] == target) {
            verbose = !verbose;
            return 1;
        }
    }

    return 0;
}


/* Pre-proccesses a line: gets rid of blank spaces
 * and comments.
 * Returns:
 *      int length of the symbols that actually make
 * sense to write them (pass to the second file proccessment) */
int process_line(char *buffer, char *line, int *iter) {
    int symbols_to_write = 0;
    int verbose = 1;

    for (int j = 0; j < BUFSIZE; j++) {
        // there is some check for 13' Ascii carriage return ??
        // iteration over a line, to get rid of comments and white spaces
        if ((buffer[j] == '/') || ((int)buffer[j] == 13) || (buffer[j] == '\n')) {
            if (j == 0)
                verbose = 0;
            break;
        } else if (buffer[j] == ' ') {
            continue;
        } else if (buffer[j] == '(') {
            process_label(buffer, &j, iter);
        } else {
            line[symbols_to_write++] = buffer[j];
        }
    }

    line[symbols_to_write] = '\0';

    if (verbose != 0) 
        (*iter)++; 

    return symbols_to_write;
}


/* Proccesses a lable (LABEL)
 * Puts the label with the following addres to the */
void process_label(char *buf, int *symbol, int *iter) {
    // adds label to the hash table
    char temp_label[100] = {0};
    char curch;
    int label_size = 0;

    temp_label[label_size++] = '@';
    while ((curch = buf[++(*symbol)]) != ')') {
        temp_label[label_size++] = curch;
    }
    temp_label[label_size] = '\0';

    char *label = (char *) calloc(label_size, sizeof(char));
    strcpy(label, temp_label);

    lookup_table[table_size++] = create_entry(label, *iter);
    memset(temp_label, 0, label_size);
}


/* Allocate memory for a new entry */
MapEntry *create_entry(char *label, int address) {
    // printf("Args: %s %d\n", label, address);
    MapEntry *newEntry = (MapEntry *) malloc(sizeof(MapEntry));
    if (newEntry == NULL) {
        fprintf(stderr, "Could not create a new entry\n");
        exit(EXIT_FAILURE);
    }
    
    newEntry->label = label;
    newEntry->address = address;

    return newEntry;
}


/* Intermediate file's path creation */
char *get_path_to_write(const char *path) {
    char add_str[] = "_no_comments";
    char *new_path;

    new_path = (char *) calloc(MAXSIZE, sizeof(char));
    memset(new_path, 0, MAXSIZE);
    // new path creation
    strcat(new_path, path);
    strcat(new_path, add_str);
    if (DEBUG)
        printf("The path to write: %s\n", new_path);

    return new_path;
}

// gets line from a file
int get_line(char *buf, size_t bufsize, FILE *fptr) {
    int count = 0;
    char ch = getc(fptr);
    char *lineBuffer = (char *)malloc(sizeof(char) * bufsize);

    while ((ch != '\n') && (ch != EOF)) {
        if (count == bufsize) {
            bufsize += 128;
            lineBuffer = realloc(lineBuffer, bufsize);
            if (lineBuffer == NULL) {
                fprintf(stderr, "Error reallocating space for line buffer.");
                exit(EXIT_FAILURE);
            }
        }
        lineBuffer[count++] = ch;
        ch = getc(fptr);
    }

    lineBuffer[count] = '\0';
    strncpy(buf, lineBuffer, (count + 1));
    free(lineBuffer);

    if (ch == EOF)
        return -1;

    return count;
}


/* Gets the path, where to write file */
char *get_result_path(char *path) {
    char *out;
    out = (char *) calloc(MAXSIZE, sizeof(char));
    int out_iter = 0;

    char ch;
    int i = 0;
    while ((ch = path[i++]) != '.') {
        out[out_iter++] = ch;
    }
    char str_to_add[] = ".bin";
    strcat(out, str_to_add);
    printf("Resulting file: %s\n", out);
    
    return out;
}


/* Creates an entry for each element of two globals lists.
 * TODO: those two lists can be moved here */
void preprocess_table(void) {
    for (int i = 0; i < NUM(symbols); i++) {
        lookup_table[table_size++] = create_entry(symbols[i], predefinitions[i]);
    }
}


/* Prints out hte lookup table */
void printTable(MapEntry **table, size_t table_size) {
    for (int i = 0; i < table_size; i++) {
        printf("%d: %s %d\n", i, table[i]->label, table[i]->address);
    }
    printf("-----------\n");
}
