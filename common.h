struct Registry {
  char name[256];
  char value[256];
  char type[256];
  char ttl[256];
};

char *trim(char *str);

int valid_ip(char *ip);
