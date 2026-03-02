#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 1024

int main(void) {
  char filename[MAX];
  if (!fgets(filename, sizeof(filename), stdin)) {
    perror("Error");
    return 1;
  }
  filename[strcspn(filename, "\n")] = '\0';
}
