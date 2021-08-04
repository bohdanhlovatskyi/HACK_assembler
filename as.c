#include "assembler.h"

#define NUM(a) sizeof(a) / sizeof(a[0])
#define INT2VOIDP(i) (void*)(uintptr_t)(i)
#define VOIDP2INT(ptr) (int *)(ptr)

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

MapEntry **lookup_table;
size_t table_size = 0;

int main(int argc, char **argv) {
    /*
    if (argc != 2) {
        printf("Error: usage: a.out path_to_file");
        exit(EXIT_FAILURE);
    }
    */

    // char test_str[] = "Test";
    // MapEntry test = *create_entry(test_str, 5); 
    /*
    for (int i = 0; i < NUM(symbols); i++) {
        lookup_table[table_size++] = create_entry(symbols[i], predefinitions[i]);
    }
    */
    char test[] = "max/MaxL.asm";
    // iter_over_lines(argv[1], 0);
    iter_over_lines(test, 0);
    exit(EXIT_SUCCESS);
}

MapEntry *create_entry(char *label, int address) {
    printf("Args: %s %d\n", label, address);
    MapEntry *newEntry = (MapEntry *) malloc(sizeof(MapEntry));
    if (newEntry == NULL) {
        fprintf(stderr, "Could not create a new entry\n");
        exit(EXIT_FAILURE);
    }
    
    newEntry->label = label;
    newEntry->address = address;

    return newEntry;
}

/*
 * accepts pointer to a file and a function to apply at each line
 */
void iter_over_lines(const char *path, int here_should_go_func) {
    FILE *fptr;

    char *buffer;
    char *line;
    size_t bufsize = 64;
    char ch;

    fptr = fopen(path, "r");
    if (fptr == NULL) {
        fprintf(stderr, "Could not open file to read\n");
        exit(EXIT_FAILURE);
    }

    // create a new path and open the file
    char add_str[] = "_no_comments";
    char new_path[32] = {0};
    strcat(new_path, path);
    strcat(new_path, add_str);
    printf("%s\n", new_path);
    FILE *fptr_to_write = fopen(new_path, "w");
    if (fptr_to_write == NULL) {
        fprintf(stderr, "Could not open the file %s to write\n", new_path);
        exit(EXIT_FAILURE);
    }

    buffer = (char *)malloc(bufsize * sizeof(char));
    // memset(buffer, 0, bufsize);
    int length = 0;

    int iter = 0;
    while ((length = get_line(buffer, bufsize, fptr)) != -1) {
        // there is some check for 13' Ascii carriage return ??
        // iteration over a line, to get rid of comments and white spaces
        int i = 0; // iterator over line
        int verbose = 1;
        for (int j = 0; j < bufsize; j++) {
            if ((buffer[j] == '/') || ((int)buffer[j] == 13) || (buffer[j] == '\n')) {
                if (j == 0) {
                    verbose = !verbose; // skip the line, if it is not 'preatry' at the first iteration
                }
                break;
            } else if (buffer[j] == ' ') {
                continue;
            } else if (buffer[j] == '(') {
                // adds label to the hash table
                char label[100];
                char curch;
                int label_size = 0;

                while ((curch = buffer[++j]) != ')') {
                    label[label_size++] = curch;
                }
                label[label_size] = '\0';
                // printf("%s\n", label);
                // printf("%d\n", iter);
                // lookup_table[table_size++] = create_entry(label, iter);

                memset(label, 0, label_size);
            } else {
                line[i++] = buffer[j];
            }
        }

        if (verbose)
            iter++; 

        if (i != 0) {
            printf("Writing to file");
            fprintf(fptr_to_write, "%s\n", line);
        }
        // memset(line, 0, bufsize);
        memset(buffer, 0, bufsize);
    }

    fclose(fptr);
    fclose(fptr_to_write);
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
    printf("%s\n", lineBuffer);
    strncpy(buf, lineBuffer, count + 1);
    free(lineBuffer);

    if (ch == EOF)
        return -1;

    return count;
}
