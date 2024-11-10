//
// Created by sriram on 11/7/24.
//

#ifndef MAP_H
#define MAP_H

typedef struct map MAP_T;
MAP_T *new_map(int capacity);
int map_upsert(MAP_T *pMap, char *key, void *pValue);
MAP_T *new_custom_map(int capacity,  unsigned long (*pHash_function)(char unsigned *));
void *map_get(const MAP_T *pMap, char *key);
int map_delete(MAP_T *pMap, char *key);
void map_free(MAP_T *pMap);
#endif //MAP_H
