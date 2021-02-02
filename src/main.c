#include"common.h"
#include"chunk.h"
#include"debug.h"

int main(int argc, char const *argv[])
{
    Chunk chunk;
    initChunk(&chunk);

    int index = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 100);
    writeChunk(&chunk, index, 100);
    
    writeChunk(&chunk, OP_RETURN, 101);

    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    return 0;
}
