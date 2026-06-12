CREATE TABLE IF NOT EXISTS periodic (
  per_pk       INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  per_period   INTEGER NOT NULL,
  per_name     TEXT NOT NULL,
  per_currency TEXT NOT NULL,
  per_value    REAL NOT NULL
) STRICT;
