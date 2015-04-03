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

#define TWICE(x) x x

const size_t CYCLES_COUNT = 10000000;

const size_t MIN_POW = 10;
const size_t MAX_POW = 28;

const size_t MIN_ELEM_SIZE = 1000;
const size_t MAX_ELEM_SIZE = 40000;
const size_t ELEM_SIZE_DELTA = 1000;

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

void build_conseq_list(void **list, size_t size) {
    size_t i;
    for (i = 0; i < size - 1; ++i) {
        list[i] = list + i + 1;
    }
    list[i] = list;
}

void build_random_list(void **list, size_t size) {
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

double measure(void **list) {
    double avg;
    
    void **cur = list;
    uint64_t start = rdtsc();
    for (size_t j = 0; j < CYCLES_COUNT; ++j) {
        TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE( cur = (void**)*cur; ))))))))
    }
    uint64_t stop = rdtsc();
    avg = (stop - start) * 1.0 / CYCLES_COUNT;
    
    return avg;
}

int main(int argc, char *argv[]) {
    printf("Sequential access\n");
    for (size_t elem_size = MIN_ELEM_SIZE; elem_size <= MAX_ELEM_SIZE; elem_size += ELEM_SIZE_DELTA) {
        printf("ELEM_SIZE: %lu\n", elem_size);
        for (size_t i = MIN_POW; i <= MAX_POW; ++i) {
            size_t list_size = 1 << i;
            void **list = (void **)calloc(list_size, elem_size * sizeof(void *));
            build_conseq_list(list, list_size);
            printf("POW: %lu, TICKS: %f\n", i, measure(list));
            free(list);
        }
    }
    printf("Random access\n");
    for (size_t elem_size = MIN_ELEM_SIZE; elem_size <= MAX_ELEM_SIZE; elem_size += ELEM_SIZE_DELTA) {
        printf("ELEM_SIZE: %lu\n", elem_size);
        for (size_t i = MIN_POW; i <= MAX_POW; ++i) {
            size_t list_size = 1 << i;
            void **list = (void **)calloc(list_size, elem_size * sizeof(void *));
            build_random_list(list, list_size);
            printf("POW: %lu, TICKS: %f\n", i, measure(list));
            free(list);
        }
    }
    return 0;
}