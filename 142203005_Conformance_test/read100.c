#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Open file for writing
    FILE *file = fopen("/tmp/testfile", "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    
    // Write 100 bytes of data
    char data[101];
    memset(data, 'A', 100);
    data[100] = '\0';
    
    fputs(data, file);
    fclose(file);
    
    return 0;
}

