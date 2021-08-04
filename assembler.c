#include "assembler.h"

#define NUM(a) sizeof(a) / sizeof(a[0])
#define MAXSIZE 100
#define BUFSIZE 256

char *symbols[] = {"R0", "R1", "R2", "R3", "R4",
                   "R5", "R6", "R7", "R8", "R9",
                   "R10", "R11", "R12", "R13", "R14",
                   "R15",
                   "SCREEN", "KBD", "SP", "LCL",
                   "ARG", "THIS", "THAT"};

// I know about enum... just lazy LMAO
int predefinitions[] = {0, 1, 2, 3, 4, 5, 6, 7, 8,
                         9, 10, 11, 12, 13, 14, 15, 16,
                         16384, 24576, 0, 1, 2, 3, 5};

MapEntry *lookup_table[MAXSIZE] = {0};
size_t table_size = 0;

int main(int argc, char **argv) {
    /*
    if (argc != 2) {
        printf("Error: usage: a.out path_to_file");
        exit(EXIT_FAILURE);
    }
    */

    // create pre-define table
    for (int i = 0; i < NUM(symbols); i++) {
        lookup_table[table_size++] = create_entry(symbols[i], predefinitions[i]);
    }

    const char test[] = "max/Max.asm";
    iter_over_lines(test, 0);
    printTable(lookup_table, table_size);
}

void printTable(MapEntry **table, size_t table_size) {
    for (int i = 0; i < table_size; i++) {
        printf("%d: %s %d\n", i, table[i]->label, table[i]->address);
    }
    printf("-----------\n");
}

/*
 * accepts pointer to a file and a function to apply at each line
 */
void iter_over_lines(const char *path, int here_should_go_func) {
    FILE *fptr;
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

    FILE *fptr_to_write = get_file_to_write(path);

    int instructions_written = 0; // keeps treck of the current instruction number
    while ((length = get_line(buffer, bufsize, fptr)) != -1) {
        // there is some check for 13' Ascii carriage return ??
        // iteration over a line, to get rid of comments and white spaces
        int sym_to_write = process_line(buffer, line, &instructions_written);

        if (sym_to_write != 0) {
            // printf("%s\n", line);
            fprintf(fptr_to_write, "%s\n", line);
        }

        // fprintf(fptr_to_write, "%s\n", line);
        memset(line, 0, bufsize);
        memset(buffer, 0, bufsize);
    }

    free(buffer);
    free(line);
    fclose(fptr);
    fclose(fptr_to_write);
}

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


    if (verbose != 0) 
        (*iter)++; 

    return symbols_to_write;
}

void process_label(char *buf, int *symbol, int *iter) {
    // adds label to the hash table
    char temp_label[100] = {0};
    char curch;
    int label_size = 0;

    while ((curch = buf[++(*symbol)]) != ')') {
        temp_label[label_size++] = curch;
    }
    temp_label[label_size] = '\0';

    char *label = (char *) calloc(label_size, sizeof(char));
    strcpy(label, temp_label);
    // printf("%s\n", label);
    // printf("%d\n", (*iter) + 1);

    lookup_table[table_size++] = create_entry(label, *iter);
    memset(temp_label, 0, label_size);
}


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


FILE *get_file_to_write(const char *path) {
    char add_str[] = "_no_comments";
    char new_path[32] = {0};
    FILE *fptr;

    // new path creation
    strcat(new_path, path);
    strcat(new_path, add_str);
    printf("The path to write: %s\n", new_path);

    // file opennign handler
    fptr = fopen(new_path, "w");
    if (fptr == NULL) {
        fprintf(stderr, "Could not open the file %s to write\n", new_path);
        exit(EXIT_FAILURE);
    }

    return fptr;
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
                printf("Error reallocating space for line buffer.");
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
