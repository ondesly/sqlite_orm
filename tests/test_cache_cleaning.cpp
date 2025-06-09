//
//  test_cache_cleaning.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 09.06.2025.
//  Copyright © 2025 Dmitrii Torkhov. All rights reserved.
//

#include <cassert>

#include <sqlite_orm/database.h>

using namespace sqlite;

namespace {
    namespace constant {
        constexpr auto table = "test_cache_cleaning";
        constexpr auto count = 1;
    } // namespace constant
} // namespace

int main() {
    struct data {
        int id;
        std::string text;
    };

    auto db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id, "id"}, {&data::text, "text"}});

    const auto test_data = std::make_shared<data>(data{0, "text"});

    //

    {
        *db << CREATE_TABLE_IF_NOT_EXISTS << constant::table << '(' << ALL << ')' << ';';
        *db << DELETE << FROM << constant::table << ';';
        *db << INSERT_OR_REPLACE_INTO << constant::table << '(' << ALL << ')' << VALUES << '(' << "null" << test_data
            << ')' << ';';

        const auto errors = db->get_last_errors();
        assert(errors.empty());
    }

    //

    {
        *db << SELECT << COUNT << FROM << constant::table;

        size_t count = 0;
        *db >> count;
        assert(count == constant::count);
    }

    //

    sqlite::database<data>::clear_cache();

    //

    {
        *db << SELECT << COUNT << FROM << constant::table;

        size_t count = 0;
        *db >> count;
        assert(count == 0);
    }

    //

    db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id, "id"}, {&data::text, "text"}});

    //

    {
        *db << SELECT << COUNT << FROM << constant::table;

        size_t count = 0;
        *db >> count;
        assert(count == constant::count);
    }

    // Clean up

    *db << DELETE << FROM << constant::table << ';';

    return 0;
}
