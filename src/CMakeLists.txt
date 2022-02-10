#
#  CMakeLists.txt
#  sqlite
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 10.02.2022.
#  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
#

set(SRC
        sqlite/sqlite3.c)

add_library(sqlite STATIC ${SRC})

#

target_include_directories(sqlite
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include/sqlite>)

set_target_properties(sqlite PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)