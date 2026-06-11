#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int run(char ** args) {
  assert(args && args[0]);

  pid_t pid = fork();
  if (pid == 0) {
    execvp(args[0], args);
    abort();
  } else if (pid > 0) {
    int sl = 0;
    assert(0 <= waitpid(pid, &sl, 0));
    if (WIFEXITED(sl)) return WEXITSTATUS(sl);
  }

  fprintf(stderr, "failed to run child process: %s\n", args[0]);
  return 1;
}

int sqlite() {
  char * args[] = { "clang", "-c", "sqlite3.c", "-o", "sqlite3.o", 0 };
  return run(args);
}

int main() {
  if (sqlite()) return 1;

  { char * args[] = { "clang", "-c", "main.c", "-o", "main.o", 0 };
    if (run(args)) return 1; }

  { char * args[] = { "clang", "main.o", "sqlite3.o", "-o", "budget-26", 0 };
    if (run(args)) return 1; }

  return 0;
}
