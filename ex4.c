#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_SIZE 1024

typedef struct {
  size_t line;
  size_t column;
} BracePos;

typedef struct {
  BracePos *data;
  size_t size;
  size_t capacity;
} BraceStack;

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

static int push_brace(BraceStack *stack, size_t line, size_t column) {
  if (stack->size == stack->capacity) {
    size_t new_capacity = stack->capacity == 0 ? 64 : stack->capacity * 2;
    BracePos *new_data = realloc(stack->data, new_capacity * sizeof(*new_data));
    if (!new_data) {
      return 0;
    }

    stack->data = new_data;
    stack->capacity = new_capacity;
  }

  stack->data[stack->size].line = line;
  stack->data[stack->size].column = column;
  stack->size++;
  return 1;
}

static int pop_brace(BraceStack *stack) {
  if (stack->size == 0) {
    return 0;
  }

  stack->size--;
  return 1;
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
  size_t line = 1;
  size_t column = 0;
  BraceStack brace_stack = {0};

  int ch;
  while ((ch = fgetc(file_ptr)) != EOF) {
    if (ch == '\n') {
      line++;
      column = 0;
    } else {
      column++;
    }

    int handled = 0;
    while (!handled) {
      switch (state) {
      case ST_DEFAULT:
        if (ch == '{') {
          if (!push_brace(&brace_stack, line, column)) {
            fprintf(stderr, "Erro: falta de memoria para validar chaves.\n");
            free(brace_stack.data);
            fclose(file_ptr);
            return 1;
          }
        } else if (ch == '}') {
          if (!pop_brace(&brace_stack)) {
            fprintf(stderr,
                    "Erro sintatico: '}' sem '{' correspondente na linha %zu, "
                    "coluna %zu.\n",
                    line, column);
            free(brace_stack.data);
            fclose(file_ptr);
            return 1;
          }
        } else if (ch == '/') {
          state = ST_AFTER_SLASH;
        } else if (ch == '"') {
          state = ST_STRING;
          escaped = 0;
        } else if (ch == '\'') {
          state = ST_CHAR;
          escaped = 0;
        }

        handled = 1;
        break;

      case ST_AFTER_SLASH:
        if (ch == '/') {
          state = ST_LINE_COMMENT;
          handled = 1;
        } else if (ch == '*') {
          state = ST_BLOCK_COMMENT;
          previous_block_char = '*';
          handled = 1;
        } else {
          state = ST_DEFAULT;
        }
        break;

      case ST_LINE_COMMENT:
        if (ch == '\n') {
          state = ST_DEFAULT;
        }
        handled = 1;
        break;

      case ST_BLOCK_COMMENT:
        if (previous_block_char == '*' && ch == '/') {
          state = ST_DEFAULT;
        }
        previous_block_char = ch;
        handled = 1;
        break;

      case ST_STRING:
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

  if (state == ST_BLOCK_COMMENT) {
    fprintf(stderr, "Erro lexico: comentario de bloco nao foi fechado.\n");
    free(brace_stack.data);
    fclose(file_ptr);
    return 1;
  }
  if (state == ST_STRING) {
    fprintf(stderr, "Erro lexico: literal de string nao foi fechado.\n");
    free(brace_stack.data);
    fclose(file_ptr);
    return 1;
  }
  if (state == ST_CHAR) {
    fprintf(stderr, "Erro lexico: literal de caractere nao foi fechado.\n");
    free(brace_stack.data);
    fclose(file_ptr);
    return 1;
  }

  if (brace_stack.size > 0) {
    BracePos missing_close = brace_stack.data[brace_stack.size - 1];
    fprintf(stderr,
            "Erro sintatico: faltou '}' para a chave aberta na linha %zu, "
            "coluna %zu.\n",
            missing_close.line, missing_close.column);
    free(brace_stack.data);
    fclose(file_ptr);
    return 1;
  }

  printf("Chaves corretamente aninhadas.\n");

  free(brace_stack.data);
  fclose(file_ptr);
  return 0;
}
