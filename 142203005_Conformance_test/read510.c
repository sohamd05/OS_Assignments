#include <stdio.h>

int main(int argc, char *argv[]) {
    // Open file for read
    FILE *file = fopen("/tmp/testfile", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    
    // Move to byte number 5
    fseek(file, 5, SEEK_SET);
    
    // Read 6 bytes (from byte number 5 to 10)
    char data[7];
    if (fread(data, 1, 6, file) != 6) {
        perror("Error reading file");
        fclose(file);
        return 1;
    }
    data[6] = '\0';
    
   // printf("%s\n", data);
    
    fclose(file);
    
    return 0;
}

