#include "sqlite3.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char * slurped = NULL;
static char * slurp(const char * file) {
  FILE * f = fopen(file, "rb");
  assert(f);

  assert(0 == fseek(f, 0, SEEK_END));
  long sz = ftell(f);
  assert(sz);
  assert(0 == fseek(f, 0, SEEK_SET));

  if (slurped) free(slurped);
  slurped = malloc(sz + 1);
  assert(1 == fread(slurped, sz, 1, f));
  slurped[sz] = 0;

  fclose(f);
  return slurped;
}

static sqlite3 * db_open() {
  sqlite3 * db;
  if (sqlite3_open("budget-26.sqlite", &db)) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return NULL;
  }
  return db;
}
static int db_exec(sqlite3 * db, const char * sql) {
  if (sqlite3_exec(db, sql, NULL, NULL, NULL)) {
    fprintf(stderr, "Can't exec: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }
  sqlite3_close(db);
  return 0;
}

static int cmd_bank() {
  return 0;
}

static int cmd_init() {
  sqlite3 * db = db_open();
  if (db_exec(db, slurp("db.sql"))) return 1;
  sqlite3_close(db);
  return 0;
}

int main() {
  char buf[1024];
  while (1) {
    printf("> ");
    if (!fgets(buf, sizeof(buf), stdin)) break;
    if (0 == strcmp(buf, "quit\n")) break;
    else if (0 == strcmp(buf, "bank\n")) { if (cmd_bank()) return 1; }
    else if (0 == strcmp(buf, "init\n")) { if (cmd_init()) return 1; }
    else puts("unknown command");
  }
}
