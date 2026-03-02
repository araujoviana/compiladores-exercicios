#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  char *reserv[] = {"asm",    "auto",     "break",   "case",   "char",
                    "const",  "continue", "default", "do",     "double",
                    "else",   "enum",     "extern",  "float",  "for",
                    "goto",   "if",       "int",     "long",   "register",
                    "return", "short",    "signed",  "sizeof", "static",
                    "struct", "switch",   "typedef", "union",  "unsigned",
                    "void",   "volatile", "while"};

  printf("Arquivo: ");
  char file_name[256];
  if (!fgets(file_name, sizeof(file_name), stdin)) {
    perror("Erro de leitura\n");
    return 1;
  }

  file_name[strcspn(file_name, "\n")] = '\0';

  FILE *file_ptr = fopen(file_name, "r");
  if (!file_ptr) {
    perror("Erro de leitura\n");
    return 1;
  }

  char line[1024];

  size_t reserv_size = sizeof(reserv) / sizeof(reserv[0]);

  // Itera sobre todas as linhas
  while (fgets(line, sizeof(line), file_ptr)) {
    // Palavra
    char word[1024];
    // Indice da palavra
    size_t word_idx = 0;
    // Itera sobre caracteres da linha atual
    for (int i = 0; line[i] != '\0'; i++) {
      // Caracter atual
      char c = line[i];
      // Se token não estiver completa (com espaço ou ponto e virgula)
      if (c != ' ' && c != ';' && c != '\n') {
        // Adiciona à palavra
        word[word_idx++] = c;
      }
      // Se a palavra estiver completa
      else {
        word[word_idx] = '\0';

        for (size_t j = 0; j < reserv_size; j++) {
          // É palavra reservada
          if (!strcmp(reserv[j], word)) {
            // Transforma em maiúsculo
            for (size_t n = 0; n <= word_idx; n++) {
              if (word[n] >= 'a' && word[n] <= 'z') {
                word[n] -= 32;
              }
            }
            break;
          }
        }

        // Imprime palavra e o seu delimitador
        printf("%s%c", word, c);
        word_idx = 0;

        // Esvazia palavra
        for (int j = word_idx; j > 0; j--) {
          word[j] = '\0';
        }
      }
    }
  }

  fclose(file_ptr);
}