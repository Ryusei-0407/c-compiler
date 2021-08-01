#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED,  // Symbol
    TK_NUM,       // Integer token
    TK_EOF,       // EOF token
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;  // Token type
    Token *next;     // Next input token
    int val;         // If kind == TK_NUM
    char *str;       // Token string
};

// Forcus token
Token *token;

// Report errors
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// When the next token is the expected symbol
// it will read one token and return true. Otherwise, it returns false
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;

    return true;
}

// When the next token is the expected symbol
// it will read one token. Otherwise, it report error
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error("'%c' is not", op);
    token = token->next;
}

// When the next token is the expected symbol
// it will read one token. Otherwise, it returns number
int expect_number() {
    if (token->kind != TK_NUM)
        error("not number");
    int val = token->val;
    token = token->next;

    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// Create a new token and connect it to `cur`
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;

    return tok;
}

// Tokenize for input string `p`
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // Skip to space
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("Can't tokenize");
    }
        
    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid argument\n");
        return 1;
    }

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // Check for first number
    printf("    mov rax, %d\n", expect_number());

    // 
    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }
    
    printf("    ret\n");
    return 0;
}
