//
//  test_empty_string.cpp
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
        std::string empty;
        std::string not_empty;
    };

    auto db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id,        "id"},
                    {&data::empty,     "empty"},
                    {&data::not_empty, "not_empty"}});

    // Create table

    const auto table = "test_empty_string";
    *db << CREATE_TABLE_IF_NOT_EXISTS << table << '(' << ALL << ')' << ';';

    // Data

    const auto test_data = std::make_shared<data>(data{0, "", "test"});

    *db << INSERT_OR_REPLACE_INTO << table << '(' << ALL << ')'
        << VALUES << '(' << "null" << test_data << ')' << ';';

    // All

    *db << SELECT << ALL << FROM << table << ';';

    std::vector<std::shared_ptr<data>> all;
    *db >> all;

    // Empty, which is empty

    *db << SELECT << ALL << FROM << table
        << WHERE << &data::empty << EQUALS << EMPTY_STRING
        << ';';

    std::vector<std::shared_ptr<data>> empty_empty;
    *db >> empty_empty;

    // Empty, which is not empty

    *db << SELECT << ALL << FROM << table
        << WHERE << &data::empty << NOT_EQUALS << EMPTY_STRING
        << ';';

    std::vector<std::shared_ptr<data>> empty_not_empty;
    *db >> empty_not_empty;

    // Not empty, which is not empty

    *db << SELECT << ALL << FROM << table
        << WHERE << &data::not_empty << NOT_EQUALS << EMPTY_STRING
        << ';';

    std::vector<std::shared_ptr<data>> not_empty_not_empty;
    *db >> not_empty_not_empty;

    // Not empty, which is empty

    *db << SELECT << ALL << FROM << table
        << WHERE << &data::not_empty << EQUALS << EMPTY_STRING
        << ';';

    std::vector<std::shared_ptr<data>> not_empty_empty;
    *db >> not_empty_empty;

    // CLean up

    *db << DELETE << FROM << table << ';';

    //

    return (all.size() == 1 &&
            empty_empty.size() == 1 && empty_not_empty.empty() &&
            not_empty_not_empty.size() == 1 && not_empty_empty.empty()) ? 0 : 1;
}