#ifndef STRING_BUF_H
#define STRING_BUF_H
#include <stdio.h>

typedef struct StringBuf {
  char *buf;
  size_t size;
} StringBuf;

int make_string_buf(char *buf, size_t size);
#endif
