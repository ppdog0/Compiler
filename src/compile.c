#include<stdio.h>
#include<stdlib.h>

#include"common.h"
#include"chunk.h"
#include"compile.h"
#include"scanner.h"
#include"debug.h"

typedef struct {
    Token current;
    Token previous;
    bool hadError; // 无法识别的token
    bool panicMode; // 恐慌模式
} Parser;

typedef enum { // 优先级由低到高
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

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

    if (token->type == EOF) {
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
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), "code");
    }
#endif
}

static void expression();
static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static void binary() {
    TokenType operatorType = parser.previous.type;

    // 操作数
    ParseRule *rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch(operatorType) {
        case PLUS:  emitByte(OP_ADD);
            break;
        case MINUS: emitByte(OP_SUBTRACT);
            break;
        case STAR:  emitByte(OP_MULTIPLY);
            break;
        case SLASH: emitByte(OP_DIVIDE);
            break;
        default:
            return;
    }
}

static void grouping() {
    expression();
    consume(RIGHT_PAREN, "Expect ')' after expression");
}

static uint8_t makeConstant(Value value) {
    int constant = addConstant(currentChunk(), value);
    if(constant > UINT8_MAX) {
        errorAt(&parser.previous, "Constant too large");
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

    // expression(); // 操作数
    parsePrecedence(PREC_UNARY);

    switch (operatorType) {
        case MINUS:
            emitByte(OP_NEGATE);
            break;
        default:
            return;
    }
}

ParseRule rules[] = {
    /* Compiling Expressions rules < Calls and Functions infix-left-paren
    [LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    * /
        //> Calls and Functions infix-left-paren
        [LEFT_PAREN] = {grouping, call, PREC_CALL},
    //< Calls and Functions infix-left-paren
    [RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [LEFT_BRACE] = {NULL, NULL, PREC_NONE}, // [big]
    [RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [COMMA] = {NULL, NULL, PREC_NONE},
    /* Compiling Expressions rules < Classes and Instances table-dot
  [DOT]           = {NULL,     NULL,   PREC_NONE},
*/
    //> Classes and Instances table-dot
    [DOT] = {NULL, dot, PREC_CALL},
    //< Classes and Instances table-dot
    [MINUS] = {unary, binary, PREC_TERM},
    [PLUS] = {NULL, binary, PREC_TERM},
    [SEMICOLON] = {NULL, NULL, PREC_NONE},
    [SLASH] = {NULL, binary, PREC_FACTOR},
    [STAR] = {NULL, binary, PREC_FACTOR},
    /* Compiling Expressions rules < Types of Values table-not
  [BANG]          = {NULL,     NULL,   PREC_NONE},
*/
    //> Types of Values table-not
    [BANG] = {unary, NULL, PREC_NONE},
    //< Types of Values table-not
    /* Compiling Expressions rules < Types of Values table-equal
  [BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
*/
    //> Types of Values table-equal
    [BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    //< Types of Values table-equal
    [EQUAL] = {NULL, NULL, PREC_NONE},
    /* Compiling Expressions rules < Types of Values table-comparisons
  [EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
  [GREATER]       = {NULL,     NULL,   PREC_NONE},
  [GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [LESS]          = {NULL,     NULL,   PREC_NONE},
  [LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
*/
    //> Types of Values table-comparisons
    [EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [GREATER] = {NULL, binary, PREC_COMPARISON},
    [GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [LESS] = {NULL, binary, PREC_COMPARISON},
    [LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    //< Types of Values table-comparisons
    /* Compiling Expressions rules < Global Variables table-identifier
  [IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
*/
    //> Global Variables table-identifier
    [IDENTIFIER] = {variable, NULL, PREC_NONE},
    //< Global Variables table-identifier
    /* Compiling Expressions rules < Strings table-string
  [STRING]        = {NULL,     NULL,   PREC_NONE},
*/
    //> Strings table-string
    [STRING] = {string, NULL, PREC_NONE},
    //< Strings table-string
    [NUMBER] = {number, NULL, PREC_NONE},
    /* Compiling Expressions rules < Jumping Back and Forth table-and
  [AND]           = {NULL,     NULL,   PREC_NONE},
*/
    //> Jumping Back and Forth table-and
    [AND] = {NULL, and_, PREC_AND},
    //< Jumping Back and Forth table-and
    [CLASS] = {NULL, NULL, PREC_NONE},
    [ELSE] = {NULL, NULL, PREC_NONE},
    /* Compiling Expressions rules < Types of Values table-false
  [FALSE]         = {NULL,     NULL,   PREC_NONE},
*/
    //> Types of Values table-false
    [FALSE] = {literal, NULL, PREC_NONE},
    //< Types of Values table-false
    [FOR] = {NULL, NULL, PREC_NONE},
    [FUN] = {NULL, NULL, PREC_NONE},
    [IF] = {NULL, NULL, PREC_NONE},
    /* Compiling Expressions rules < Types of Values table-nil
  [NIL]           = {NULL,     NULL,   PREC_NONE},
*/
    //> Types of Values table-nil
    [NIL] = {literal, NULL, PREC_NONE},
    //< Types of Values table-nil
    /* Compiling Expressions rules < Jumping Back and Forth table-or
  [OR]            = {NULL,     NULL,   PREC_NONE},
*/
    //> Jumping Back and Forth table-or
    [OR] = {NULL, or_, PREC_OR},
    //< Jumping Back and Forth table-or
    [PRINT] = {NULL, NULL, PREC_NONE},
    [RETURN] = {NULL, NULL, PREC_NONE},
    /* Compiling Expressions rules < Superclasses table-super
  [SUPER]         = {NULL,     NULL,   PREC_NONE},
*/
    //> Superclasses table-super
    [SUPER] = {super_, NULL, PREC_NONE},
    //< Superclasses table-super
    /* Compiling Expressions rules < Methods and Initializers table-this
  [THIS]          = {NULL,     NULL,   PREC_NONE},
*/
    //> Methods and Initializers table-this
    [THIS] = {this_, NULL, PREC_NONE},
    //< Methods and Initializers table-this
    /* Compiling Expressions rules < Types of Values table-true
  [TRUE]          = {NULL,     NULL,   PREC_NONE},
*/
    //> Types of Values table-true
    [TRUE] = {literal, NULL, PREC_NONE},
    //< Types of Values table-true
    [VAR] = {NULL, NULL, PREC_NONE},
    [WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_EEROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static void parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if(prefixRule == NULL) {
        errorAt(&parser.previous, "Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

static ParseRule* getRule(TokenType type) {
    return &rules[type];
}

static void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    consume(EOF, "Expected end of expression");
    endCompiler();
    return !parser.hadError;
}