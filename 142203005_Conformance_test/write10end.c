#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Open file for appending
    FILE *file = fopen("/tmp/testfile", "a");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    
    // Write 10 bytes at the end of the file
    char data[11];
    memset(data, 'B', 10);
    data[10] = '\0';
    
    fputs(data, file);
    fclose(file);
    
    return 0;
}

