#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256

int main(void) {
  int qtde_mai = 0;
  int qtde_min = 0;
  int qtde_num = 0;

  printf("File name: ");

  char file_name[256];
  if (!fgets(file_name, sizeof(file_name), stdin)) {
    perror("ERRO DE LEITURA DO ARQUIVO\n");
    return 1;
  }

  file_name[strcspn(file_name, "\n")] = '\0';

  FILE *file_ptr = fopen(file_name, "r");

  char line[MAX];
  // Itera sobre linhas do arquivo
  while (fgets(line, sizeof(line), file_ptr)) {
    // Itera sobre caracteres da linha
    for (size_t i = 0; line[i] != '\0'; i++) {
      char c = line[i];
      if (c >= 'A' && c <= 'Z') {
        qtde_mai++;
      } else if (c >= 'a' && c <= 'z') {
        qtde_min++;
      } else if (c >= '0' && c <= '9') {
        qtde_num++;
      }
    }
  }

  fclose(file_ptr);

  printf("Quantidade de números: %d\n", qtde_num);
  printf("Quantidade de maiúsculas: %d\n", qtde_mai);
  printf("Quantidade de minusculas: %d\n", qtde_min);
}