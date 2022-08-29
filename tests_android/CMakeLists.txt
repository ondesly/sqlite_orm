#
#  CMakeLists.txt
#  sqlite_orm
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 29.08.2022.
#  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
#

add_library(sqliteormtest SHARED
        app/src/main/cpp/main.cpp)

target_link_libraries(sqliteormtest PUBLIC sqlite_orm android)

set_target_properties(sqliteormtest PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES)
set_target_properties(sqliteormtest PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)