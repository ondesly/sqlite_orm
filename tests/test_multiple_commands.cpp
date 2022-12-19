//
//  test_multiple_commands.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 19.12.2022.
//  Copyright © 2022 Dmitrii Torkhov. All rights reserved.
//

#include <sqlite_orm/database.h>

using namespace sqlite;

int main() {
    struct data {
        int id;
        std::string text;
    };

    auto db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id,   "id"},
                    {&data::text, "text"}});

    // Create table

    const auto table = "test_multiple_commands";
    *db << CREATE_TABLE_IF_NOT_EXISTS << table << '(' << ALL << ')' << ';';

    // Data

    const auto test_data = std::make_shared<data>(data{0, "text"});

    *db << INSERT_OR_REPLACE_INTO << table << '(' << ALL << ')'
        << VALUES << '(' << "null" << test_data << ')' << ';';

    //

    std::vector<command> commands{SELECT, ALL, FROM};

    *db << commands << table << ';';

    std::vector<std::shared_ptr<data>> data;
    *db >> data;

    // Clean up

    *db << DELETE << FROM << table << ';';

    //

    return (data.size() == 1) ? 0 : 1;
}