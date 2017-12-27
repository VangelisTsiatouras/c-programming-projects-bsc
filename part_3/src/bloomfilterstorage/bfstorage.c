#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "bfstorage.h"

#include "../threadpool/threadpool.h" // included for tester!!

BFStorage *create_bf_storage(int numberOfBloomFilters) {
    BFStorage *bfStorage = malloc(sizeof(BFStorage));
    if (!bfStorage) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    bfStorage->bloomFilterArray = malloc(sizeof(BloomFilterArrayElement) * numberOfBloomFilters);
    if (!bfStorage->bloomFilterArray) {
        printf("malloc error %s\n", strerror(errno));
        exit(MALLOC_ERROR);
    }
    bfStorage->numberOfBloomFilters = numberOfBloomFilters;
    for (int i = 0; i < numberOfBloomFilters; i++) {
        bfStorage->bloomFilterArray[i].bloomFilter = create_bloom_filter();
        bfStorage->bloomFilterArray[i].bloomFilterObtained = 0;
        bfStorage->bloomFilterArray[i].bloomFilterID = i;
    }
    return bfStorage;
}

void destroy_bf_storage(BFStorage *bfStorage) {
    for (int i = 0; i < bfStorage->numberOfBloomFilters; i++) {
        destroy_bloom_filter(bfStorage->bloomFilterArray[i].bloomFilter);
    }
    free(bfStorage->bloomFilterArray);
    free(bfStorage);
}

BloomFilterArrayElement *obtain_filter_bf_storage(BFStorage *bfStorage) {
    pthread_mutex_lock(&storageMutex);
    BloomFilterArrayElement *bloomFilterArrayElement = NULL;
    for (int i = 0; i < bfStorage->numberOfBloomFilters; i++) {
        if (bfStorage->bloomFilterArray[i].bloomFilterObtained == 0) {
            bfStorage->bloomFilterArray[i].bloomFilterObtained = 1;
            bloomFilterArrayElement = &bfStorage->bloomFilterArray[i];
            break;
        }
    }
    pthread_mutex_unlock(&storageMutex);
    return bloomFilterArrayElement;
}

void release_filter_bf_storage(BFStorage *bfStorage, int position) {
    pthread_mutex_lock(&storageMutex);
    bfStorage->bloomFilterArray[position].bloomFilterObtained = 0;
    pthread_mutex_unlock(&storageMutex);
}

void *testerBFStorage() {
    BFStorage *bfStorage = create_bf_storage(NUMBER_OF_THREADS);
    JobScheduler *jobScheduler = create_scheduler(NUMBER_OF_THREADS);
    for (int i = 0; i < 10; i++) {
        Job *job = create_job(1);
        job->pointerToFunction = helloBloomFilter;
        job->args[0] = bfStorage;
        submit_scheduler(jobScheduler, job);
    }
    terminate_threads_scheduler(jobScheduler);
    destroy_scheduler(jobScheduler);
    destroy_bf_storage(bfStorage);
    exit(0);
}

void helloBloomFilter(BFStorage *bfStorage) {
    BloomFilterArrayElement *bloomFilterArrayElement = obtain_filter_bf_storage(bfStorage);
    BloomFilter *bloomFilter = bloomFilterArrayElement->bloomFilter;
    printf("i am thread %ld and i am using bloom filter %d\n", pthread_self(), bloomFilterArrayElement->bloomFilterID);
    fflush(stdout);
    usleep(100);
    release_filter_bf_storage(bfStorage, bloomFilterArrayElement->bloomFilterID);
//    print_bit_vector(bloomFilter);

}