#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>

#define EXT2_SUPER_MAGIC 0xEF53

typedef struct ext2_super_block superblock_t;
typedef struct ext2_group_desc group_desc_t;
typedef struct ext2_inode inode_t;
typedef struct ext2_dir_entry_2 directory_entry_t;

int get_inode(int fd, const char *path);
void read_inode(int fd, int inode_number);
void read_directory(int fd, inode_t *inode);
void read_data(int fd, inode_t *inode);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s device-file-name path-on-partition inode/data\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *device_name = argv[1];
    const char *path = argv[2];
    const char *mode = argv[3];

    int fd = open(device_name, O_RDONLY);
    if (fd < 0) {
        perror("Error opening device");
        return EXIT_FAILURE;
    }

    int inode_number = get_inode(fd, path);
    if (inode_number < 0) {
        printf("Error: File or directory not found.\n");
        close(fd);
        return EXIT_FAILURE;
    }

    if (strcmp(mode, "inode") == 0) {
        printf("%d\n", inode_number);
        read_inode(fd, inode_number);
    } else if (strcmp(mode, "data") == 0) {
        inode_t inode;
        lseek64(fd, 1024 + sizeof(superblock_t) + sizeof(group_desc_t), SEEK_SET); // Move to inode table
        lseek64(fd, (inode_number - 1) * sizeof(inode_t), SEEK_CUR); // Move to the desired inode
        read(fd, &inode, sizeof(inode_t));
        read_data(fd, &inode);
    } else {
        printf("Invalid mode. Use 'inode' or 'data'\n");
    }

    close(fd);
    return EXIT_SUCCESS;
}

int get_inode(int fd, const char *path) {
    // Read the superblock
    superblock_t superblock;
    if (pread(fd, &superblock, sizeof(superblock_t), 1024) != sizeof(superblock_t)) {
        perror("Error reading superblock");
        return -1;
    }

    if (superblock.s_magic != EXT2_SUPER_MAGIC) {
        printf("Error: Not an ext2 filesystem.\n");
        return -1;
    }

    // Start traversing from root inode
    inode_t inode;
    int current_inode = EXT2_ROOT_INO;

    char *token;
    char *path_copy = strdup(path);
    token = strtok(path_copy, "/");

    while (token != NULL) {
        lseek64(fd, 1024 + superblock.s_inode_size * current_inode, SEEK_SET); // Move to the desired inode
        read(fd, &inode, sizeof(inode_t));

        // Search for directory entry with given name
        directory_entry_t dir_entry;
        int offset = 0;
        int found = 0;

        while (offset < inode.i_size && !found) {
            pread(fd, &dir_entry, sizeof(directory_entry_t), inode.i_block[0] * 1024 + offset);

            if (strncmp(dir_entry.name, token, dir_entry.name_len) == 0) {
                current_inode = dir_entry.inode;
                found = 1;
            }

            offset += dir_entry.rec_len;
        }

        if (!found) {
            printf("Error: Path not found.\n");
            return -1;
        }

        token = strtok(NULL, "/");
    }

    free(path_copy);
    return current_inode;
}

void read_inode(int fd, int inode_number) {
    inode_t inode;
    lseek64(fd, 1024 + sizeof(superblock_t) + sizeof(group_desc_t), SEEK_SET); // Move to inode table
    lseek64(fd, (inode_number - 1) * sizeof(inode_t), SEEK_CUR); // Move to the desired inode
    read(fd, &inode, sizeof(inode_t));

    // Print inode information
    printf("Inode number: %d\n", inode_number);
    printf("File size: %d bytes\n", inode.i_size);
    printf("Number of blocks: %d\n", inode.i_blocks);
    // Print more inode information as needed
}

void read_directory(int fd, inode_t *inode) {
    // Check if inode is a directory
    if (!(inode->i_mode & S_IFDIR)) {
        printf("Error: Not a directory.\n");
        return;
    }

    // Read directory entries
    directory_entry_t dir_entry;
    int offset = 0;

    while (offset < inode->i_size) {
        pread(fd, &dir_entry, sizeof(directory_entry_t), inode->i_block[0] * 1024 + offset);
        printf("Name: %.*s\n", dir_entry.name_len, dir_entry.name);
        offset += dir_entry.rec_len;
    }
}

void read_data(int fd, inode_t *inode) {
    // Check if inode is a regular file
    if (!(inode->i_mode & S_IFREG)) {
        printf("Error: Not a regular file.\n");
        return;
    }

    // Read file data
    char buffer[inode->i_size];
    pread(fd, buffer, inode->i_size, inode->i_block[0] * 1024);
    printf("File content:\n%s\n", buffer);
}
