//
//  test_input.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 29.06.2023.
//  Copyright © 2023 Dmitrii Torkhov. All rights reserved.
//

#include <cassert>
#include <string>

#include <sqlite_orm/database.h>

using namespace sqlite;

namespace {

    struct data {
        int id;
        std::string data;
    };

    namespace sample {

        const char *chars_1 = "string_1";
        const char *chars_2 = "string_2";

        const int int_1 = 1;
        const int int_2 = 2;

        const uint8_t uint8_1 = 1;
        const uint8_t uint8_2 = 2;

        const uint32_t uint32_1 = 1;
        const uint32_t uint32_2 = 2;

        const bool bool_1 = true;

        const data object = {0, chars_1};

    }

    namespace query {

        const char *fields = "id data ";
        const char *semicolon = ";";
        const char *chars = "string_1 ";
        const char *string = "'string_1' ";
        const char *command = "SELECT ";
        const char *vector_of_command = "SELECT COUNT(*) ";
        const char *boolean = "1 ";
        const char *integer = "1 ";
        const char *object = ",'string_1'";
        const char *vector_of_string = "(string_1, string_2) ";
        const char *vector_of_int = "(1, 2) ";

    }

}

std::shared_ptr<sqlite::database<data>> create_db() {
    auto db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id,   "id"},
                    {&data::data, "data"}});
    return db;
}

int main() {

    // Fields

    {
        auto db = create_db();
        *db << &data::id;
        *db << &data::data;
        assert(db->get_query() == query::fields);
    }

    // Char

    {
        auto db = create_db();
        *db << ';';
        assert(db->get_query() == query::semicolon);
    }

    // Chars

    {
        auto db = create_db();
        *db << sample::chars_1;
        assert(db->get_query() == query::chars);
    }

    // String

    {
        auto db = create_db();
        *db << std::string(sample::chars_1);
        assert(db->get_query() == query::string);
    }

    // Command

    {
        auto db = create_db();
        *db << SELECT;
        assert(db->get_query() == query::command);
    }

    // Vector of command

    {
        auto db = create_db();
        *db << std::vector<command>{SELECT, COUNT};
        assert(db->get_query() == query::vector_of_command);
    }

    // Bool

    {
        auto db = create_db();
        *db << sample::bool_1;
        assert(db->get_query() == query::boolean);
    }

    // Int

    {
        auto db = create_db();
        *db << sample::int_1;
        assert(db->get_query() == query::integer);
    }

    // Uint8

    {
        auto db = create_db();
        *db << sample::uint8_1;
        assert(db->get_query() == query::integer);
    }

    // Uint32

    {
        auto db = create_db();
        *db << sample::uint32_1;
        assert(db->get_query() == query::integer);
    }

    // Object

    {
        auto db = create_db();
        *db << std::make_shared<data>(sample::object);
        assert(db->get_query() == query::object);
    }

    // Vector of string

    {
        auto db = create_db();
        *db << std::vector<std::string>{sample::chars_1, sample::chars_2};
        assert(db->get_query() == query::vector_of_string);
    }

    // Vector of int

    {
        auto db = create_db();
        *db << std::vector<int>{sample::int_1, sample::int_2};
        assert(db->get_query() == query::vector_of_int);
    }

    // Vector of uint8

    {
        auto db = create_db();
        *db << std::vector<uint8_t>{sample::uint8_1, sample::uint8_2};
        assert(db->get_query() == query::vector_of_int);
    }

    // Vector of uint32

    {
        auto db = create_db();
        *db << std::vector<uint32_t>{sample::uint32_1, sample::uint32_2};
        assert(db->get_query() == query::vector_of_int);
    }

    return 0;
}
