#
#  CMakeLists.txt
#  sqlite_orm
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 10.02.2022.
#  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
#

add_library(sqlite_orm STATIC sqlite_orm/aasset_vfs.cpp)

target_link_libraries(sqlite_orm PUBLIC sqlite)

target_include_directories(sqlite_orm
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>)

set_target_properties(sqlite_orm PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)