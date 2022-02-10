#
#  CMakeLists.txt
#  sqlite
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 10.02.2022.
#  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
#

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(sqlite
        VERSION 1.0.0
        LANGUAGES C CXX)

#########
# Build #
#########

add_subdirectory(src)