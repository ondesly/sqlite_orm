#
#  CMakeLists.txt
#  sqlite_orm
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 10.02.2022.
#  Copyright © 2022-2023 Dmitrii Torkhov. All rights reserved.
#

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(sqlite_orm
        VERSION 1.4.5
        LANGUAGES C CXX)

################
# Dependencies #
################

include(FetchContent)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

FetchContent_Declare(
        sqlite
        GIT_REPOSITORY https://github.com/azadkuh/sqlite-amalgamation.git
        GIT_TAG 3.38.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(sqlite)

find_package(sqlite REQUIRED)

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