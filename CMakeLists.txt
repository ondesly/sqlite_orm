#
#  CMakeLists.txt
#  sqlite
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 10.02.2022.
#  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
#

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(sqlite_orm
        VERSION 1.1.0
        LANGUAGES C CXX)

################
# Dependencies #
################

include(FetchContent)

FetchContent_Declare(
        sqlite
        GIT_REPOSITORY https://github.com/azadkuh/sqlite-amalgamation.git
        GIT_TAG v3.8.10.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(sqlite)

#########
# Build #
#########

add_subdirectory(src)

###########
# Testing #
###########

enable_testing()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif ()