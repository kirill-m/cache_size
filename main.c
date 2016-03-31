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
};

void getCpuInfo(FILE* fp);
void iterate(FILE* fp, short is_random);
size_t getRandomNumber(size_t limit);

int main() {
    printf("Processing...\n");
    
    time_t current_time;
    time(&current_time);
    char* c_time_string;
    c_time_string = ctime(&current_time);
    
    FILE *fp;
    fp=fopen("results.txt", "w");
    fprintf(fp, "%s", c_time_string);
    fprintf(fp, "Cache size.\n");
    
    getCpuInfo(fp);
    
    fprintf(fp, "\nSequential iteration:\n");
    iterate(fp, 0);
    
    fprintf(fp, "\nRandom iteration:\n");
    iterate(fp, 1);
    
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
    
    fprintf(fp, "\nSystem info:\n");
    
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

void iterate(FILE* fp, short is_random) {
    
    double iteration_time = 0;
    
    size_t node_size = sizeof(struct Node);
    
    size_t elements_min = BOTTOM_SIZE / node_size;
    size_t elements_max = TOP_SIZE / node_size;
    
    for(size_t size = elements_min; size < elements_max; size *= 2) {
        struct Node *buff = malloc(size * sizeof(struct Node));
        
        for(size_t i = 0; i < size - 1; i++) {
            buff[i].next = &buff[i+1];
        }
        
        if (is_random) {
            for(size_t i = 0; i < size - 1; i++) {
                struct Node temp;
                size_t index = getRandomNumber(size - 2);
                temp = buff[i];
                buff[i] = buff[index];
                buff[index] = temp;
            }
        }
        
        buff[size - 1].next = &buff[0];
        
        clock_t start, end;
        
        for (size_t j = 0; j < CYCLES_NUMBER; j++) {
            start = clock();
            struct Node *node = buff;
            
            for (size_t i = 0; i < STEPS; i++) {
                node = node->next;
            }
            
            end = clock();
            iteration_time += (double)(end - start) / CLOCKS_PER_SEC;
        }
        
        iteration_time /= CYCLES_NUMBER;
        
        free(buff);
        
        fprintf(fp, "%zd", node_size*size);
        fprintf(fp, "\t%f\n", iteration_time);
    }
}


size_t getRandomNumber(size_t limit) {
    size_t divisor = RAND_MAX / (limit+1);
    size_t random_number;
    
    do {
        random_number = rand() / divisor;
    } while (random_number > limit);
    
    return random_number;
}


