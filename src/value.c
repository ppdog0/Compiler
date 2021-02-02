#include"value.h"
#include"memory.h"

void initValueArray(Array *array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void writeValueArray(Array *array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values,
                                 oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(Array *array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value) {
    printf("%g", value);
}