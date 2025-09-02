#
#  CMakeLists.txt
#  sqlite_orm
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 10.02.2022.
#  Copyright © 2022-2023 Dmitrii Torkhov. All rights reserved.
#

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(sqlite_orm
        VERSION 1.5.1
        LANGUAGES C CXX)

################
# Dependencies #
################

include(FetchContent)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

FetchContent_Declare(
        sqlite3
        GIT_REPOSITORY https://github.com/ondesly/sqlite3.git
        GIT_TAG v3.50.4
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(sqlite3)

#########
# Build #
#########

add_subdirectory(src)

###########
# Testing #
###########

enable_testing()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    if (ANDROID)
        add_subdirectory(tests_android)
    else ()
        add_subdirectory(tests)
    endif ()
endif ()