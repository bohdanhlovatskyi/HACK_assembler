#include "assembler.h"

#define NUM(a) sizeof(a) / sizeof(a[0])
#define MAXSIZE 100
#define BUFSIZE 256
#define DEBUG 0

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
    preprocess_table(lookup_table, table_size);

    const char test[] = "rect/Rect.asm";
    iter_over_lines(test, 0);
    printTable(lookup_table, table_size);

    // TABLE TEST
    // int return_address = find_address(lookup_table, table_size, "LOOP");
    // printf("%d\n", return_address);

    iter_over_lines("rect/Rect.asm_no_comments", 1);
    
}

void preprocess_table(MapEntry **table, size_t table_size) {
    for (int i = 0; i < NUM(symbols); i++) {
        lookup_table[table_size++] = create_entry(symbols[i], predefinitions[i]);
    }
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
void iter_over_lines(const char *path, int pass) {
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

        int sym_to_write;
        switch (pass)
        {
        case 0:
            sym_to_write = process_line(buffer, line, &instructions_written);
            break;
        case 1:
            sym_to_write = second_pass(buffer, length, line);
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
}

int find_address(MapEntry **table, int table_size, char *target){
    int bottom= 0;
    int mid;
    int top = table_size - 1;
    int found = 0;

    while(bottom <= top){
        mid = (bottom + top)/2;
        if (strcmp(table[mid]->label, target) == 0){
            if (DEBUG != 0)
                printf("%s found at location %d.\n", target, mid+1);
            found = !found;
            break;
        }
        if (strcmp(table[mid]->label, target) < 0){
            top = mid - 1;
        }
        if (strcmp(table[mid]->label, target) > 0){
            bottom = mid + 1;
        }
    }

    if (found)
        return table[mid + 1]->address;
    return -1;
}

// modifies line
int second_pass(char *buffer, int symbols_written, char *line) {
    // char label[MAXSIZE] = {0};
    int label_size = 0;
    char ch;
    int i = 0;

    if (buffer[0] == '@') {
        int lookup = find_address(lookup_table, table_size, buffer);
        if (lookup == -1) {
            if (isdigit(buffer[1]) != 0) {
                strcpy(line, buffer);
            } else {
                // this should handle the variables
                // int address = get_available_address(lookup_table, table_size);
                // lookup_table[table_size++] = create_entry(buffer, address);
                strcpy(line, buffer);
            }
        } else {
            // creates a new command, substituting the (LABEL) with its
            // address
            char *lookup_str;
            sprintf(lookup_str, "@%d", lookup);   
            // printf("Lookup: %s\n", lookup_str);
            strcpy(line, lookup_str);
        }
    } else {
        strcpy(line, buffer);
    }

    return 1;
}

int get_available_address(MapEntry **table, size_t table_size) {
    int *addresses;
    int adr_counter = 0;
    for (int i = 0; i < table_size; i++) {
        addresses[adr_counter++] = table[i]->address;
    }
    // TODO: find lowerst possible not taken addres starting from 16
    // and return it

    return 0;
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

    line[symbols_to_write] = '\0';

    // for (int i = 0; i < symbols_to_write; i++)
    //    printf("%c(%d) ", line[i], (int)line[i]);

    if (verbose != 0) 
        (*iter)++; 

    return symbols_to_write;
}

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
    char new_path[64] = {0};
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
    } else {
        // printf("FIle at this path was successfully openned: %s\n", new_path);
        ;
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
