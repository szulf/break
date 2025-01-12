#include "list_type.h"
#include "string_type.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct program_variables
{
    string_type project_name;
    string_type src_dir;
    list_type src_files;
    string_type header_dir;
    list_type header_files;
    string_type cc;
    string_type cxx;
    list_type c_flags;
    list_type cxx_flags;
} pv_type;

pv_type parse_file(const char* path)
{
    pv_type pv;

    FILE* fptr = fopen(path, "r");
    if (fptr == NULL)
    {
        fprintf(stderr, "Cannot find breakfile\n");
        exit(1);
    }

    fclose(fptr);
    return pv;
}

int main(int argc, char** argv)
{
    // struct stat stats;
    // stat("breakfile", &stats);
    // printf("%ld\n", stats.st_mtim.tv_sec);

    if (argc < 2)
    {
        printf("Not enough arguments\n");
        printf("Use 'break help' for help\n");
        return 1;
    }

    if (memcmp(argv[1], "help", strlen(argv[1])) == 0)
    {
        if (argc == 2)
        {
            printf("BREAK - a make inspired build tool for c\n\n");
            printf("Commands:\n");
            printf("init     start a new break project\n");
            printf("build    build all of the files\n");
        }
        else
        {
            if (memcmp(argv[2], "init", strlen(argv[2])) == 0)
            {
                printf("usage: break init project-name\n\n");
                // TODO
                // add some more here
                // like a little description
            }
        }
    }
    else if (memcmp(argv[1], "init", strlen(argv[1])) == 0)
    {
        if (argc != 3)
        {
            printf("Not enough arguments\n");
            printf("Use 'break help init' for help\n");
            return 1;
        }

        FILE* fptr = fopen("breakfile", "w");

        fprintf(fptr, "project_name := %s\n", argv[2]);
        fprintf(fptr, "source_directory := src\n");
        fprintf(fptr, "source_files := [main.c]\n\n");
        fprintf(fptr, "set configs = [\n");
        fprintf(fptr, "    {\"debug\", \"-g -O0 -Wall -Werror\"},\n");
        fprintf(fptr, "    {\"release\", \"-O3 -DNDEBUG\"},\n");
        fprintf(fptr, "]");

        fclose(fptr);
    }
    else if (memcmp(argv[1], "build", strlen(argv[1])) == 0)
    {
        // struct stat stats;
        // if (stat("build", &stats) == -1)
        // {
        //     mkdir("build", 0777);
        // }

        pv_type pv = parse_file("breakfile");
        // printf("%s\n", pv.project_name);
    }

    return 0;
}
