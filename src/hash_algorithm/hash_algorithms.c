//
// Created by sriram on 11/7/24.
//

#include "hash.h"

unsigned long djb(char unsigned *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = (hash << 5) + hash + c;

    return hash;
}

unsigned long basic(char unsigned *str) {
    unsigned long hash = 0;
    int c;
    while ((c = *str++))
        hash += c;

    return hash;
}
