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

int create_db_header(int fd, dbheader_t **headerOut) {
    dbheader_t *header = calloc(1, sizeof(dbheader_t));
    if (header == NULL) {
        printf("failed to allocate memory for dbheader_t\n");
        return STATUS_ERROR;
    }

    header->magic = HEADER_MAGIC;
    header->version = HEADER_VERSION;
    header->count = 0;
    header->filesize = sizeof(dbheader_t);

    *headerOut = header;

    dbheader_t header_net;
    header_net.magic = htonl(header->magic);
    header_net.version = htons(header->version);
    header_net.count = htons(header->count);
    header_net.filesize = htonl(header->filesize);

    if (write(fd, &header_net, sizeof(header_net)) != sizeof(header_net)) {
        perror("write");
        printf("failed to write dbheader_t\n");
        return STATUS_ERROR;
    } ;

    return STATUS_SUCCESS;
}

int validate_db_header(int fd, dbheader_t **headerOut) {
    if (fd < 0) {
        printf("got bad fd for header validation\n");
        return STATUS_ERROR;
    }

    dbheader_t *header = calloc(1, sizeof(dbheader_t));
    if (header == NULL) {
        printf("failed to allocate memory for dbheader_t\n");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(dbheader_t)) != sizeof(dbheader_t)) {
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