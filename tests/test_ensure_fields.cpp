//
//  test_ensure_fields.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 12.05.2024.
//  Copyright © 2024 Dmitrii Torkhov. All rights reserved.
//

#include <cassert>

#include <sqlite_orm/database.h>

using namespace sqlite;

namespace {
    const auto db_name = "test.db";
    const auto table = "test_ensure_fields";
}

int main() {
    std::remove(db_name);

    struct data {
        int id;
        std::string text;
        std::string new_text;
    };

    auto db = sqlite::database<data>::open(db_name);
    db->set_fields({{&data::id,   "id"},
                    {&data::text, "text"}});

    *db << CREATE_TABLE_IF_NOT_EXISTS << table << '(' << ALL << ')' << ';';

    // 2 columns

    {
        const auto test_data = std::make_shared<data>(data{0, "text"});
        *db << INSERT_OR_REPLACE_INTO << table << '(' << ALL << ')'
            << VALUES << '(' << "null" << test_data << ')' << ';';

        const auto &errors = db->get_last_errors();
        assert(errors.empty());

        *db << SELECT << COUNT << FROM << table;
        const int count = *db;
        assert(count == 1);
    }

    db->set_fields({{&data::id,   "id"},
                    {&data::text, "text"},
                    {&data::new_text, "new_text"}});
    db->ensure_fields(table);

    // 3 columns

    {
        const auto test_data = std::make_shared<data>(data{0, "text", "new_text"});
        *db << INSERT_OR_REPLACE_INTO << table << '(' << ALL << ')'
            << VALUES << '(' << "null" << test_data << ')' << ';';

        const auto &errors = db->get_last_errors();
        assert(errors.empty());

        *db << SELECT << COUNT << FROM << table;
        const int count = *db;
        assert(count == 2);
    }

    return 0;
}