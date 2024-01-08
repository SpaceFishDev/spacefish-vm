#include "vm.h"

typedef struct
{
    char type;
    char *content;
} Token;

#define IS_NUM(c) (c >= '0' && c <= '9')
#define TOKEN(type, content) ((Token){type, content})

enum
{
    NUM,
    STRING,
    KEYWORD,
    WS,
    COLON,
    COMMA,
    COMMENT,
    NL,
    END,
};

#define IS_LETTER(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

Token lex(char **src)
{
    char *str = *src;
    char c = *str;
    if (IS_NUM(c))
    {
        char *begin = str;
        char has_decimal = 0;
        while (IS_NUM(*str) || (!has_decimal && *str == '.'))
        {
            if (*str == '.')
            {
                has_decimal = 1;
            }
            ++str;
        }
        int len = str - begin;
        char *content = calloc(1, len + 1);
        memcpy(content, begin, len);
        *src += len;
        return TOKEN(NUM, content);
    }
    if (c == '"')
    {
        ++str;
        char *begin = str;
        while (*str != '"')
        {
            ++str;
        }
        int len = str - begin;
        char *content = calloc(1, len + 1);
        memcpy(content, begin, len);
        ++str;
        *src += len + 2;
        return TOKEN(STRING, content);
    }
    if (IS_LETTER(c))
    {
        char *begin = str;
        while (IS_LETTER(*str) || IS_NUM(*str) || *str == '_' || *str == '.')
        {
            ++str;
        }
        int len = str - begin;
        char *content = calloc(1, len + 1);
        memcpy(content, begin, len);
        *src += len;
        return TOKEN(KEYWORD, content);
    }
    if (c == ' ' || c == '\t')
    {
        ++*src;
        char *content = malloc(2);
        content[0] = c;
        content[1] = 0;
        if (!*src)
        {
            return TOKEN(WS, content);
        }
        return lex(src);
    }
    if (c == '\0')
    {
        return TOKEN(END, "\\0");
    }
    if (c == ';')
    {
        char *begin = str + 1;
        while (*str && *str != '\n')
        {
            ++str;
        }
        int len = str - begin;
        char *content = calloc(1, len + 1);
        memcpy(content, begin, len);
        *src += len + 1;
        if (!*src)
        {
            return TOKEN(COMMENT, content);
        }
        return lex(src);
    }
    if (c == '\n')
    {
        ++*src;
        if (!*src)
        {
            return TOKEN(NL, "\\n");
        }
        return lex(src);
    }
    if (c == ',')
    {
        ++*src;
        return TOKEN(COMMA, ",");
    }
    if (c == ':')
    {
        ++*src;
        return TOKEN(COLON, ":");
    }
    return TOKEN(-1, NULL);
}

char *read_file(char *path)
{
    FILE *fp = fopen(path, "r");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        int len = ftell(fp);
        int x = fseek(fp, 0, SEEK_SET);
        char *buffer = calloc(1, len + 1);
        x = fread(buffer, len, 1, fp);
        if (!x)
        {
            printf("sad pepe\n");
        }
        fclose(fp);
        return buffer;
    }
    fclose(fp);
    return 0;
}

enum
{
    LABEL,
    WPUSH,
    WPOP,
    WADD,
    WPUTNUM,
    END_OF_FILE,
    ERR,
};

// idk what to call it ngl so we go with word
typedef struct
{
    int type;
    Token data;
} word;

#define WORD(t, d) \
    ((word){t, d})

word parse_words(Token **token_ptr)
{
    Token *tokens = *token_ptr;
    switch ((*tokens).type)
    {
    case KEYWORD:
    {
        if (!strcmp(tokens[0].content, "push"))
        {
            *token_ptr += 2;
            return WORD(WPUSH, tokens[1]);
        }
        if (!strcmp(tokens[0].content, "pop"))
        {
            *token_ptr += 1;
            return WORD(WPOP, tokens[0]);
        }
        if (!strcmp(tokens[0].content, "add"))
        {
            *token_ptr += 1;
            return WORD(WADD, tokens[0]);
        }
        if (!strcmp(tokens[0].content, "put_num"))
        {
            *token_ptr += 1;
            return WORD(WPUTNUM, tokens[0]);
        }
        if (tokens[1].type == COLON)
        {
            *token_ptr += 2;
            return WORD(LABEL, tokens[0]);
        }
    }
    break;
    case END:
    {
        return WORD(END_OF_FILE, tokens[0]);
    }
    }
    *token_ptr += sizeof(Token);
    return WORD(ERR, TOKEN(0, ""));
}

#define TOKEN_TYPE_TO_STRING(x) ((x <= END) ? (((char *[]){"NUM",           \
                                                           "STRING",        \
                                                           "KEYWORD",       \
                                                           "WS",            \
                                                           "COLON",         \
                                                           "COMMA",         \
                                                           "COMMENT",       \
                                                           "NL",            \
                                                           "END"})[(int)x]) \
                                            : "NO CLUE MATE")

int main(void)
{
    char *src = read_file("main.sfvm");
    Token *tokens = malloc(100 * sizeof(Token)); // FOR NOW PLEASE FUTURE ME MAKE THIS ACTUALLY COUNT N SHIT
    int t_i = 0;
    while (1)
    {
        Token t = lex(&src);
        tokens[t_i] = t;
        ++t_i;
        printf("[%s][%s]\n", TOKEN_TYPE_TO_STRING(t.type), t.content);
        if (t.type == END || t.type == -1)
        {
            break;
        }
    }
    while (1)
    {
        word w = parse_words(&tokens);
        printf("%d, %s\n", w.type, w.data.content);
        if (w.type == ERR || w.type == END_OF_FILE)
        {
            return 0; // swaggers
        }
    }
}