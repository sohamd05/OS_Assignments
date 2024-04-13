#include <stdio.h>

int main(int argc, char *argv[]) {
    // Rename file
    if (rename("/tmp/testfile", "/tmp/renamed_testfile") == -1) {
        perror("Error renaming file");
        return 1;
    }
    
    return 0;
}

