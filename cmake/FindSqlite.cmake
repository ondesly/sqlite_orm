#
#  FindSqlite.cmake
#  sqlite_orm
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 29.08.2022.
#  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
#

add_library(sqlite STATIC
        ${sqlite_SOURCE_DIR}/sqlite3.c)

target_include_directories(sqlite PUBLIC
        $<BUILD_INTERFACE:${sqlite_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

set_target_properties(sqlite PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES)