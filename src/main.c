#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"common.h"
#include"chunk.h"
#include"debug.h"
#include"vm.h"

static void repl() {
    char line[1024];
    for (;;) {
        printf(">> ");

        if(!fgets(line,sizeof(line),stdin)) {
            printf("\n");
            break;
        }

        interpret(line);
    }
}

static char* readFile(const char* path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END); // 将指针移至文件末尾
    size_t fileSize = ftell(file); // 返回当前文件位置
    rewind(file); // 设置文件位置为开头

    char *buffer = (char *)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read File \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char* path) {
    char *source = readFile(path);
    InterpretResult result = interpret(source);
    free(source);

    if(result==INTERPRET_COMPILE_EEROR)
        exit(65);
    if(result==INTERPRET_RUNTIME_EEROR)
        exit(70);
}

int main(int argc, char const *argv[])
{
    initVM();

    if (argc==1) {
        repl();
    } else if(argc==2) {
        runFile(argv[1]);
    } else {
        fprintf(stderr, "Usage: run [path]\n");
        exit(64);
    }

    freeVM();
    return 0;
}
