//
//  main.c
//  Cache measurement
//
//  Created by Alexey Halaidzhy on 03.04.15.
//  Copyright (c) 2015 Alexey Halaidzhy. All rights reserved.
//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TWICE(x) x x

const size_t CYCLES_COUNT = 1000000;

const size_t MIN_POW = 10;
const size_t MAX_POW = 28;

const size_t MIN_PADDING = 1;
const size_t MAX_PADDING = 40001;
const size_t DELTA_PADDING = 5000;

const size_t MIN_LIST_SIZE = 1000 / sizeof(void *);
const size_t MAX_LIST_SIZE = 5000000 / sizeof(void *);
const double LIST_SIZE_STEP = 1.15;

void build_conseq_list(void **list, size_t size, size_t padding) {
    size_t i;
    for (i = 0; i < size - padding; i += padding) {
        list[i] = list + i + padding;
    }
    list[i] = list;
}

void build_random_list(void **list, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        list[i] = NULL;
    }
    srand((unsigned int) time(NULL));
    size_t prev_index = 0;
    for (size_t i = 0; i < size - 1; ++i) {
        size_t current_index = rand() % size;
        while (list[current_index]) {
            current_index = (current_index + 1) % size;
        }
        list[prev_index] = list + current_index;
        prev_index = current_index;
    }
    list[prev_index] = list;
}

double measure(void **list) {
    double avg;
    
    void **cur = list;
    clock_t start = clock();
    for (size_t j = 0; j < CYCLES_COUNT; ++j) {
        TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE( cur = *cur; ))))))))
    }
    clock_t stop = clock();
    avg = (stop - start) * 1.0 / CYCLES_COUNT;
    
    return avg;
}

int main(int argc, char *argv[]) {
    if (argc == 2 && !strcmp(argv[1], "-r")) {
        printf("Random access\n");
        for (size_t list_size = MIN_LIST_SIZE; list_size <= MAX_LIST_SIZE; list_size *= LIST_SIZE_STEP) {                void **list = (void **)calloc(list_size, sizeof(void *));
            build_random_list(list, list_size);
            printf("LIST WAS SUCCESSFULLY BUILT\n");
            printf("SIZE: %lu, TICKS: %f\n", list_size * sizeof(void *), measure(list));
            free(list);
        }
    }
    else {
        printf("Sequential access\n");
        for (size_t padding = MIN_PADDING; padding <= MAX_PADDING; padding += DELTA_PADDING) {
            printf("PADDING: %lu\n", padding);
            for (size_t list_size = MIN_LIST_SIZE; list_size <= MAX_LIST_SIZE; list_size *= LIST_SIZE_STEP) {
                if (list_size > padding) {
                    void **list = (void **)calloc(list_size, sizeof(void *));
                    build_conseq_list(list, list_size, padding);
                    printf("LIST_SIZE: %lu, SIZE: %lu, TICKS: %f\n", list_size, list_size * sizeof(void *), measure(list));
                    free(list);
                }
            }
        }
    }
    return 0;
}