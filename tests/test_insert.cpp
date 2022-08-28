//
//  test_insert.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 27.08.2022.
//  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
//

#include <sqlite_orm/database.h>

using namespace sqlite;

int main() {
    sqlite3 *sqlite_db = nullptr;
    sqlite3_open("test.db", &sqlite_db);

    struct data {
        int id;
        int number;
        std::string text;
    };

    auto db = sqlite::database<data>::create(sqlite_db);
    db->set_fields({{&data::id,     "id"},
                    {&data::number, "number"},
                    {&data::text,   "text"}});

    // Create table

    const auto table = "test_table";
    *db << CREATE_TABLE_IF_NOT_EXISTS << table << '(' << ALL << ')' << ';';

    // Data

    const int number = 5;
    const std::string text = "test_text";
    const auto test_data = std::make_shared<data>(data{0, number, text});

    *db << INSERT_OR_REPLACE_INTO << table << '(' << ALL << ')'
        << VALUES << '(' << "null" << test_data << ')' << ';';

    // Select

    *db << SELECT << ALL << FROM << table << ';';

    std::vector<std::shared_ptr<data>> data;
    *db >> data;

    // Remove all

    *db << DELETE << FROM << table << ';';

    //

    return (data.size() == 1 && data.front()->number == number && data.front()->text == text) ? 0 : 1;
}