#include "list_type.h"
#include "string_type.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static void error(int32_t line, const char* message)
{
    printf("Error at line %d: %s\n", line, message);
}

typedef enum token_category_type {
    LEFT_SQUARE,
    RIGHT_SQUARE,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,

    EQUAL,
    COLON_EQUAL,

    IDENTIFIER,
    STRING,

    VAR,
    SET,

    EOF_,
} tc_type;

typedef struct token_type
{
    tc_type tc;
    string_type lexeme;
    int32_t line;
} token_type;

static token_type tokenize_string(char buf[512], int32_t* pos, int32_t line)
{
    char lexeme[512];
    memset(lexeme, 0, 512);

    size_t i = strcspn(buf + *pos + 1, "\"");
    strncpy(lexeme, buf + *pos + 1, i);
    *pos += i + 1;

    string_type st;
    string_new(&st, lexeme);

    token_type tt = {STRING, st, line};

    return tt;
}

static token_type tokenize_identifier(char buf[512], int32_t* pos, int32_t line)
{
    char c = buf[*pos];
    int32_t i = 0;
    while (isalpha(c) || c == '_')
    {
        i++;
        c = buf[i + *pos];
    }

    char val[512];
    memset(val, 0, 512);

    strncpy(val, buf + *pos, i);

    *pos += i;

    token_type tt;
    if (strcmp(val, "set") == 0)
    {
        tt.tc = SET;
        tt.lexeme = STRING_EMPTY;
    }
    else if (strcmp(val, "var") == 0)
    {
        tt.tc = VAR;
        tt.lexeme = STRING_EMPTY;
    }
    else
    {
        tt.tc = IDENTIFIER;
        string_type st;
        string_new(&st, val);
        tt.lexeme = st;
    }
    tt.line = line;

    return tt;
}

list_type scan_tokens(FILE* fptr)
{
    list_type tokens;
    list_new(&tokens, sizeof(token_type), 5);

    int32_t start = 0;
    int32_t current = 0;
    int32_t line = 1;

    char buf[512];
    while (fgets(buf, 512, fptr))
    {
        current = 0;
        char c = buf[current];
        bool flag = true;

        while (c != '\n' && c != '\0' && flag)
        {
            switch (c)
            {
                case '[': {
                    token_type tt = {LEFT_SQUARE, STRING_EMPTY, 0};
                    list_append(&tokens, &tt);
                    break;
                }
                case ']': {
                    token_type tt = {RIGHT_SQUARE, STRING_EMPTY, 0};
                    list_append(&tokens, &tt);
                    break;
                }
                case '{': {
                    token_type tt = {LEFT_BRACE, STRING_EMPTY, 0};
                    list_append(&tokens, &tt);
                    break;
                }
                case '}': {
                    token_type tt = {RIGHT_BRACE, STRING_EMPTY, 0};
                    list_append(&tokens, &tt);
                    break;
                }
                case '=': {
                    token_type tt = {EQUAL, STRING_EMPTY, 0};
                    list_append(&tokens, &tt);
                    break;
                }
                case ',': {
                    token_type tt = {COMMA, STRING_EMPTY, 0};
                    list_append(&tokens, &tt);
                    break;
                }
                case '"': {
                    token_type tt = tokenize_string(buf, &current, line);
                    list_append(&tokens, &tt);
                    break;
                }

                case '#':
                    flag = false;
                    break;

                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    break;

                default: {
                    if (isalpha(c))
                    {
                        token_type tt = tokenize_identifier(buf, &current, line);
                        list_append(&tokens, &tt);
                    }
                    else
                    {
                        error(line, "unexpected character");
                    }
                    break;
                }
            }

            current++;
            c = buf[current];
        }

        line++;
    }

    return tokens;
}

// Usage example
// int main()
// {
//     FILE* fptr = fopen("breakfile", "r");
//
//     list_type lt = scan_tokens(fptr);
//     for (size_t i = 0; i < lt.length; i++)
//     {
//         if (!string_empty(&((token_type*) list_get(&lt, i))->lexeme))
//         {
//             printf("%s\n", ((token_type*) list_get(&lt, i))->lexeme.str);
//         }
//     }
//
//     for (size_t i = 0; i < lt.length; i++)
//     {
//         if (!string_empty(&((token_type*) list_get(&lt, i))->lexeme))
//         {
//             string_free(&((token_type*) list_get(&lt, i))->lexeme);
//         }
//     }
//     list_free(&lt);
//
//     fclose(fptr);
//     return 0;
// }
