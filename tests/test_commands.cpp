//
//  test_commands.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 29.06.2023.
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

    namespace sample {

        const char *string_1 = "string_1";
        const char *string_2 = "string_2";

        const int int_1 = 1;
        const int int_2 = 2;

        const uint32_t uint32_1 = 1;
        const uint32_t uint32_2 = 2;

    }

    namespace query {

        const char *in_vector_of_string = "data IN (string_1, string_2) ";
        const char *in_vector_of_int = "data IN (1, 2) ";

    }

}

std::shared_ptr<sqlite::database<data>> create_db() {
    auto db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id,   "id"},
                    {&data::data, "data"}});
    return db;
}

int main() {

    // Vector of string

    {
        auto db = create_db();
        *db << &data::data << IN << std::vector<std::string>{sample::string_1, sample::string_2};
        assert(db->get_query() == query::in_vector_of_string);
    }

    // Vector of int

    {
        auto db = create_db();
        *db << &data::data << IN << std::vector<int>{sample::int_1, sample::int_2};
        const auto q = db->get_query();
        assert(db->get_query() == query::in_vector_of_int);
    }

    // Vector of uint32

    {
        auto db = create_db();
        *db << &data::data << IN << std::vector<uint32_t>{sample::uint32_1, sample::uint32_2};
        const auto q = db->get_query();
        assert(db->get_query() == query::in_vector_of_int);
    }

    return 0;
}
