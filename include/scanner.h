#ifndef SCANNER_H
#define SCANNER_H

typedef enum
{
    // \t \v \n \f \r \0 占位符 直接忽略
    // /**/ // 注释 直接忽略

    // 标点符号
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE, // () {}
    LEFT_BRACKET,
    RIGHT_BRACKET, // [] <>
    COMMA,
    DOT,
    SEMICOLON,
    QUESTION,
    COLON,
    HASH, // , . ; ? : #

    // 一元运算符和二元运算符
    PLUS,
    PLUS_PLUS,
    PLUS_EQUAL, // + ++ +=
    MINUS,
    MINUS_MINUS,
    MINUS_EQUAL,
    POINT, // - -- -= ->
    SLASH,
    SLASH_EQUAL, // / /=
    STAR,
    STAR_EQUAL, // * *=
    MOD,
    MOD_EQUAL, // % %=
    BANG,
    BANG_EQUAL, // ! !=
    EQUAL,
    EQUAL_EQUAL, // = ==
    GREATER,
    SHIFT_RIGHT,
    GREATER_EQUAL,
    SHIFT_RIGHT_EQUAL, // > >> >= >>=
    LESS,
    SHIFT_LEFT,
    LESS_EQUAL,
    SHIFT_LEFT_EQUAL, // < << <= <<=
    AND,
    AND_AND,
    AND_EQUAL, // & && &=
    OR,
    OR_OR,
    OR_EQUAL, // | || |=
    XOR,
    XOR_EQUAL, // ^ ^=
    NOT,
    NOT_EQUAL, // ~ ~=

    // 关键字
    RETURN,
    VOID,
    EXTERN, // return void extern
    INCLUDE,
    SIZEOF,
    TYPEDEF,
    REGISTER,
    VOLATILE, // include sizeof typedef register volatile

    FLOAT,
    INT,
    CHAR,
    DOUBLE,
    AUTO, // float int char double auto
    LONG,
    SHORT,
    CONST,
    SIGNED,
    UNSIGNED,
    STATIC, // long short const signed unsigned static
    ENUM,
    STRUCT,
    UNION, // enum struct union
    FALSE,
    TRUE,
    TOEKN_NULL, // false true

    FOR,
    IF,
    WHILE,
    DO,
    ELSE,
    SWITCH,
    CASE, // for if while do else switch case
    BREAK,
    CONTINUE,
    DEFAULT,
    GOTO, // break continue default goto

    IDENTIFIER,
    NUMBER,
    STRING, // 标识符 数字 字符串
    TOKEN_EOF,    // 文件末尾标识符符

    S,
    E,
    T,
    F, //语法分析符号

    //学习使用
    PRINT,
    VAR,
    FUN,
    TOKEN_EEROR,
    SUPER,
    THIS,
    CLASS
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

void initScanner(const char *source);
Token scanToken();

#endif