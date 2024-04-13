#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>

int main(int argc, char *argv[]) {
    int fd;
    int count;
    int ino, i;
    int bgno, iindex, inode_size, block_size;
    unsigned long inode_offset;
    struct ext2_super_block sb;
    struct ext2_inode inode;
    struct ext2_group_desc bgdesc;

    // Check if the correct number of arguments is provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filesystem image> <inode number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Convert the inode number argument to integer
    ino = atoi(argv[2]);

    // Open the filesystem image file in read-only mode
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening filesystem image");
        exit(EXIT_FAILURE);
    }

    // Seek past the boot block
    if (lseek(fd, 1024, SEEK_SET) == -1) {
        perror("Error seeking past boot block");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Read the superblock
    count = read(fd, &sb, sizeof(struct ext2_super_block));
    if (count != sizeof(struct ext2_super_block)) {
        perror("Error reading superblock");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Print superblock information
    printf("Magic: %x\n", sb.s_magic);
    printf("Inode count: %d\n", sb.s_inodes_count);

    // Calculate block and inode size
    inode_size = sb.s_inode_size;
    block_size = 1024 << sb.s_log_block_size;

    // Calculate block group number and inode index
    bgno = (ino - 1) / sb.s_inodes_per_group;
    iindex = (ino - 1) % sb.s_inodes_per_group;

    // Seek to the block group descriptor table
    if (lseek(fd, 1024 + block_size + bgno * sizeof(bgdesc), SEEK_SET) == -1) {
        perror("Error seeking to block group descriptor");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Read the block group descriptor
    count = read(fd, &bgdesc, sizeof(struct ext2_group_desc));
    if (count != sizeof(struct ext2_group_desc)) {
        perror("Error reading block group descriptor");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Print inode table location
    printf("Inode Table: %d\n", bgdesc.bg_inode_table);

    // Calculate offset of the specified inode within the inode table
    inode_offset = bgdesc.bg_inode_table * block_size + iindex * inode_size;

    // Seek to the specified inode
    if (lseek(fd, inode_offset, SEEK_SET) == -1) {
        perror("Error seeking to inode");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Read the inode
    count = read(fd, &inode, sizeof(inode));
    if (count != sizeof(inode)) {
        perror("Error reading inode");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Print inode size and block numbers
    printf("Size of file: %d\n", inode.i_size);
    printf("Number of blocks: %d, Blocks:", inode.i_blocks);

    for (i = 0; i < inode.i_blocks; i++) {
        printf("%d", inode.i_block[i]);
        if (i < inode.i_blocks - 1) {
            printf(", ");
        }
    }
    printf("\n");

    // Close the file descriptor
    close(fd);

    return 0;
}
