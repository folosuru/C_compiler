#include <errno.h>
#include <stdio.h>
#include <string.h>

char* file_load_buf(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == 0) {
        fprintf(stderr, "file load err");
        return 0;
    }

    if (fseek(file, 0, SEEK_END) == -1) {
        fprintf(stderr, "file load err");
        return 0;
    }
    size_t size = ftell(file);
    if (fseek(file, 0, SEEK_SET) == -1) {
        fprintf(stderr, "file load err");
        return 0;
    }
    char *buf = calloc(1, size + 2);
    fread(buf, size, 1, file);
    fclose(file);
    if (size == 0 || buf[size - 1] != '\n') {
        buf[size++] = '\n';
    }
    buf[size] = '\0';
    return buf;
}