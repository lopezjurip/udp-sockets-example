#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Registry {
  char name[256];
  char value[256];
  char type[256];
  char ttl[256];
};

long read_dns_file(FILE *file, struct Registry *registries[]) {
  long count = 0;
  char line[1024];
  while (fgets(line, sizeof line, file) != NULL) {
    struct Registry *registry = malloc(sizeof(struct Registry));
    sscanf(line, "%s %s %s %s", registry->name, registry->value, registry->type, registry->ttl);
    registries[count++] = registry;
  }
  return count;
}

long line_count(FILE *file) {
  long c;
  long lines = 0;
  while ((c = fgetc(file)) != EOF) {
    if (c == '\n' || c == '\r') ++lines;
  }
  fseek(file, 0, SEEK_SET); // seek back to beginning of file
  return lines;
}

int main(int argc, char const *argv[]) {
  FILE *file = fopen("./server.dns", "r");

  long size = line_count(file);
  struct Registry **registries = calloc(size, sizeof(struct Registry));
  size = read_dns_file(file, registries);

  fclose(file);

  for (size_t i = 0; i < size; i++) {
    printf("%s\n", registries[i]->value);
  }

  return 0;
}
