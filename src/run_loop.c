/*
 *  run_loop.c
 *
 *  Created by Matthias Ringwald on 6/6/09.
 */

#include <btstack/run_loop.h>

#include <stdio.h>
#include <stdlib.h>  // exit()


#include "../config.h"

static run_loop_t * the_run_loop = NULL;

extern run_loop_t run_loop_posix;

#ifdef USE_COCOA_RUN_LOOP
extern run_loop_t run_loop_cocoa;
#endif

// assert run loop initialized
void run_loop_assert(){
    if (!the_run_loop){
        fprintf(stderr, "ERROR: run_loop function called before run_loop_init!\n");
        exit(10);
    }
}

/**
 * Add data_source to run_loop
 */
void run_loop_add_data_source(data_source_t *ds){
    run_loop_assert();
    the_run_loop->add_data_source(ds);
}

/**
 * Remove data_source from run loop
 */
int run_loop_remove_data_source(data_source_t *ds){
    run_loop_assert();
    return the_run_loop->remove_data_source(ds);
}

/**
 * Add timer to run_loop (keep list sorted)
 */
void run_loop_add_timer(timer_t *ts){
    run_loop_assert();
    the_run_loop->add_timer(ts);
}

/**
 * Remove timer from run loop
 */
int run_loop_remove_timer(timer_t *ts){
    run_loop_assert();
    return the_run_loop->remove_timer(ts);
}

void run_loop_timer_dump(){
    run_loop_assert();
    return the_run_loop->dump_timer();
}

/**
 * Execute run_loop
 */
void run_loop_execute() {
    run_loop_assert();
    the_run_loop->execute();
}

// init must be called before any other run_loop call
void run_loop_init(RUN_LOOP_TYPE type){
    if (the_run_loop){
        fprintf(stderr, "ERROR: run loop initialized twice!\n");
        exit(10);
    }
    switch (type) {
        case RUN_LOOP_POSIX:
            the_run_loop = &run_loop_posix;
            break;
#ifdef USE_COCOA_RUN_LOOP
        case RUN_LOOP_COCOA:
            the_run_loop = &run_loop_cocoa;
            break;
#endif
        default:
            fprintf(stderr, "ERROR: invalid run loop type %u selected!\n", type);
            exit(10);
            break;
    }
    the_run_loop->init();
}

// set timer
void run_loop_set_timer(timer_t *a, int timeout_in_ms){
    gettimeofday(&a->timeout, NULL);
    a->timeout.tv_sec  += timeout_in_ms / 1000;
    a->timeout.tv_usec += (timeout_in_ms % 1000) * 1000;
    if (a->timeout.tv_usec > 1000000) {
        a->timeout.tv_usec -= 1000000;
        a->timeout.tv_sec++;
    }
}

// compare timers - NULL is assumed to be before the Big Bang
// pre: 0 <= tv_usec < 1000000
int run_loop_timeval_compare(struct timeval *a, struct timeval *b){
    if (!a || !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    
    if (a->tv_sec < b->tv_sec) {
        return -1;
    }
    if (a->tv_sec > b->tv_sec) {
        return 1;
    }
    
    if (a->tv_usec < b->tv_usec) {
        return -1;
    }
    if (a->tv_usec > b->tv_usec) {
        return 1;
    }
    
    return 0;
    
}

// compare timers - NULL is assumed to be before the Big Bang
// pre: 0 <= tv_usec < 1000000
int run_loop_timer_compare(timer_t *a, timer_t *b){
    if (!a || !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    return run_loop_timeval_compare(&a->timeout, &b->timeout);
}

