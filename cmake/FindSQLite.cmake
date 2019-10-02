set(SQLITE_ROOT ${CMAKE_CURRENT_LIST_DIR}/../)

set(SQLITE_INCLUDE_DIRS ${SQLITE_ROOT})

set(SQLITE_SRC
    ${SQLITE_ROOT}/sqlite/sqlite3.c)

add_library(sqlite STATIC ${SQLITE_SRC})