#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <size_in_MB>\n", argv[0]);
        return 1;
    }
    
    long size_mb = atol(argv[1]);
    long size_bytes = size_mb * 1024 * 1024;
    
    FILE *file = fopen("output.txt", "w");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }
    
    const char *text = "Hello world\n";
    size_t text_len = strlen(text);
    
    long written = 0;
    while (written < size_bytes) {
        size_t to_write = (size_bytes - written > text_len) ? text_len : (size_bytes - written);
        fwrite(text, 1, to_write, file);
        written += to_write;
    }
    
    fclose(file);
    printf("Generated file of approximately %ld MB.\n", size_mb);
    return 0;
}