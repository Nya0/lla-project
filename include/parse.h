#ifndef PARSE_H
#define PARSE_H

#define HEADER_VERSION 0x1
#define HEADER_MAGIC 0xdead8eee

typedef struct {
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
} dbheader_t;

typedef struct {
    char name[60];
    char address[256];
    unsigned short hours;
} employee_t;

int create_db_header(int fd, dbheader_t **headerOut);
int validate_db_header(int fd, dbheader_t **headerOut);


# endif
