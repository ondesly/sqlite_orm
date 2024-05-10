//
//  test_commands.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 14.07.2023.
//  Copyright © 2023 Dmitrii Torkhov. All rights reserved.
//

#include <cassert>
#include <string>

#include <sqlite_orm/database.h>

using namespace sqlite;

namespace {

    struct data {
        int id;
        int number;
        std::string data;
    };

    namespace constant {

        const char *table = "commands";

    }

    namespace sample {

        const int id_1 = 1;
        const int id_2 = 2;
        const int id_3 = 3;

        const int number_1 = 10;
        const int number_2 = 20;
        const int number_3 = 30;

        const char *data_1 = "data_1";
        const char *data_2 = "data_2";
        const char *data_3 = "data_3";

    }

}

std::shared_ptr<sqlite::database<data>> create_db_with_data() {
    auto db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id,     "id"},
                    {&data::number, "number"},
                    {&data::data,   "data"}});

    *db << CREATE_TABLE_IF_NOT_EXISTS << constant::table << '(' << ALL << ')' << ';';
    *db << DELETE << FROM << constant::table << ';';

    *db << INSERT_OR_REPLACE_INTO << constant::table << '(' << ALL << ')' << VALUES;

    const auto data_2 = std::make_shared<data>(data{0, sample::number_2, sample::data_2});
    *db << '(' << "null" << data_2 << ')' << ',';

    const auto data_1 = std::make_shared<data>(data{0, sample::number_1, sample::data_1});
    *db << '(' << "null" << data_1 << ')' << ',';

    const auto data_3 = std::make_shared<data>(data{0, sample::number_3, sample::data_3});
    *db << '(' << "null" << data_3 << ')' << ';';

    return db;
}

int main() {

    // SELECT, ALL, FROM

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        const std::vector<std::shared_ptr<data>> records = *db;
        assert(records.size() == 3);
    }

    // ASC

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table << ORDER_BY << &data::number << ASC;

        const std::vector<std::shared_ptr<data>> records = *db;
        assert(records.size() == 3);
        assert(records.front()->number == sample::number_1);
    }

    // DESC

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table << ORDER_BY << &data::number << DESC;

        const std::vector<std::shared_ptr<data>> records = *db;
        assert(records.size() == 3);
        assert(records.front()->number == sample::number_3);
    }

    // LIMIT

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table << LIMIT << 1;

        const std::vector<std::shared_ptr<data>> records = *db;
        assert(records.size() == 1);
    }

    return 0;
}
