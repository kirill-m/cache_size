//
//  main.c
//  cache_size
//
//  Created by Kirill Matveev on 31.03.16.
//  Copyright © 2016 Kirill. All rights reserved.
//

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "limits.h"

#define BOTTOM_SIZE 100
#define TOP_SIZE 70000000
#define PAYLOAD_SIZE 3
#define STEPS 5000000
#define CYCLES_NUMBER 20

struct Node {
    struct Node *next;
    char payload[PAYLOAD_SIZE];
    //int visited;
};

void getCpuInfo(FILE* fp);

int main() {
    printf("Processing...\n");
    
    time_t current_time;
    time(&current_time);
    char* c_time_string;
    c_time_string = ctime(&current_time);
    
    FILE *fp;
    fp=fopen("results.txt", "w");
    fprintf(fp, "%s", c_time_string);
    fprintf(fp, "CACHE SIZE TEST\n");
    
    getCpuInfo(fp);
    printf("See results in results.txt.\n");
    fclose(fp);
    
    return 0;
}


void getCpuInfo(FILE* fp) {
    FILE *pp;
#ifdef __linux__
    pp = popen("lscpu | grep cache", "r");
#elif __APPLE__
    pp = popen("sysctl -a | grep cachesize", "r");
#else
    return;
#endif
    
    fprintf(fp, "\nSystem info...\n");
    
    if (pp != NULL) {
        while (1) {
            char *line;
            char buf[1024];
            line = fgets(buf, sizeof buf, pp);
            if (line == NULL) break;
            fprintf(fp, "%s", line);
        }
        fprintf(fp, "\n");
        pclose(pp);
    }
}

