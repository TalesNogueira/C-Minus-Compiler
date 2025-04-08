/*-------------------------------------------------------------------------------------------------/
 *  Utils functions for a C- Compiler
 *  File: utils.c
 *---------------------------------*/

#define _DEFAULT_SOURCE

#include "utils.h"
#include "globals.h"

#define MAX_FILES 100
#define MAX_NAME 256

void inputSelect(void) {
    char files[MAX_FILES][MAX_NAME];
    int count = 0;

    const char* folder = INPUT_DIR;
    DIR *dir = opendir(folder);
    if (!dir) {
        perror("> Error [ Misc ]:\n    Could not find the desired folder.\n");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    printf("> Files available at \"%s\":\n", folder);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            printf("[%d] %s\n", count, entry->d_name);
            strncpy(files[count], entry->d_name, MAX_NAME);
            count++;
        }
    }
    closedir(dir);

    if (count == 0) {
        printf("> Error [ Misc ]:\n    No files found.\n");
        exit(EXIT_FAILURE);
    }

    int choice;
    printf("> Enter the desired file number: ");
    scanf("%d", &choice);

    if (choice < 0 || choice >= count) {
        printf("> Error [ Misc ]:\n    Invalid choice.\n");
        exit(EXIT_FAILURE);
    }

    snprintf(source, sizeof(source), "%s/%s", folder, files[choice]);
}

void openInput(void) {
    yyin = fopen(source, "r");

    if (!yyin) {
        perror("> Error [ Misc ]:\n    Invalid file.\n");
        exit(EXIT_FAILURE);
    }
}