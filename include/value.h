#ifndef VALUE_H
#define VALUE_H

#include"common.h"

typedef double Value;

typedef struct {
    int capacity;
    int count;
    Value *values;
} Array;

void initValueArray(Array *array);
void writeValueArray(Array *array, Value value);
void freeValueArray(Array *array);
void printValue(Value value);

#endif