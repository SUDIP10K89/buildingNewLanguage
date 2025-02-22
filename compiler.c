#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKENS 100

// AST Node types
typedef enum {
    AST_ASSIGN, AST_PRINT, AST_IF, AST_EXPR
} ASTNodeType;

// AST Node structure
typedef struct Node {
    ASTNodeType type;
    union {
        struct { char* var; struct Node* expr; } assign;
        struct { struct Node* expr; } print;
        struct { struct Node* cond; struct Node* then; struct Node* els; } if_stmt;
        struct { char* value; struct Node* left; struct Node* right; char* op; } expr;
    } data;
    struct Node* next;  // Explicit next pointer for statement list
} Node;

// Function prototypes
Node* parse_statement();
void print_error(const char* message, int line, const char* token);

// Token types enumeration
typedef enum {
    IDENTIFIER, NUMBER, KEYWORD, OPERATOR, ASSIGN, SEMICOLON,
    LPAREN, RPAREN, LBRACE, RBRACE, COMPARE_OP
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char value[100];
} Token;

// Global variables
Token tokens[MAX_TOKENS];
int token_count = 0;
int current_token = 0;
FILE *output_file;
int line_number = 1;
int errors_found = 0;

// Add a token to the token array
void add_token(TokenType type, char *value) {
    if (token_count >= MAX_TOKENS) {
        print_error("Token limit exceeded", line_number, value);
        errors_found = 1;
        return;
    }
    tokens[token_count].type = type;
    strncpy(tokens[token_count].value, value, sizeof(tokens[token_count].value) - 1);
    tokens[token_count].value[sizeof(tokens[token_count].value) - 1] = '\0';
    token_count++;
}

// Print error message with line number and token context
void print_error(const char* message, int line, const char* token) {
    if (token) {
        printf("Error (line %d): %s near token '%s'\n", line, message, token);
    } else {
        printf("Error (line %d): %s\n", line, message);
    }
    errors_found = 1;
}

// Lexical analyzer
void lexer(const char *code) {
    int i = 0;
    while (code[i] != '\0') {
        if (code[i] == '\n') {
            line_number++;
            i++;
            continue;
        }
        if (isspace(code[i])) {
            i++;
            continue;
        } else if (isalpha(code[i])) {
            char buffer[100] = {0};
            int j = 0;
            while (isalnum(code[i])) buffer[j++] = code[i++];
            if (strcmp(buffer, "print") == 0 || strcmp(buffer, "if") == 0 || strcmp(buffer, "else") == 0)
                add_token(KEYWORD, buffer);
            else
                add_token(IDENTIFIER, buffer);
        } else if (isdigit(code[i])) {
            char buffer[100] = {0};
            int j = 0;
            while (isdigit(code[i])) buffer[j++] = code[i++];
            add_token(NUMBER, buffer);
        } else if (code[i] == '=') {
            if (code[i + 1] == '=') {
                add_token(COMPARE_OP, "==");
                i += 2;
            } else {
                add_token(ASSIGN, "=");
                i++;
            }
        } else if (code[i] == '+' || code[i] == '-' || code[i] == '*' || code[i] == '/') {
            add_token(OPERATOR, (char[]){code[i], '\0'});
            i++;
        } else if (code[i] == '>' || code[i] == '<') {
            add_token(OPERATOR, (char[]){code[i], '\0'});
            i++;
        } else if (code[i] == ';') {
            add_token(SEMICOLON, ";");
            i++;
        } else if (code[i] == '(') {
            add_token(LPAREN, "(");
            i++;
        } else if (code[i] == ')') {
            add_token(RPAREN, ")");
            i++;
        } else if (code[i] == '{') {
            add_token(LBRACE, "{");
            i++;
        } else if (code[i] == '}') {
            add_token(RBRACE, "}");
            i++;
        } else {
            char error_msg[100];
            snprintf(error_msg, sizeof(error_msg), "Unknown character: %c", code[i]);
            print_error(error_msg, line_number, NULL);
            i++;
        }
    }
}

// Match a token type and advance if matched
int match(TokenType expected) {
    if (current_token >= token_count) return 0;
    if (tokens[current_token].type == expected) {
        current_token++;
        return 1;
    }
    return 0;
}

// Create a new AST node
Node* create_node(ASTNodeType type) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        print_error("Memory allocation failed", line_number, NULL);
        exit(1);
    }
    node->type = type;
    node->next = NULL;  // Initialize next pointer
    return node;
}

// Parse an expression
Node* parse_expression() {
    Node* expr = NULL;
    if (match(NUMBER) || match(IDENTIFIER)) {
        expr = create_node(AST_EXPR);
        expr->data.expr.value = strdup(tokens[current_token - 1].value);
        expr->data.expr.left = NULL;
        expr->data.expr.right = NULL;
        expr->data.expr.op = NULL;
        while (match(OPERATOR)) {
            Node* op_node = create_node(AST_EXPR);
            op_node->data.expr.op = strdup(tokens[current_token - 1].value);
            op_node->data.expr.left = expr;
            if (match(NUMBER) || match(IDENTIFIER)) {
                op_node->data.expr.right = create_node(AST_EXPR);
                op_node->data.expr.right->data.expr.value = strdup(tokens[current_token - 1].value);
                op_node->data.expr.right->data.expr.left = NULL;
                op_node->data.expr.right->data.expr.right = NULL;
                op_node->data.expr.right->data.expr.op = NULL;
            } else {
                print_error("Expected number or identifier after operator", line_number, tokens[current_token].value);
                free(op_node);
                return expr;
            }
            expr = op_node;
        }
    } else {
        print_error("Expected number or identifier in expression", line_number, tokens[current_token].value);
    }
    return expr;
}

// Parse an assignment
Node* parse_assignment() {
    Node* node = NULL;
    if (match(IDENTIFIER)) {
        node = create_node(AST_ASSIGN);
        node->data.assign.var = strdup(tokens[current_token - 1].value);
        if (match(ASSIGN)) {
            node->data.assign.expr = parse_expression();
            if (!match(SEMICOLON)) {
                print_error("Missing semicolon after assignment", line_number, tokens[current_token].value);
            }
        } else {
            char error_msg[100];
            snprintf(error_msg, sizeof(error_msg), "Expected '=' after identifier '%s'", node->data.assign.var);
            print_error(error_msg, line_number, tokens[current_token].value);
            free(node->data.assign.var);
            free(node);
            return NULL;
        }
    }
    return node;
}

// Parse a print statement
Node* parse_print() {
    Node* node = NULL;
    if (match(KEYWORD) && strcmp(tokens[current_token - 1].value, "print") == 0) {
        node = create_node(AST_PRINT);
        if (match(LPAREN)) {
            if (match(IDENTIFIER) || match(NUMBER)) {
                node->data.print.expr = create_node(AST_EXPR);
                node->data.print.expr->data.expr.value = strdup(tokens[current_token - 1].value);
                node->data.print.expr->data.expr.left = NULL;
                node->data.print.expr->data.expr.right = NULL;
                node->data.print.expr->data.expr.op = NULL;
            } else {
                node->data.print.expr = NULL;  // Empty print()
            }
            if (!match(RPAREN)) {
                print_error("Expected ')' after print", line_number, tokens[current_token].value);
            }
            if (!match(SEMICOLON)) {
                print_error("Missing semicolon after print statement", line_number, tokens[current_token].value);
            }
        } else {
            print_error("Expected '(' after 'print'", line_number, tokens[current_token].value);
            free(node);
            return NULL;
        }
    }
    return node;
}

// Parse if-else statement
Node* parse_if_else() {
    Node* node = NULL;
    if (match(KEYWORD) && strcmp(tokens[current_token - 1].value, "if") == 0) {
        node = create_node(AST_IF);
        if (match(LPAREN)) {
            node->data.if_stmt.cond = parse_expression();
            if (match(RPAREN)) {
                if (match(LBRACE)) {
                    Node* then_list = NULL;
                    Node* last_then = NULL;
                    while (!match(RBRACE) && current_token < token_count) {
                        Node* stmt = parse_statement();
                        if (stmt) {
                            if (!then_list) {
                                then_list = stmt;
                            } else {
                                last_then->next = stmt;  // Use explicit next pointer
                            }
                            last_then = stmt;
                        }
                    }
                    node->data.if_stmt.then = then_list;
                    if (match(KEYWORD) && strcmp(tokens[current_token - 1].value, "else") == 0) {
                        if (match(LBRACE)) {
                            Node* else_list = NULL;
                            Node* last_else = NULL;
                            while (!match(RBRACE) && current_token < token_count) {
                                Node* stmt = parse_statement();
                                if (stmt) {
                                    if (!else_list) {
                                        else_list = stmt;
                                    } else {
                                        last_else->next = stmt;  // Use explicit next pointer
                                    }
                                    last_else = stmt;
                                }
                            }
                            node->data.if_stmt.els = else_list;
                        } else {
                            print_error("Expected '{' after 'else'", line_number, tokens[current_token].value);
                        }
                    } else {
                        node->data.if_stmt.els = NULL;
                    }
                } else {
                    print_error("Expected '{' after if condition", line_number, tokens[current_token].value);
                }
            } else {
                print_error("Expected ')' after if condition", line_number, tokens[current_token].value);
            }
        } else {
            print_error("Expected '(' after 'if'", line_number, tokens[current_token].value);
            free(node);
            return NULL;
        }
    }
    return node;
}

// Parse a statement
Node* parse_statement() {
    if (current_token >= token_count) return NULL;
    if (tokens[current_token].type == IDENTIFIER) {
        return parse_assignment();
    } else if (tokens[current_token].type == KEYWORD) {
        if (strcmp(tokens[current_token].value, "print") == 0) {
            return parse_print();
        } else if (strcmp(tokens[current_token].value, "if") == 0) {
            return parse_if_else();
        } else {
            char error_msg[100];
            snprintf(error_msg, sizeof(error_msg), "Unknown keyword '%s'", tokens[current_token].value);
            print_error(error_msg, line_number, tokens[current_token].value);
            current_token++;
        }
    } else {
        char error_msg[100];
        snprintf(error_msg, sizeof(error_msg), "Unexpected token '%s'", tokens[current_token].value);
        print_error(error_msg, line_number, tokens[current_token].value);
        current_token++;
    }
    return NULL;
}

// Generate C code from AST
void generate_code(Node* node, FILE* file) {
    if (!node) return;
    switch (node->type) {
        case AST_ASSIGN:
            fprintf(file, "    int %s = ", node->data.assign.var);
            generate_code(node->data.assign.expr, file);
            fprintf(file, ";\n");
            break;
        case AST_PRINT:
            if (node->data.print.expr) {
                fprintf(file, "    printf(\"%%d\\n\", ");
                generate_code(node->data.print.expr, file);
                fprintf(file, ");\n");
            } else {
                fprintf(file, "    printf(\"\\n\");\n");
            }
            break;
        case AST_IF:
            fprintf(file, "    if (");
            generate_code(node->data.if_stmt.cond, file);
            fprintf(file, ") {\n");
            Node* then_stmt = node->data.if_stmt.then;
            while (then_stmt) {
                generate_code(then_stmt, file);
                then_stmt = then_stmt->next;  // Use explicit next pointer
            }
            fprintf(file, "    }\n");
            if (node->data.if_stmt.els) {
                fprintf(file, "    else {\n");
                Node* else_stmt = node->data.if_stmt.els;
                while (else_stmt) {
                    generate_code(else_stmt, file);
                    else_stmt = else_stmt->next;  // Use explicit next pointer
                }
                fprintf(file, "    }\n");
            }
            break;
        case AST_EXPR:
            if (node->data.expr.op) {
                generate_code(node->data.expr.left, file);
                fprintf(file, " %s ", node->data.expr.op);
                generate_code(node->data.expr.right, file);
            } else {
                fprintf(file, "%s", node->data.expr.value);
            }
            break;
    }
}

// Free AST memory
void free_ast(Node* node) {
    if (!node) return;
    switch (node->type) {
        case AST_ASSIGN:
            free(node->data.assign.var);
            free_ast(node->data.assign.expr);
            break;
        case AST_PRINT:
            free_ast(node->data.print.expr);
            break;
        case AST_IF:
            free_ast(node->data.if_stmt.cond);
            free_ast(node->data.if_stmt.then);
            free_ast(node->data.if_stmt.els);
            break;
        case AST_EXPR:
            if (node->data.expr.value) free(node->data.expr.value);
            if (node->data.expr.op) free(node->data.expr.op);
            free_ast(node->data.expr.left);
            free_ast(node->data.expr.right);
            break;
    }
    free_ast(node->next);  // Free the next node in the list
    free(node);
}

// Main function
int main() {
    char *source_code = NULL;
    size_t buffer_size = 0;
    size_t input_length = 0;

    printf("Enter your source code (press Ctrl+D or Ctrl+Z then Enter to finish):\n");
    input_length = getline(&source_code, &buffer_size, stdin);
    if (input_length == -1) {
        printf("Error reading input or no input provided.\n");
        free(source_code);
        return 1;
    }
    if (input_length > 0 && source_code[input_length - 1] == '\n') {
        source_code[input_length - 1] = '\0';
    }

    printf("Source Code:\n%s\n", source_code);

    lexer(source_code);
    if (errors_found) {
        printf("Errors found during lexing. Aborting.\n");
        free(source_code);
        return 1;
    }

    Node* ast = NULL;
    Node* last_node = NULL;
    current_token = 0;
    while (current_token < token_count) {
        Node* stmt = parse_statement();
        if (stmt) {
            if (!ast) {
                ast = stmt;
            } else {
                last_node->next = stmt;  // Use explicit next pointer
            }
            last_node = stmt;
        }
    }

    if (errors_found) {
        printf("Errors found during parsing. Aborting.\n");
        free_ast(ast);
        free(source_code);
        return 1;
    }

    output_file = fopen("output.c", "w");
    if (!output_file) {
        print_error("Unable to open output file", line_number, NULL);
        free_ast(ast);
        free(source_code);
        return 1;
    }
    fprintf(output_file, "#include <stdio.h>\nint main() {\n");
    Node* current = ast;
    while (current) {
        generate_code(current, output_file);
        current = current->next;  // Use explicit next pointer
    }
    fprintf(output_file, "    return 0;\n}\n");
    fclose(output_file);

    printf("\nC code generated in output.c\n");

    printf("\nCompiling output.c...\n");
    if (system("gcc output.c -o output") != 0) {
        printf("Error: Compilation of output.c failed.\n");
    } else {
        printf("Running the compiled program...\n");
        printf("----- Program Output -----\n");
        if (system("./output") != 0) {
            printf("Error: Execution of output failed.\n");
        }
        printf("-------------------------\n");
    }

    free_ast(ast);
    free(source_code);
    return 0;
}
