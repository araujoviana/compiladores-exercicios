#include <stdio.h>
#include <string.h>

#define maxline 256

int main(void) {
  printf("Digite o nome do arquivo: ");
  char filename[maxline];
  if (!fgets(filename, sizeof(filename), stdin)) {
    printf("Erro de leitura\n");
    return 1;
  }
  filename[strcspn(filename, "\n")] = '\0';

  FILE *file_ptr = fopen(filename, "r");
  if (!file_ptr) {
    printf("Nao foi possivel abrir");
    return 1;
  }

  char line[maxline];
  int line_count = 0;

  while (fgets(line, sizeof(line), file_ptr)) {
    line_count++;
  }

  printf("CONTAGEM DE LINHAS: %d\n", line_count);

  fclose(file_ptr);

  return 0;
}