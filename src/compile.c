#include<stdio.h>
#include<stdlib.h>

#include"common.h"
#include"chunk.h"
#include"compile.h"
#include"scanner.h"

typedef struct {
    Token current;
    Token previous;
    bool hadError; // 无法识别的token
    bool panicMode; // 恐慌模式
} Parser;

typedef enum
{
} Precedence;

Parser parser;

Chunk *compilingChunk;

static Chunk* currentChunk() {
    return compilingChunk;
}

static errorAt(Token *token, const char* message) {
    if (parser.panicMode)
        return;
    parser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_EEROR) {
        // pass
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_EEROR)
            break;

        errorAt(&parser.current, parser.current.start);
    }
}

static void consume(TokenType type, const char* message) {
    if(parser.current.type == type) {
        advance();
        return;
    }

    errorAt(&parser.current, message);
}

static void emitByte(uint8_t byte) {
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn()
{
    emitByte(RETURN);
}

static void endCompiler() {
    emitReturn();
}

static void grouping() {
    expression();
    consume(RIGHT_PAREN, "Expect ')' after expression");
}

static uint8_t makeConstant(Value value) {
    int constant = addConstant(currentChunk(), value);
    if(constant > UINT8_MAX) {
        errorAt(&parser.previous.start, "Constant too large");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitConstant(Value value) {
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static void number() {
    double value = strtod(parser.previous.start, NULL);
    emitConstant(value);
}

static void unary() {
    TokenType operatorType = parser.previous.type;

    expression(); // 操作数

    switch (operatorType) {
        case MINUS:
            emitByte(OP_NEGATE);
            break;
        default:
            return;
    }
}

static void parsePrecedence() {}

static void expression() {}

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expected end of expression");
    endCompiler();
    return !parser.hadError;
}