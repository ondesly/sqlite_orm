#
#  CMakeLists.txt
#  sqlite_orm
#
#  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 27.08.2022.
#  Copyright © 2022-2023 Dmitrii Torkhov. All rights reserved.
#

project(tests)

#

function(add name)
    add_executable(${name} ${name}.cpp)

    target_link_libraries(${name} sqlite_orm)

    set_target_properties(${name} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)

    add_test(NAME ${name} COMMAND ${name})
endfunction()

#

add("test_insert")
add("test_empty_string")
add("test_multiple_commands")
add("test_input")
add("test_getters")
add("test_locks")
add("test_commands")
add("test_error")
add("test_ensure_fields")
