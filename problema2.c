#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libgen.h>

bool isFile(char* path) {
    struct stat sb;
    if (stat(path, &sb) == -1) return false;
    return (sb.st_mode & S_IFMT) == S_IFREG;
}

bool isAvailableDirectory(char* dir) {
    struct stat sb;
    if (stat(dir, &sb) == -1) return false;
    if ((sb.st_mode & S_IFMT) != S_IFDIR &&
        (sb.st_mode & S_IFMT) != S_IFLNK) return false;
    if (access(dir, W_OK) != 0) return false;
    return true;
}

ssize_t copy(char* src, char* dst) {
    struct stat sb;
    if (stat(src, &sb) == -1) return -1;

    int srcFd = open(src, O_RDONLY);
    int dstFd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, sb.st_mode);
    if (srcFd == -1 || dstFd == -1) return -1;
    
    ssize_t ret = sb.st_size;
    size_t blockSize = 512;
    unsigned char buf[512];
    for (blkcnt_t i = 0; i < sb.st_blocks; i++) {
        ssize_t readBytes = read(srcFd, buf, blockSize);
        if (readBytes == -1) {
            ret = -1;
            break;
        } 

        ssize_t writtenBytes = write(dstFd, buf, readBytes);
        if (writtenBytes == -1) {
            ret = -1;
            break;
        }
    }

    close(srcFd);
    close(dstFd);

    return ret;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <src> <dst>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* src = argv[1];
    char* dst = argv[2];

    if (!isFile(src)) {
        fprintf(stderr, "%s is not a regular file or does not exist\n", src);
        exit(EXIT_FAILURE);
    }

    char *dstDir = dirname(dst);
    if (!isAvailableDirectory(dstDir)) {
        fprintf(stderr, "%s does not exists or you lack permissions\n", dstDir);
        exit(EXIT_FAILURE);
    }

    ssize_t transfered = copy(src, dst);
    if (transfered == -1) {
        fprintf(stderr, "error while copying\n");
        return -1;
    }

    printf("written %ld bytes to %s\n", transfered, dst);

    return 0;
}
