#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

typedef struct {
    char *employee_string;
    char *filepath;
    bool newfile;
    bool help;

} Options;

Options parse_args(int argc, char *argv[]) {
    Options opts = {0};  // init default all
    int c;

    while ((c = getopt(argc, argv, "hnf:a:")) != -1) {
        switch (c) {
            case 'n': opts.newfile = true; break;
            case 'f': opts.filepath = optarg; break;
            case 'h': opts.help = true; break;
            case 'a': opts.employee_string = optarg; break;
            default:
                printf("usage: %s [-n] [-f filepath]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    return opts;
}

int main(int argc, char *argv[]) {
    int db_file = 0;
    struct dbheader_t *db_header = NULL;
    struct employee_t *employees = NULL;

    Options opts = parse_args(argc, argv);

    if (opts.filepath == NULL) {
        printf("file path is required\n");
        exit(EXIT_FAILURE);
    }

    if (opts.newfile) {
        db_file = create_db_file(opts.filepath);
        if (db_file == STATUS_ERROR) {
            printf("could not create db file\n");
            exit(EXIT_FAILURE);
        }
        if (create_db_header(&db_header) == STATUS_ERROR) {
            printf("could not create db header\n");
            exit(EXIT_FAILURE);
        };
    } else {
        db_file = open_db_file(opts.filepath);
        if (db_file == STATUS_ERROR) {
            printf("could not open db file\n");
            exit(EXIT_FAILURE);
        }
        if (validate_db_header(db_file, &db_header) == STATUS_ERROR) {
            printf("failed to validate db header data\n");
            exit(EXIT_FAILURE);
        };
    }

    if (read_employees(db_file, db_header, &employees)) { // reading overwrites our employee
        printf("failed to read employees\n");
        return 0;
    }

    if (opts.employee_string != NULL) {
        if (add_employee(db_header, &employees, opts.employee_string) == STATUS_ERROR) {
            printf("failed to parse and add employee string\n");
        }
    }

    printf("filepath: %s\n", opts.filepath);
    printf("new file: %u\n", opts.newfile);

    output_file(db_file, db_header, employees);

    free(db_header);
    free(employees);
    close(db_file);

    return 0;
}