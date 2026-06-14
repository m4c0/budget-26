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

static sqlite3 * db;
static int db_open() {
  if (!sqlite3_open("budget-26.sqlite", &db)) return 0;
  fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  db = NULL;
  return 1;
}
static int db_exec(const char * sql) {
  if (!sqlite3_exec(db, sql, NULL, NULL, NULL)) return 0;
  fprintf(stderr, "Can't exec: %s\n", sqlite3_errmsg(db));
  return 1;
}
static sqlite3_stmt * db_prepare(const char * sql) {
  sqlite3_stmt * stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)) return NULL;
  return stmt;
}

static int cmd_bank() {
  sqlite3_stmt * stmt = db_prepare("SELECT * FROM bnk ORDER BY bnk_name");
  if (!stmt) return 1;

  printf("ID  Name\n");
  while (sqlite3_step(stmt) != SQLITE_DONE) {
    printf("%3d %s\n",
        sqlite3_column_int(stmt, 0),
        sqlite3_column_text(stmt, 1));
  }

  sqlite3_finalize(stmt);
  return 0;
}

static int cmd_bank_add() {
  printf("Name> ");

  char name[1024];
  if (!fgets(name, sizeof(name), stdin)) return 1;

  sqlite3_stmt * stmt = db_prepare("INSERT INTO bnk (bnk_name) VALUES (?)");
  sqlite3_bind_text(stmt, 1, name, strlen(name) - 1, NULL);
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Can't step: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return 1;
  }

  sqlite3_finalize(stmt);
  return 0;
}

static int cmd_cat() {
  sqlite3_stmt * stmt = db_prepare("SELECT * FROM cat ORDER BY cat_name");
  if (!stmt) return 1;

  printf("ID  Name\n");
  while (sqlite3_step(stmt) != SQLITE_DONE) {
    printf("%3d %s\n",
        sqlite3_column_int(stmt, 0),
        sqlite3_column_text(stmt, 1));
  }

  sqlite3_finalize(stmt);
  return 0;
}

static int cmd_cat_add() {
  printf("Name> ");

  char name[1024];
  if (!fgets(name, sizeof(name), stdin)) return 1;

  sqlite3_stmt * stmt = db_prepare("INSERT INTO cat (cat_name) VALUES (?)");
  sqlite3_bind_text(stmt, 1, name, strlen(name) - 1, NULL);
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Can't step: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    return 1;
  }

  sqlite3_finalize(stmt);
  return 0;
}

static int cmd_init() {
  return db_exec(slurp("db.sql"));
}

static int loop() {
  char buf[1024];
  while (1) {
    printf("> ");
    if (!fgets(buf, sizeof(buf), stdin)) break;
    if (0 == strcmp(buf, "quit\n")) break;
    else if (0 == strcmp(buf, "bank\n"))     { if (cmd_bank())     return 1; }
    else if (0 == strcmp(buf, "bank add\n")) { if (cmd_bank_add()) return 1; }
    else if (0 == strcmp(buf, "cat\n"))      { if (cmd_cat())      return 1; }
    else if (0 == strcmp(buf, "cat add\n"))  { if (cmd_cat_add())  return 1; }
    else puts("unknown command");
  }
  return 0;
}

int main() {
  if (db_open()) return 1;
  if (cmd_init()) return 1;

  int res = loop();
  sqlite3_close(db);

  return res;
}
