//
//  test_locks.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 02.07.2023.
//  Copyright © 2023 Dmitrii Torkhov. All rights reserved.
//

#include <string>

#include <sqlite_orm/database.h>

using namespace sqlite;

namespace {

    struct data {
        int id;
        std::string data;
    };

    namespace constant {

        const char *table = "getters";
        const size_t count = 2;

    }

}

std::shared_ptr<sqlite::database<data>> create_db_with_data() {
    auto db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id,   "id"},
                    {&data::data, "data"}});

    *db << CREATE_TABLE_IF_NOT_EXISTS << constant::table << '(' << ALL << ')' << ';';
    *db << DELETE << FROM << constant::table << ';';

    const auto data_1 = std::make_shared<data>(data{0, {}});
    *db << INSERT_OR_REPLACE_INTO << constant::table << '(' << ALL << ')'
        << VALUES << '(' << "null" << data_1 << ')' << ';';

    const auto data_2 = std::make_shared<data>(data{0, {}});
    *db << INSERT_OR_REPLACE_INTO << constant::table << '(' << ALL << ')'
        << VALUES << '(' << "null" << data_2 << ')' << ';';

    return db;
}

int main() {

    // Mutex

    {
        auto db = create_db_with_data();
        std::mutex &mutex = *db;
        mutex.lock();

        *db << SELECT << COUNT << FROM << constant::table;
        const size_t count = *db;
        assert(count == constant::count);

        mutex.unlock();
    }

    {
        auto db = create_db_with_data();
        const sqlite::lock lock(*db);

        *db << SELECT << COUNT << FROM << constant::table;
        const size_t count = *db;
        assert(count == constant::count);
    }

    {
        auto db = create_db_with_data();

        *db << SELECT << COUNT << FROM << constant::table;
        const size_t count = *db;
        assert(count == constant::count);
    }

    return 0;
}
