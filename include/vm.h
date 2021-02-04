#ifndef VM_H
#define VM_H

#include"chunk.h"

#define STACK_MAX 256

typedef struct { // 虚拟机
    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *stackTop;
} VM;

typedef enum{
    INTERPRET_OK,
    INTERPRET_COMPILE_EEROR,
    INTERPRET_RUNTIME_EEROR
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(Chunk *chunk);
void push(Value Value);
Value pop();

#endif