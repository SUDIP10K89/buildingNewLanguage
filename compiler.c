#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKENS 100

// Define token types
typedef enum {
    IDENTIFIER, NUMBER, KEYWORD, OPERATOR, ASSIGN, SEMICOLON, LPAREN, RPAREN
} TokenType;

// Structure for a token
typedef struct {
    TokenType type;
    char value[100];
} Token;

Token tokens[MAX_TOKENS];  // Store tokens
int token_count = 0;

void add_token(TokenType type, char *value) {
    tokens[token_count].type = type;
    strcpy(tokens[token_count].value, value);
    token_count++;
}

void lexer(const char *code) {
    int i = 0;
    while (code[i] != '\0') {
        if (isspace(code[i])) {
            i++;  // Skip spaces
        }
        else if (isalpha(code[i])) {  // Identifiers and keywords
            char buffer[100];
            int j = 0;
            while (isalnum(code[i])) {
                buffer[j++] = code[i++];
            }
            buffer[j] = '\0';

            if (strcmp(buffer, "print") == 0)
                add_token(KEYWORD, buffer);
            else
                add_token(IDENTIFIER, buffer);
        }
        else if (isdigit(code[i])) {  // Numbers
            char buffer[100];
            int j = 0;
            while (isdigit(code[i])) {
                buffer[j++] = code[i++];
            }
            buffer[j] = '\0';
            add_token(NUMBER, buffer);
        }
        else if (code[i] == '=') {
            add_token(ASSIGN, "=");
            i++;
        }
        else if (code[i] == '+') {
            add_token(OPERATOR, "+");
            i++;
        }
        else if (code[i] == ';') {
            add_token(SEMICOLON, ";");
            i++;
        }
        else if (code[i] == '(') {
            add_token(LPAREN, "(");
            i++;
        }
        else if (code[i] == ')') {
            add_token(RPAREN, ")");
            i++;
        }
        else {
            printf("Unknown character: %c\n", code[i]);
            i++;
        }
    }
}

void print_tokens() {
    printf("\nTokens:\n");
    for (int i = 0; i < token_count; i++) {
        printf("Token: %s (Type: %d)\n", tokens[i].value, tokens[i].type);
    }
}

void compile(const char *source_code) {
    printf("Compiling...\n");
    printf("Source Code:\n%s\n", source_code);
}

int main() {
    const char *source_code = "x = 10;\ny = x + 5;\nprint(y);";

    printf("Compiling...\nSource Code:\n%s\n", source_code);

    lexer(source_code);
    print_tokens();

    return 0;
}

