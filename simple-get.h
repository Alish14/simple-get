#ifndef SIMPLE_GET_H
#define SIMPLE_GET_H
#include <stdio.h>
#include <time.h>

typedef struct {
    char url[100];
    struct tm datetime;
} Entry;

int read_config(char *file_name, Entry entries[], int *numEntries);
int download_file(char url[], char destination[]);
int compare_entries(const void *a, const void *b);
size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream);

#endif
