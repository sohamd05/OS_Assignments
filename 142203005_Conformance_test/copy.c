#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Open source file for reading
    FILE *source = fopen("/tmp/testfile", "r");
    if (source == NULL) {
        perror("Error opening source file");
        return 1;
    }
    
    // Open destination file for writing
    FILE *destination = fopen("/tmp/testfile_copy", "w");
    if (destination == NULL) {
        perror("Error opening destination file");
        fclose(source);
        return 1;
    }
    
    // Copy contents from source to destination
    int character;
    while ((character = fgetc(source)) != EOF) {
        fputc(character, destination);
    }
    
    fclose(source);
    fclose(destination);
    
    return 0;
}

