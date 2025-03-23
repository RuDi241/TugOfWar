#include "../include/config.h"

int main(int argc, char *argv[]) {

  Config config;
  read_config("./config.txt", &config);
  fprintf_config(stdout, &config);
  return 0;
}
