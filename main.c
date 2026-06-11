#include "sqlite3.h"

#include <assert.h>
#include <stdio.h>

int main() {
  sqlite3 * db;

  if (sqlite3_open("budget-26.sqlite", &db)) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  sqlite3_stmt * stmt;
  if (sqlite3_prepare_v2(db, "SELECT ?", -1, &stmt, 0)) {
    fprintf(stderr, "Can't prepare statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  sqlite3_bind_int(stmt, 1, 69);

  if (sqlite3_step(stmt) != SQLITE_ROW) {
    fprintf(stderr, "Can't step statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  fprintf(stderr, "==> %d\n", sqlite3_column_int(stmt, 0));

  sqlite3_finalize(stmt);

  sqlite3_close(db);
  return 0;
}
