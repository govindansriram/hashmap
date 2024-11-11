//
// Created by sriram on 11/7/24.
//

#ifndef MAP_H
#define MAP_H
#include <stddef.h>

typedef struct map MAP_T;

struct pair {
    void *pValue;
    char *key;
};

typedef struct pairs {
    struct pair *pPairs;
    int len;
} PAIRS_T;
MAP_T *new_map(int capacity);
int map_upsert(MAP_T *pMap, char *key, void *pValue, size_t size);
MAP_T *new_custom_map(int capacity,  unsigned long (*pHash_function)(char unsigned *));
void *map_get(MAP_T *pMap, char *key);
int map_delete(MAP_T *pMap, char *key);
void map_free(MAP_T *pMap);
int map_capacity(MAP_T *pMap);
int map_len(MAP_T *pMap);
PAIRS_T *map_pairs(MAP_T *map);
void pairs_free(PAIRS_T *pairs);
#endif //MAP_H
