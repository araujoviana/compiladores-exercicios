#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_SIZE 1024

static const char *const KEYWORDS[] = {
    "asm",           "auto",      "break",        "case",     "char",
    "const",         "continue",  "default",      "do",       "double",
    "else",          "enum",      "extern",       "float",    "for",
    "goto",          "if",        "inline",       "int",      "long",
    "register",      "restrict",  "return",       "short",    "signed",
    "sizeof",        "static",    "struct",       "switch",   "typedef",
    "union",         "unsigned",  "void",         "volatile", "while",
    "_Alignas",      "_Alignof",  "_Atomic",      "_Bool",    "_Complex",
    "_Generic",      "_Imaginary", "_Noreturn",   "_Static_assert",
    "_Thread_local"};

enum ScannerState {
  ST_DEFAULT,
  ST_AFTER_SLASH,
  ST_LINE_COMMENT,
  ST_BLOCK_COMMENT,
  ST_STRING,
  ST_CHAR
};

static int read_file_path(char *file_path, size_t file_path_size, int argc,
                          char *argv[]) {
  if (argc >= 2) {
    if (snprintf(file_path, file_path_size, "%s", argv[1]) >=
        (int)file_path_size) {
      fprintf(stderr, "Erro: caminho do arquivo muito grande.\n");
      return 0;
    }

    return 1;
  }

  printf("Digite o nome do arquivo: ");
  if (!fgets(file_path, file_path_size, stdin)) {
    fprintf(stderr, "Erro de leitura do nome do arquivo.\n");
    return 0;
  }

  file_path[strcspn(file_path, "\n")] = '\0';
  if (file_path[0] == '\0') {
    fprintf(stderr, "Erro: nome do arquivo vazio.\n");
    return 0;
  }

  return 1;
}

static int is_identifier_start(int ch) {
  return isalpha((unsigned char)ch) || ch == '_';
}

static int is_identifier_part(int ch) {
  return isalnum((unsigned char)ch) || ch == '_';
}

static int is_keyword(const char *lexeme) {
  size_t keyword_count = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);
  for (size_t i = 0; i < keyword_count; i++) {
    if (strcmp(KEYWORDS[i], lexeme) == 0) {
      return 1;
    }
  }

  return 0;
}

static void print_uppercase(const char *lexeme) {
  for (size_t i = 0; lexeme[i] != '\0'; i++) {
    putchar(toupper((unsigned char)lexeme[i]));
  }
}

static char *read_identifier(FILE *file_ptr, int first_char) {
  size_t cap = 32;
  size_t len = 0;
  char *lexeme = malloc(cap);
  if (!lexeme) {
    return NULL;
  }

  lexeme[len++] = (char)first_char;

  int ch;
  while ((ch = fgetc(file_ptr)) != EOF && is_identifier_part(ch)) {
    if (len + 1 >= cap) {
      size_t new_cap = cap * 2;
      char *new_buffer = realloc(lexeme, new_cap);
      if (!new_buffer) {
        free(lexeme);
        return NULL;
      }

      lexeme = new_buffer;
      cap = new_cap;
    }

    lexeme[len++] = (char)ch;
  }

  lexeme[len] = '\0';

  if (ch != EOF) {
    ungetc(ch, file_ptr);
  }

  return lexeme;
}

int main(int argc, char *argv[]) {
  char file_path[MAX_PATH_SIZE];
  if (!read_file_path(file_path, sizeof(file_path), argc, argv)) {
    return 1;
  }

  FILE *file_ptr = fopen(file_path, "r");
  if (!file_ptr) {
    perror("Erro ao abrir o arquivo");
    return 1;
  }

  enum ScannerState state = ST_DEFAULT;
  int escaped = 0;
  int previous_block_char = 0;
  int ch;

  while ((ch = fgetc(file_ptr)) != EOF) {
    int handled = 0;

    while (!handled) {
      switch (state) {
      case ST_DEFAULT:
        if (is_identifier_start(ch)) {
          char *lexeme = read_identifier(file_ptr, ch);
          if (!lexeme) {
            fprintf(stderr, "Erro: falta de memoria durante tokenizacao.\n");
            fclose(file_ptr);
            return 1;
          }

          if (is_keyword(lexeme)) {
            print_uppercase(lexeme);
          } else {
            fputs(lexeme, stdout);
          }

          free(lexeme);
        } else if (ch == '/') {
          state = ST_AFTER_SLASH;
        } else {
          putchar(ch);
          if (ch == '"') {
            state = ST_STRING;
            escaped = 0;
          } else if (ch == '\'') {
            state = ST_CHAR;
            escaped = 0;
          }
        }

        handled = 1;
        break;

      case ST_AFTER_SLASH:
        if (ch == '/') {
          putchar('/');
          putchar('/');
          state = ST_LINE_COMMENT;
          handled = 1;
        } else if (ch == '*') {
          putchar('/');
          putchar('*');
          state = ST_BLOCK_COMMENT;
          previous_block_char = '*';
          handled = 1;
        } else {
          putchar('/');
          state = ST_DEFAULT;
        }
        break;

      case ST_LINE_COMMENT:
        putchar(ch);
        if (ch == '\n') {
          state = ST_DEFAULT;
        }
        handled = 1;
        break;

      case ST_BLOCK_COMMENT:
        putchar(ch);
        if (previous_block_char == '*' && ch == '/') {
          state = ST_DEFAULT;
        }
        previous_block_char = ch;
        handled = 1;
        break;

      case ST_STRING:
        putchar(ch);
        if (escaped) {
          escaped = 0;
        } else if (ch == '\\') {
          escaped = 1;
        } else if (ch == '"') {
          state = ST_DEFAULT;
        }
        handled = 1;
        break;

      case ST_CHAR:
        putchar(ch);
        if (escaped) {
          escaped = 0;
        } else if (ch == '\\') {
          escaped = 1;
        } else if (ch == '\'') {
          state = ST_DEFAULT;
        }
        handled = 1;
        break;
      }
    }
  }

  if (state == ST_AFTER_SLASH) {
    putchar('/');
  }

  fclose(file_ptr);
  return 0;
}
