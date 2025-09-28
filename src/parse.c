#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include "common.h"
#include "parse.h"

int create_db_header(struct dbheader_t **headerOut) {
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("failed to allocate memory for dbheader_t\n");
        return STATUS_ERROR;
    }

    header->magic = HEADER_MAGIC;
    header->version = HEADER_VERSION;
    header->count = 0;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;
    return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if (fd < 0) {
        printf("got bad fd \n");
        return STATUS_ERROR;
    }

    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("failed to allocate memory for dbheader_t\n");
        return STATUS_ERROR;
    }

    lseek(fd, 0, SEEK_SET);

    if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("read");
        free(header);
        return STATUS_ERROR;
    };

    header->magic = ntohl(header->magic);
    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->filesize = ntohl(header->filesize);

    if (header->magic != HEADER_MAGIC) {
        printf("improper db header magic\n");
        free(header);
        return STATUS_ERROR;
    }

    if (header->version != HEADER_VERSION) {
        printf("improper db header version\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat db_stat = {0};
    fstat(fd, &db_stat);
    if (header->filesize != db_stat.st_size) {
        printf("file size mismatch, database could be corrupt\n");
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;
    return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *header, struct employee_t **employeesOut) {
    if (fd < 0) {
        printf("got bad fd \n");
        return STATUS_ERROR;
    }

    if (header == NULL) {
        printf("uiser did not provide correct blagf");
        return STATUS_ERROR;
    }

    header->magic = htonl(header->magic);
    header->version = htons(header->version);
    header->count = htons(header->count);
    header->filesize = htonl(header->filesize);

    lseek(fd, 0, SEEK_SET);

    if (write(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("write");
        printf("failed to write dbheader_t\n");
        return STATUS_ERROR;
    } ;

    return STATUS_SUCCESS;
}
