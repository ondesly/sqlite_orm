//
//  database.h
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 21.05.2019.
//  Copyright © 2019-2023 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "base_database.h"
#include "commands.h"
#include "column.h"
#include "sqlite3.h"

namespace sqlite {

    class mutex_holder {
    public:

        static std::mutex &get() {
            static std::mutex mutex;
            return mutex;
        }

    };

    //

    class db_cache {
    public:

        static sqlite3 *open_db(const std::string &path, int flags, const char *vfs) {
            static std::unordered_map<std::string, sqlite3 *> dbs;

            sqlite3 *db = dbs[path];

            if (!db) {
                sqlite3_open_v2(path.c_str(), &db, flags, vfs);
                dbs[path] = db;
            }

            return db;
        }

    };

    //

    using lock = std::lock_guard<std::mutex>;

    //

    template<class T>
    class database : public base_database<T> {

        using base = base_database<T>;

    public:

        static std::shared_ptr<sqlite::database<T>>
        open_read_only(const std::string &path, const std::string &vfs_name = {}) {
            return open(path, SQLITE_OPEN_READONLY, vfs_name);
        }

        static std::shared_ptr<sqlite::database<T>>
        open(const std::string &path, const std::string &vfs_name = {}) {
            return open(path, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, vfs_name);
        }

    public:

        explicit database(sqlite3 *db) : base(db) {}

    public:

        // Mutex

        operator std::mutex &() const {
            return mutex_holder::get();
        }

        operator std::lock_guard<std::mutex>() {
            return std::lock_guard<std::mutex>(mutex_holder::get());
        }

        // Pointers

        database &operator<<(int T::* const pointer) {
            const auto it = base::find(pointer);
            base::m_query << (*it).get_name() << " ";

            return *this;
        }

        database &operator<<(std::string T::* const pointer) {
            const auto it = base::find(pointer);
            base::m_query << (*it).get_name() << " ";

            return *this;
        }

        // Char

        database &operator<<(char c) {
            base::m_query << c;

            if (c == ';') {
                switch (m_active_command) {
                    case command::CREATE_TABLE_IF_NOT_EXISTS:
                    case command::ALTER_TABLE:
                    case command::CREATE_INDEX_IF_NOT_EXISTS:
                    case command::INSERT_OR_REPLACE_INTO:
                    case command::UPDATE:
                    case command::DELETE:
                        base::exec();
                        break;
                    default:
                        break;
                }

                m_active_command = command::NONE;
            }

            return *this;
        }

        database &operator<<(const char *s) {
            base::m_query << s << " ";

            return *this;
        }

        // String

        database &operator<<(const std::string &s) {
            base::m_query << "'" << s << "'" << " ";

            return *this;
        }

        // Commands

        database &operator<<(command c) {
            switch (c) {
                case command::PRAGMA:
                    base::m_query << "PRAGMA ";
                    break;
                case command::TABLE_INFO:
                    base::m_query << "table_info ";
                    break;
                case command::SELECT:
                    base::m_query << "SELECT ";
                    break;
                case command::CREATE_TABLE_IF_NOT_EXISTS:
                    base::m_query << "CREATE TABLE IF NOT EXISTS ";
                    m_active_command = command::CREATE_TABLE_IF_NOT_EXISTS;
                    break;
                case command::ALTER_TABLE:
                    base::m_query << "ALTER TABLE ";
                    m_active_command = command::ALTER_TABLE;
                    break;
                case command::CREATE_INDEX_IF_NOT_EXISTS:
                    base::m_query << "CREATE INDEX IF NOT EXISTS ";
                    m_active_command = command::CREATE_INDEX_IF_NOT_EXISTS;
                    break;
                case command::INSERT_OR_REPLACE_INTO:
                    base::m_query << "INSERT OR REPLACE INTO ";
                    m_active_command = command::INSERT_OR_REPLACE_INTO;
                    break;
                case command::UPDATE:
                    base::m_query << "UPDATE ";
                    m_active_command = command::UPDATE;
                    break;
                case command::DELETE:
                    base::m_query << "DELETE ";
                    m_active_command = command::DELETE;
                    break;
                case command::ADD_COLUMN:
                    base::m_query << "ADD COLUMN ";
                    break;
                case command::SET:
                    base::m_query << "SET ";
                    break;
                case command::COUNT:
                    base::m_query << "COUNT(*) ";
                    break;
                case command::FROM:
                    base::m_query << "FROM ";
                    break;
                case command::WHERE:
                    base::m_query << "WHERE ";
                    break;
                case command::ORDER_BY:
                    base::m_query << "ORDER BY ";
                    break;
                case command::ALL:
                    if (m_active_command == command::CREATE_TABLE_IF_NOT_EXISTS) {
                        base::m_query << base::m_all_fields_with_types << " ";
                    } else {
                        base::m_query << base::m_all_fields << " ";
                    }
                    break;
                case command::VALUES:
                    base::m_query << "VALUES ";
                    break;
                case command::BETWEEN:
                    base::m_query << "BETWEEN ";
                    break;
                case command::AND:
                    base::m_query << "AND ";
                    break;
                case command::OR:
                    base::m_query << "OR ";
                    break;
                case command::IN:
                    base::m_query << "IN ";
                    break;
                case command::ON:
                    base::m_query << "ON ";
                    break;
                case command::EQUALS:
                    base::m_query << '=';
                    break;
                case command::NOT_EQUALS:
                    base::m_query << "!=";
                    break;
                case command::EMPTY_STRING:
                    base::m_query << "''";
                    break;
                case command::ASC:
                    base::m_query << "ASC ";
                    break;
                case command::DESC:
                    base::m_query << "DESC ";
                    break;
                case command::LIMIT:
                    base::m_query << "LIMIT ";
                    break;
                default:
                    break;
            }

            return *this;
        }

        database &operator<<(const std::vector<command> &v) {
            for (auto command: v) {
                operator<<(command);
            }

            return *this;
        }

        // Values

        database &operator<<(bool value) {
            base::m_query << (value ? 1 : 0) << " ";

            return *this;
        }

        database &operator<<(int32_t value) {
            base::m_query << value << " ";

            return *this;
        }

        database &operator<<(uint32_t value) {
            base::m_query << value << " ";

            return *this;
        }

        database &operator<<(const std::shared_ptr<T> &object) {
            base::write_values(object);

            return *this;
        }

        template<class V>
        database &operator<<(const std::vector<V> &values) {
            base::m_query << "(";
            for (int i = 0; i < values.size(); ++i) {
                base::m_query << values[i];

                if (i != values.size() - 1) {
                    base::m_query << ", ";
                }
            }
            base::m_query << ") ";

            return *this;
        }

        database &operator<<(const std::vector<uint8_t> &values) {
            base::m_query << "(";
            for (int i = 0; i < values.size(); ++i) {
                base::m_query << std::to_string(values[i]);

                if (i != values.size() - 1) {
                    base::m_query << ", ";
                }
            }
            base::m_query << ") ";

            return *this;
        }

        database &operator<<(const std::unordered_set<int> &values) {
            base::m_query << "(";
            size_t counter = 0;
            for (int value: values) {
                base::m_query << value;

                if (counter != values.size() - 1) {
                    base::m_query << ", ";
                }

                ++counter;
            }
            base::m_query << ") ";

            return *this;
        }

        // Target

        database &operator>>(int T::* const pointer) {
            base::m_int_pointer = pointer;

            return *this;
        }

        database &operator>>(std::string T::* const pointer) {
            base::m_string_pointer = pointer;

            return *this;
        }

        // Getters

        template<class V>
        operator V() {
            V value = 0;
            *this >> value;
            return value;
        }

        template<class V>
        void operator>>(V &value) {
            base::iterate([&](sqlite3_stmt *const statement) {
                value = sqlite3_column_int(statement, 0);
            });
        }

        template<class V>
        operator std::unordered_map<V, std::shared_ptr<T>>() {
            std::unordered_map<V, std::shared_ptr<T>> map;
            *this >> map;
            return map;
        }

        template<class V>
        void operator>>(std::unordered_map<V, std::shared_ptr<T>> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                const auto key = base::get(base::m_int_pointer, statement);
                container.emplace(key, base::make_object(statement));
            });
        }

        operator std::unordered_map<std::string, std::shared_ptr<T>>() {
            std::unordered_map<std::string, std::shared_ptr<T>> map;
            *this >> map;
            return map;
        }

        void operator>>(std::unordered_map<std::string, std::shared_ptr<T>> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                const auto key = base::get(base::m_string_pointer, statement);
                container.emplace(key, base::make_object(statement));
            });
        }

        template<class V>
        operator std::unordered_set<V>() {
            std::unordered_set<V> set;
            *this >> set;
            return set;
        }

        template<class V>
        void operator>>(std::unordered_set<V> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                const auto value = base::get(base::m_int_pointer, statement);
                container.emplace(value);
            });
        }

        operator std::unordered_set<std::string>() {
            std::unordered_set<std::string> set;
            *this >> set;
            return set;
        }

        void operator>>(std::unordered_set<std::string> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                const auto value = base::get(base::m_string_pointer, statement);
                container.emplace(value);
            });
        }

        operator std::unordered_set<std::shared_ptr<T>>() {
            std::unordered_set<std::shared_ptr<T>> set;
            *this >> set;
            return set;
        }

        void operator>>(std::unordered_set<std::shared_ptr<T>> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                container.emplace(base::make_object(statement));
            });
        }

        template<class V>
        operator std::vector<V>() {
            std::vector<V> v;
            *this >> v;
            return v;
        }

        template<class V>
        void operator>>(std::vector<V> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                const auto value = base::get(base::m_int_pointer, statement);
                container.emplace_back(value);
            });
        }

        operator std::vector<std::string>() {
            std::vector<std::string> v;
            *this >> v;
            return v;
        }

        void operator>>(std::vector<std::string> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                const auto value = base::get(base::m_string_pointer, statement);
                container.emplace_back(value);
            });
        }

        operator std::vector<std::shared_ptr<T>>() {
            std::vector<std::shared_ptr<T>> v;
            *this >> v;
            return v;
        }

        void operator>>(std::vector<std::shared_ptr<T>> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                container.emplace_back(base::make_object(statement));
            });
        }

        void ensure_fields(const std::string &table) {
            *this << PRAGMA << TABLE_INFO << '(' << table << ')' << ';';

            std::unordered_set<std::string> current_fields;
            const bool success = base::iterate([&](sqlite3_stmt *const statement) {
                const auto column_name = sqlite3_column_text(statement, 1);
                current_fields.emplace(reinterpret_cast<const std::string::value_type *>(column_name));
            });
            if (!success) {
                return;
            }

            for (const auto &field: base::m_fields) {
                if (!current_fields.count(field.get_name())) {
                    add_field(table, field);
                }
            }
        }

        void add_field(const std::string &table, const sqlite::column<T> &field) {
            *this << ALTER_TABLE << table << ADD_COLUMN << field.get_name() << base::to_string(field.get_type()) << ';';
        }

    private:

        command m_active_command = command::NONE;

    private:

        static std::shared_ptr<sqlite::database<T>>
        open(const std::string &path, int flags, const std::string &vfs_name) {
            const auto c_vfs_name = vfs_name.empty() ? nullptr : vfs_name.c_str();
            const auto db = db_cache::open_db(path, flags, c_vfs_name);
            return std::make_shared<sqlite::database<T>>(db);
        }

    };

}