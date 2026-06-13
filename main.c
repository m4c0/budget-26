#include "sqlite3.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char * slurp(const char * file) {
  FILE * f = fopen(file, "rb");
  assert(f);

  assert(0 == fseek(f, 0, SEEK_END));
  long sz = ftell(f);
  assert(sz);
  assert(0 == fseek(f, 0, SEEK_SET));

  char * data = malloc(sz + 1);
  assert(1 == fread(data, sz, 1, f));
  data[sz] = 0;

  fclose(f);
  return data;
}

static int init() {
  sqlite3 * db;

  if (sqlite3_open("budget-26.sqlite", &db)) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  char * src = slurp("db.sql");

  sqlite3_stmt * stmt;
  if (sqlite3_prepare_v2(db, src, -1, &stmt, 0)) {
    fprintf(stderr, "Can't prepare statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    free(src);
    return 1;
  }

  free(src);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Can't step statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  sqlite3_finalize(stmt);

  sqlite3_close(db);
  return 0;
}

int main() {
  char buf[1024];
  while (1) {
    printf("> ");
    if (!fgets(buf, sizeof(buf), stdin)) break;
    if (0 == strncmp(buf, "quit\n", sizeof(buf))) break;
    else if (0 == strncmp(buf, "init\n", sizeof(buf))) {
      if (init()) return 1;
    }
    else puts("unknown command");
  }
}
