#include"common.h"
#include"chunk.h"
#include"debug.h"
#include"vm.h"

int main(int argc, char const *argv[])
{
    initVM();

    Chunk chunk;
    initChunk(&chunk);

    int index = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 100);
    writeChunk(&chunk, index, 100);

    index = addConstant(&chunk, 3.4);
    writeChunk(&chunk, OP_CONSTANT, 100);
    writeChunk(&chunk, index, 100);

    writeChunk(&chunk, OP_ADD, 100);

    index = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CONSTANT, 100);
    writeChunk(&chunk, index, 100);

    writeChunk(&chunk, OP_DIVIDE, 100);
    writeChunk(&chunk, OP_NEGATE, 101);

    writeChunk(&chunk, OP_RETURN, 102);

    disassembleChunk(&chunk, "test chunk");
    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);
    return 0;
}
