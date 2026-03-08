#include <stdio.h>
#include <string.h>

#define MAX_PATH_SIZE 1024

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

  int ch;
  long line_count = 0;
  int last_char = '\n';
  int read_any_char = 0;

  while ((ch = fgetc(file_ptr)) != EOF) {
    read_any_char = 1;
    if (ch == '\n') {
      line_count++;
    }

    last_char = ch;
  }

  if (read_any_char && last_char != '\n') {
    line_count++;
  }

  printf("CONTAGEM DE LINHAS: %ld\n", line_count);

  fclose(file_ptr);
  return 0;
}
