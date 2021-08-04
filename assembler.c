#include "assembler.h"

#define NUM(a) sizeof(a) / sizeof(a[0])

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error: usage: a.out path_to_file");
        exit(EXIT_FAILURE);
    }

    // hcreate(100);
    iter_over_lines(argv[1], 0);
}

/*
 * accepts pointer to a file and a function to apply at each line
 */
void iter_over_lines(const char *path, int here_should_go_func) {
    FILE *fptr;

    char *buffer;
    char *line;
    size_t bufsize = 256;
    char ch;

    fptr = fopen(path, "r");
    buffer = (char *)malloc(bufsize * sizeof(char));
    int length = 0;

    // create a new path and open the file
    char add_str[] = "_no_comments";
    char new_path[100];
    strcat(new_path, path);
    strcat(new_path, add_str);
    printf("%s\n", new_path);
    FILE *fptr_to_write = fopen(new_path, "w");
    if (fptr_to_write == NULL) {
        fprintf(stderr, "Could not open the file %s to write\n", new_path);
        exit(EXIT_FAILURE);
    }

    int iter = 0;
    while ((length = get_line(buffer, bufsize, fptr)) != -1) {
        // there is some check for 13' Ascii carriage return ??
        // iteration over a line, to get rid of comments and white spaces
        int i = 0; // todo wtf???
        int verbose = 1;
        for (int j = 0; j < bufsize; j++) {
            if ((buffer[j] == '/') || ((int)buffer[j] == 13) || (buffer[j] == '\n')) {
                if (j == 0)
                    verbose = 0;
                break;
            } else if (buffer[j] == ' ') {
                continue;
            } else if (buffer[j] == '(') {
                // adds label to the hash table
                char label[100];
                char curch;
                int label_size = 0;
                // ENTRY e, *ep;

                while ((curch = buffer[++j]) != ')') {
                    label[label_size++] = curch;
                }
                label[label_size] = '\0';
                printf("%s\n", label);
                printf("%d\n", iter + 1);

                // e.key = label; 
                // e.data = (void *)(iter + 1);
                memset(label, 0, label_size);
            } else {
                line[i++] = buffer[j];
            }
        }

        if (verbose != 0) 
            iter++; 

        if (i != 0)
            fprintf(fptr_to_write, "%s\n", line);
        memset(line, 0, bufsize);
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
    strncpy(buf, lineBuffer, (count + 1));
    free(lineBuffer);

    if (ch == EOF)
        return -1;

    return count;
}
