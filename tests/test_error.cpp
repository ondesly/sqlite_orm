//
//  test_error.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 10.05.2024.
//  Copyright © 2024 Dmitrii Torkhov. All rights reserved.
//

#include <cassert>

#include <sqlite_orm/database.h>

using namespace sqlite;

namespace {
    constexpr size_t max_error_count = 10;
    const char *no_such_table_error = "no such table: wrong_table";
}

int main() {
    struct data {
        int id;
        std::string text;
    };

    auto db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id,   "id"},
                    {&data::text, "text"}});

    const auto table = "test_error";
    const auto wrong_table = "wrong_table";
    const auto test_data = std::make_shared<data>(data{0, "text"});

    // Success

    *db << CREATE_TABLE_IF_NOT_EXISTS << table << '(' << ALL << ')' << ';';
    *db << INSERT_OR_REPLACE_INTO << table << '(' << ALL << ')'
        << VALUES << '(' << "null" << test_data << ')' << ';';

    auto errors = db->get_last_errors();
    assert(errors.empty());

    // Error

    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << ';';
    *db << INSERT_OR_REPLACE_INTO << wrong_table << '(' << ALL << ')'
        << VALUES << '(' << "null" << test_data << ')' << ';';

    errors = db->get_last_errors();
    assert(errors.size() == max_error_count);
    assert(errors.front() == no_such_table_error);

    return 0;
}