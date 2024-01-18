#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <string.h>
#include "simple-get.h"

int read_config(char *file_name, Entry entries[], int *numEntries);
int download_file(char url[], char destination[]);
int compare_entries(const void *a, const void *b);

size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream);

int main(int argc, char *argv[]) {
    int numEntries = 0;
    Entry entries[100];
    char *config_file = NULL;
    int switch_conf=0;
    char *destination = NULL;
    char *param_b = NULL;

    int opt;

    while ((opt = getopt(argc, argv, ":if:c:o:thy")) != -1) {
        switch (opt) {
            case 'h':
                  puts("-------------------------------------------------------------------------------------- ");
                  puts("| you can download file specefic time                                                 |");
                  puts("| usage simple-get -c config.txt -o /Users/alish/Documents/C_exer/downloaded_file.txt |");
                  puts("| confige file format:");
                  puts("| https://www.google.com   2025/01/15  12:30:45                                        ");
                  puts("--------------------------------------------------------------------------------------\n");

                break;
            case 'c':
                config_file = optarg;
                printf("%s\n",config_file);
                switch_conf=1;

                break;
            case 'o':
                 destination= optarg;
                break;
            case '?':
                exit(EXIT_FAILURE);
        }
    }
    
    if (switch_conf==0 && config_file == NULL) {
        fprintf(stderr, "Configuration file is not provided.\n");
        exit(EXIT_FAILURE);
    }
    if (destination==NULL){
        printf("you should specify destination ...\n");
        exit(EXIT_FAILURE);
        }
    read_config(config_file, entries, &numEntries);

    qsort(entries, numEntries, sizeof(Entry), compare_entries);

    for (int i = 0; i < numEntries; i++) {

        printf("Downloading %s with delay %ld seconds...\n", entries[i].url,
               mktime(&entries[i].datetime) - time(NULL));
               printf("url is :%s   destination is:%s \n",entries[i].url,destination);

               
               do{
                                 printf("waiting for the time\n");
                sleep(5);

               }while (mktime(&entries[i].datetime) - time(NULL)> -1);
               
        download_file(entries[i].url, destination);
    }

    return 0;
}

size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int read_config(char *file_name, Entry entries[], int *numEntries) {
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        perror("Error opening configuration file");
        exit(EXIT_FAILURE);
    }

    char line[120];
    
    while (fgets(line, 120, fp)) {
        char *token = strtok(line, " ");
        strcpy(entries[*numEntries].url, token);

        token = strtok(NULL, "\"");
        sscanf(token, "%d/%d/%d %d:%d:%d",
               &entries[*numEntries].datetime.tm_year,
               &entries[*numEntries].datetime.tm_mon,
               &entries[*numEntries].datetime.tm_mday,
               &entries[*numEntries].datetime.tm_hour,
               &entries[*numEntries].datetime.tm_min,
               &entries[*numEntries].datetime.tm_sec);

        entries[*numEntries].datetime.tm_year -= 1900;
        (*numEntries)++;
    }

    fclose(fp);

    return 0;
}

int compare_entries(const void *a, const void *b) {
    time_t time_a = mktime(&((Entry *)a)->datetime);
    time_t time_b = mktime(&((Entry *)b)->datetime);

    return difftime(time_a, time_b);
}

int download_file(char url[], char destination[]) {
    CURL *curl;
    FILE *fp;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        fp = fopen(destination, "wb");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
        fclose(fp);
    }

    curl_global_cleanup();

    return 0;
}
