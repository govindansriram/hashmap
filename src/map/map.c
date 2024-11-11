//
// Created by sriram on 11/7/24.
//

#include "map.h"
#include "hash.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm-generic/errno-base.h>

static int map_resize(MAP_T *pMap);

struct item {
    void *pValue;
    char *key;
    struct item *pNext;
};

struct map {
    int capacity;
    int len;
    struct item *pBuckets;
    unsigned long (*pHash_function)(char unsigned *);
};

int map_capacity(MAP_T *pMap) {
    return pMap->capacity;
}

int map_len(MAP_T *pMap) {
    return pMap->len;
}

MAP_T *new_standard_map(const int capacity) {
    return new_custom_map(capacity, &djb);
}

/**
 * creates a copy of the data on the heap
 *
 * @param source a data pointer
 * @param size the size of all the data belonging to the pointer
 * @return a new memory pointer on the heap
 */
void *heap_copy(void *source, size_t size) {
    void *dest = malloc(size);
    if (dest == NULL) {
        return NULL;
    }

    // Copy the contents from src to dest
    memcpy(dest, source, size);
    return dest;
}

/**
 * DON'T use this for production purposes. This function should only be used to make
 * MAPS with bad hash functions, in order to test properties like collisions
 *
 * @param capacity the initial max amount of objects the map can hold
 * @param pHash_function the function used to generate index hashes
 * @return a new map with a custom hash function if successful othewise NULL
 */
MAP_T *new_custom_map(const int capacity,  unsigned long (*pHash_function)(char unsigned *)) {
    MAP_T *map = malloc(sizeof(MAP_T));

    if (map == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    map->pBuckets = calloc(capacity, sizeof(struct item));

    if (map->pBuckets == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    map->pHash_function = pHash_function;
    map->capacity = capacity;
    map->len = 0;
    return map;
}

/**
 * updates or inserts a new key, value pair into the hashmap;
 * the value is updated if the key is already present in the map;
 * otherwise the key and value are inserted
 *
 * @param pMap the map being upserted into
 * @param key the key of the data
 * @param pValue the data
 * @return 0 if the upsert was successful
 */
static int upsert(MAP_T *pMap, char *key, void *pValue) {

    unsigned long hash = (*pMap->pHash_function)((unsigned char *)key);

    hash %= pMap->capacity; //index

    struct item *pBucket = pMap->pBuckets + hash;

    if (pBucket->key == NULL) { // insert case
        pBucket->key = key;
        pBucket->pValue = pValue;

        pMap->len++;

        return 0;
    }

    if (strcmp(pBucket->key, key) == 0) { // update case
        free(pBucket->pValue);
        free(key); // free the second hash allocated key
        pBucket->pValue = pValue;
        return 0;
    }

    // collision case

    struct item *pNext_item = malloc(sizeof(struct item));

    if (pNext_item == NULL) {
        errno = ENOMEM;
        return -1;
    }

    pNext_item->key = key;
    pNext_item->pValue = pValue;
    pNext_item->pNext = NULL;

    while(pBucket->pNext)
        pBucket = pBucket->pNext;

    pBucket->pNext = pNext_item;
    pMap->len++;

    return 0;
}

/**
 * updates or inserts a new key, value pair into the hashmap;
 * the value is updated if the key is already present in the map;
 * otherwise the key and value are inserted. The hashmap is also resized if
 * the capacity exceeds 75%
 *
 * @param pMap the map being upserted into
 * @param key the key of the data
 * @param pValue the data
 * @param size the size of the value being stored
 * @return 0 if the upsert was successful
 */
int map_upsert(MAP_T *pMap, char *key, void *pValue, size_t size) {

    if (pMap->len / (double) pMap->capacity >= 0.75) {
        // do resizing
        if (map_resize(pMap) == -1) {
            return -1;
        }
    }

    key = heap_copy(key, (strlen(key) + 1) * sizeof(char)); //ensure the key is on the heap
    if (key == NULL) {
        errno = ENOMEM;
        return -1;
    }

    pValue = heap_copy(pValue, size); //ensure the value is on the heap
    if (pValue == NULL) {
        errno = ENOMEM;
        return -1;
    }

    return upsert(pMap, key, pValue);
}

/**
 * doubles the capacity of the hashmap
 * @param pMap
 * @return 0 if the capacity was doubled
 */
static int map_resize(MAP_T *pMap) {
    const int old_length = pMap->capacity;
    struct item *pOldBuckets = pMap->pBuckets;

    pMap->capacity = old_length * 2;
    pMap->len = 0;
    pMap->pBuckets = calloc(pMap->capacity, sizeof(struct item));

    if (pMap->pBuckets == NULL) {
        errno = ENOMEM;
        return -1;
    }

    for (int i = 0; i < old_length; i++) {
        struct item *bucket = pOldBuckets + i;

        int entered = 0;

        while(bucket->pNext) {
            struct item *next = bucket->pNext;
            upsert(pMap, bucket->key, bucket->pValue);

            if (entered > 0) // only free links in the bucket not the original array element
                free(bucket);

            bucket = next;
            entered++;
        }

        if (bucket->key != NULL) // upsert last element in linked list if it exists
            upsert(pMap, bucket->key, bucket->pValue);

        if (entered > 0) // only free links in the bucket not the original array element
            free(bucket);
    }

    free(pOldBuckets); // frees the allocated bucket array

    return 0;
}

/**
 * gets the value for the key in the map if it exists
 * @param pMap the map with the data
 * @param key the key whose data you wish to extract
 * @return a void * to the value if the key is present otherwise NULL
 */
void *map_get(MAP_T *pMap, char *key) {
    unsigned long hash = (*pMap->pHash_function)((unsigned char *)key);
    hash %= pMap->capacity; //index

    struct item *bucket = pMap->pBuckets + hash; // bucket linked list

    if (bucket->key == NULL) { // bucket is entirely empty
        return NULL;
    }

    while(strcmp(bucket->key, key) != 0) {
        bucket = bucket->pNext;

        if (!bucket)
            return NULL;
    }

    return bucket->pValue;
}

/**
 * deletes the key and value from the map if it exists
 * @param pMap the map with your key
 * @param key the key you wish to remove
 * @return 0 if the key is present
 */
int map_delete(MAP_T *pMap, char *key) {
    unsigned long hash = (*pMap->pHash_function)((unsigned char *)key);
    hash %= pMap->capacity; //index

    struct item *bucket = pMap->pBuckets + hash; // bucket linked list

    if (!bucket->key) { // index holds no data
        return -1;
    }

    if (strcmp(bucket->key, key) == 0) { // removing the first element
        free(bucket->key);
        bucket->key = NULL;
        free(bucket->pValue);
        bucket->pValue = NULL;

        if (bucket->pNext != NULL) { // has > 1 elements
            struct item *old_next = bucket->pNext;
            bucket->key = old_next->key;
            bucket->pValue = old_next->pValue;
            bucket->pNext = old_next->pNext;
            free(old_next);
        }

        pMap->len--;
        return 0;
    }

    struct item *prev = bucket;
    bucket = bucket->pNext;

    if (bucket == NULL)
        return -1;

    while(strcmp(bucket->key, key) != 0) {
        prev = bucket;

        if (!bucket->pNext) { // cannot find item
            return -1;
        }

        bucket = bucket->pNext;
    }

    struct item *next = bucket->pNext;

    free(bucket->key);
    free(bucket->pValue);
    free(bucket);

    if (!next) { // item is the last element in the linked list
        bucket = NULL;
        prev->pNext = NULL;
        pMap->len--;
        return 0;
    }

    // element is nested

    prev->pNext = next;
    pMap->len--;
    return 0;
}

/**
 * frees the map from memory
 * @param pMap the map you wish to free
 */
void map_free(MAP_T *pMap) {

    struct item *buckets = pMap->pBuckets;

    for (int i = 0; i < pMap->capacity; i++) {
        struct item *bucket = buckets + i;

        // free first element properties in linked list but not the item since its part of the array
        free(bucket->pValue);
        free(bucket->key);
        bucket = bucket->pNext;

        while(bucket) { // free proceeding elements and properties
            free(bucket->key);
            free(bucket->pValue);
            struct item *next = bucket->pNext;
            free(bucket);
            bucket = next;
        }
    }

    free(buckets);
    free(pMap);
}
