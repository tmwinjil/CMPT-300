#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "restart.h"
#include "fileio.h"

#if 1
#define VERBOSE(p) (p)
#else
#define VERBOSE(p) (0)
#endif

int file_read(char *path, int offset, void *buffer, size_t bufbytes)
{
    if (!path || !buffer || bufbytes < 1 || offset < 0)
        return IOERR_INVALID_ARGS; 

    int fd = r_open2(path, O_RDONLY);
    if (fd == -1) //file failed to open
        return IOERR_INVALID_PATH;

    if(lseek(fd, (off_t) offset, SEEK_SET) == (off_t) -1) {
        r_close(fd);
        return IOERR_POSIX;
    }
    size_t bytesRd = r_read(fd, buffer, bufbytes);
    r_close(fd);
    if (bytesRd == -1) {
       return IOERR_POSIX;
    }else {
        return (int) bytesRd;
    }
}

int file_info(char *path, void *buffer, size_t bufbytes)
{
    if (!path || !buffer || bufbytes < 1)
	    return IOERR_INVALID_ARGS;
    struct stat info;
    char type = 'X';
    if(stat(path, &info) == -1)
        return IOERR_POSIX;

    if (S_ISREG(info.st_mode))
        type = 'f';
    else if(S_ISDIR(info.st_mode))
        type = 'd';
        
    sprintf((char*)buffer,"Size:%ld Accessed:%ld Modified:%ld Type %c",info.st_size, info.st_atime, info.st_mtime, type);
    return 0;
}

int file_write(char *path, int offset, void *buffer, size_t bufbytes)
{
    if (!path || !buffer || bufbytes < 1 || offset < 0)
        return IOERR_INVALID_ARGS; 

    int fd = r_open2(path, O_WRONLY);
    if (fd == -1)//file failed to open
        return IOERR_INVALID_PATH;
    if(lseek(fd, (off_t) offset, SEEK_SET) == (off_t) -1) {
        r_close(fd);
        return IOERR_POSIX;
    }
    size_t bytesRd = r_write(fd, buffer, bufbytes);
    r_close(fd);
    if (bytesRd == -1)
       return IOERR_POSIX;
    else
        return (int) bytesRd;
}

int file_create(char *path, char *pattern, int repeatcount)
{
    if (!path || !pattern || repeatcount < 1)
        return IOERR_INVALID_ARGS;
    return IOERR_NOT_YET_IMPLEMENTED;
}

int file_remove(char *path)
{
    if (!path)
        return IOERR_INVALID_ARGS;
    return IOERR_NOT_YET_IMPLEMENTED;
}

int dir_create(char *path)
{
    if (!path)
        return IOERR_INVALID_ARGS;

    return IOERR_NOT_YET_IMPLEMENTED;
}

int dir_list(char *path, void *buffer, size_t bufbytes)
{
    if (!path || !buffer || bufbytes < 1)
        return IOERR_INVALID_ARGS;
    return IOERR_NOT_YET_IMPLEMENTED;
}


int file_checksum(char *path)
{
    if (!path)
        return IOERR_INVALID_ARGS;
    return IOERR_NOT_YET_IMPLEMENTED;
}

int dir_checksum(char *path)
{
    if (!path)
        return IOERR_INVALID_ARGS;
    return IOERR_NOT_YET_IMPLEMENTED;
}
