//
//  main.c
//  Cache measurement
//
//  Created by Alexey Halaidzhy on 03.04.15.
//  Copyright (c) 2015 Alexey Halaidzhy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

#define TWICE(x) x x

struct tms tmsBegin, tmsEnd;

const size_t CYCLES_COUNT = 1000000;

const size_t MIN_POW = 10;
const size_t MAX_POW = 25;

const size_t MIN_ELEM_SIZE = 1;
const size_t MAX_ELEM_SIZE = 21;
const size_t ELEM_SIZE_DELTA = 4;

void time_start() {
    times(&tmsBegin);
}

size_t time_stop() {
    times(&tmsEnd);
    return ((tmsEnd.tms_utime - tmsBegin.tms_utime) + (tmsEnd.tms_stime - tmsBegin.tms_stime));
}

void build_conseq_list(void **list, size_t size, size_t elem_size) {
    size_t i;
    for (i = 0; i < size - 1; ++i) {
        list[i] = list + i + 1;
    }
    list[i] = list;
}

void build_random_list(void **list, size_t size, size_t elem_size) {
    srand((unsigned int) time(NULL));
    size_t prev_index = 0;
    for (size_t i = 0; i < size - 1; ++i) {
        size_t current_index;
        do {
            current_index = rand() % size;
        } while (list[current_index]);
        list[prev_index] = list + current_index;
        prev_index = current_index;
    }
    list[prev_index] = list;
}

size_t measure(void **list) {
    size_t avg;
    
    void **cur = list;
    time_start();
    for (int j = 0; j < CYCLES_COUNT; ++j) {
        TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE( cur = (void**)*cur; ))))))))
    }
    size_t t = time_stop();
    avg = t / CYCLES_COUNT;
    
    return avg;
}

int main(int argc, char *argv[]) {
    for (size_t i = MIN_POW; i < MAX_POW; ++i) {
        size_t list_size = 1 << i;
        for (size_t elem_size = MIN_ELEM_SIZE; elem_size < MAX_ELEM_SIZE; elem_size += ELEM_SIZE_DELTA) {
            printf("=================="
                   "NEW EXPERIMENT"
                   "Current pow: %lu\n"
                   "Current elem_size: %lu\n", i, elem_size);
            
            printf("Sequential memory access\n");
            void **list = (void **)calloc(list_size, elem_size);
            printf("List generation started\n");
            build_conseq_list(list, list_size, elem_size);
            printf("List generation finished\n");
            printf("Average process ticks count: %lu\n", measure(list));
            free(list);

            printf("Random memory access\n");
            list = (void **)calloc(list_size, elem_size);
            printf("List generation started\n");
            build_random_list(list, list_size, elem_size);
            printf("List generation finished\n");
            printf("Average process ticks count: %lu\n", measure(list));
            free(list);
        }
        
    }
    return 0;
}