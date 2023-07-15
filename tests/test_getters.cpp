//
//  test_getters.cpp
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 02.07.2023.
//  Copyright © 2023 Dmitrii Torkhov. All rights reserved.
//

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <sqlite_orm/database.h>

using namespace sqlite;

namespace {

    struct data {
        int id;
        int number;
        std::string data;
    };

    namespace constant {

        const char *table = "getters";
        const size_t count = 2;

    }

    namespace sample {

        const int id_1 = 1;
        const int id_2 = 2;

        const int number_1 = 10;
        const int number_2 = 20;

        const char *data_1 = "data_1";
        const char *data_2 = "data_2";

    }

}

std::shared_ptr<sqlite::database<data>> create_db_with_data() {
    auto db = sqlite::database<data>::open("test.db");
    db->set_fields({{&data::id,     "id"},
                    {&data::number, "number"},
                    {&data::data,   "data"}});

    *db << CREATE_TABLE_IF_NOT_EXISTS << constant::table << '(' << ALL << ')' << ';';
    *db << DELETE << FROM << constant::table << ';';

    const auto data_1 = std::make_shared<data>(data{0, sample::number_1, sample::data_1});
    *db << INSERT_OR_REPLACE_INTO << constant::table << '(' << ALL << ')'
        << VALUES << '(' << "null" << data_1 << ')' << ';';

    const auto data_2 = std::make_shared<data>(data{0, sample::number_2, sample::data_2});
    *db << INSERT_OR_REPLACE_INTO << constant::table << '(' << ALL << ')'
        << VALUES << '(' << "null" << data_2 << ')' << ';';

    return db;
}

int main() {

    // Int

    {
        auto db = create_db_with_data();
        *db << SELECT << COUNT << FROM << constant::table;

        int count;
        *db >> count;
        assert(count == constant::count);
    }

    // Int, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << COUNT << FROM << constant::table;

        const int count = *db;
        assert(count == constant::count);
    }

    // Size_t

    {
        auto db = create_db_with_data();
        *db << SELECT << COUNT << FROM << constant::table;

        size_t count;
        *db >> count;
        assert(count == constant::count);
    }

    // Size_t, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << COUNT << FROM << constant::table;

        const size_t count = *db;
        assert(count == constant::count);
    }

    // Map of int and record, default field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_map<int, std::shared_ptr<data>> records;
        *db >> records;
        assert(records.size() == constant::count);
        assert(records.begin()->first == records.begin()->second->id);
    }

    // Map of int and record, default field, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        const std::unordered_map<int, std::shared_ptr<data>> records = *db;
        assert(records.size() == constant::count);
        assert(records.begin()->first == records.begin()->second->id);
    }

    // Map of int and record, specific field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_map<int, std::shared_ptr<data>> records;
        *db >> &data::number >> records;
        assert(records.size() == constant::count);
        assert(records.begin()->first == records.begin()->second->number);
    }

    // Map of uint and record, specific field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_map<uint32_t, std::shared_ptr<data>> records;
        *db >> &data::number >> records;
        assert(records.size() == constant::count);
        assert(records.begin()->first == records.begin()->second->number);
    }

    // Map of string and record, default field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_map<std::string, std::shared_ptr<data>> records;
        *db >> records;
        assert(records.size() == constant::count);
        assert(records.begin()->first == std::to_string(records.begin()->second->id));
    }

    // Map of string and record, default field, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        const std::unordered_map<std::string, std::shared_ptr<data>> records = *db;
        assert(records.size() == constant::count);
        assert(records.begin()->first == std::to_string(records.begin()->second->id));
    }

    // Map of string and record, specific field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_map<std::string, std::shared_ptr<data>> records;
        *db >> &data::data >> records;
        assert(records.size() == constant::count);
        assert(records.begin()->first == records.begin()->second->data);
    }

    // Set of int, default field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_set<int> records;
        *db >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::id_1 || *records.begin() == sample::id_2);
    }

    // Set of int, default field, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        const std::unordered_set<int> records = *db;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::id_1 || *records.begin() == sample::id_2);
    }

    // Set of int, specific field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_set<int> records;
        *db >> &data::number >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::number_1 || *records.begin() == sample::number_2);
    }

    // Set of uint, specific field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_set<uint32_t> records;
        *db >> &data::number >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::number_1 || *records.begin() == sample::number_2);
    }

    // Set of string, default field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_set<std::string> records;
        *db >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == std::to_string(sample::id_1) || *records.begin() == std::to_string(sample::id_2));
    }

    // Set of string, default field, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        const std::unordered_set<std::string> records = *db;
        assert(records.size() == constant::count);
        assert(*records.begin() == std::to_string(sample::id_1) || *records.begin() == std::to_string(sample::id_2));
    }

    // Set of string, specific field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_set<std::string> records;
        *db >> &data::data >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::data_1 || *records.begin() == sample::data_2);
    }

    // Set of object

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::unordered_set<std::shared_ptr<data>> records;
        *db >> records;
        assert(records.size() == constant::count);
        assert((*records.begin())->data == sample::data_1 || (*records.begin())->data == sample::data_2);
    }

    // Set of object, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        const std::unordered_set<std::shared_ptr<data>> records = *db;
        assert(records.size() == constant::count);
        assert((*records.begin())->data == sample::data_1 || (*records.begin())->data == sample::data_2);
    }

    // Vector of int, default field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::vector<int> records;
        *db >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::id_1);
    }

    // Vector of int, default field, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        const std::vector<int> records = *db;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::id_1);
    }

    // Vector of int, specific field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::vector<int> records;
        *db >> &data::number >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::number_1);
    }

    // Vector of uint, specific field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::vector<uint32_t> records;
        *db >> &data::number >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::number_1);
    }

    // Vector of string, default field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::vector<std::string> records;
        *db >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == std::to_string(sample::id_1));
    }

    // Vector of string, default field, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        const std::vector<std::string> records = *db;
        assert(records.size() == constant::count);
        assert(*records.begin() == std::to_string(sample::id_1));
    }

    // Vector of string, specific field

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::vector<std::string> records;
        *db >> &data::data >> records;
        assert(records.size() == constant::count);
        assert(*records.begin() == sample::data_1);
    }

    // Vector of object

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        std::vector<std::shared_ptr<data>> records;
        *db >> records;
        assert(records.size() == constant::count);
        assert((*records.begin())->data == sample::data_1);
    }

    // Vector of object, assignment

    {
        auto db = create_db_with_data();
        *db << SELECT << ALL << FROM << constant::table;

        const std::vector<std::shared_ptr<data>> records = *db;
        assert(records.size() == constant::count);
        assert((*records.begin())->data == sample::data_1);
    }

    // Different target fields

    {
        auto db = create_db_with_data();

        *db << SELECT << ALL << FROM << constant::table;

        std::vector<int> number_records;
        *db >> &data::number >> number_records;
        assert(number_records.front() == sample::number_1);

        //

        *db << SELECT << ALL << FROM << constant::table;

        std::vector<int> id_records;
        *db >> id_records;
        assert(id_records.front() == sample::id_1);
    }

    return 0;
}
