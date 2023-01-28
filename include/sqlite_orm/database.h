//
//  database.h
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 21.05.2019.
//  Copyright © 2019-2022 Dmitrii Torkhov. All rights reserved.
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

        explicit database(sqlite3 *const db) : base(db) {

        }

    public:

        // Mutex

        operator std::mutex &() const {
            return mutex_holder::get();
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

        database &operator<<(const char c) {
            base::m_query << c;

            if (c == ';') {
                switch (m_active_command) {
                    case CREATE_TABLE_IF_NOT_EXISTS:
                    case CREATE_INDEX_IF_NOT_EXISTS:
                    case INSERT_OR_REPLACE_INTO:
                    case UPDATE:
                    case DELETE:
                        base::exec();
                        break;
                    default:
                        break;
                }

                m_active_command = NONE;
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

        database &operator<<(const command c) {
            switch (c) {
                case sqlite::SELECT:
                    base::m_query << "SELECT ";
                    break;
                case sqlite::CREATE_TABLE_IF_NOT_EXISTS:
                    base::m_query << "CREATE TABLE IF NOT EXISTS ";
                    m_active_command = CREATE_TABLE_IF_NOT_EXISTS;
                    break;
                case sqlite::CREATE_INDEX_IF_NOT_EXISTS:
                    base::m_query << "CREATE INDEX IF NOT EXISTS ";
                    m_active_command = CREATE_INDEX_IF_NOT_EXISTS;
                    break;
                case sqlite::INSERT_OR_REPLACE_INTO:
                    base::m_query << "INSERT OR REPLACE INTO ";
                    m_active_command = INSERT_OR_REPLACE_INTO;
                    break;
                case sqlite::UPDATE:
                    base::m_query << "UPDATE ";
                    m_active_command = UPDATE;
                    break;
                case sqlite::DELETE:
                    base::m_query << "DELETE ";
                    m_active_command = DELETE;
                    break;
                case sqlite::SET:
                    base::m_query << "SET ";
                    break;
                case sqlite::COUNT:
                    base::m_query << "COUNT(*) ";
                    break;
                case sqlite::FROM:
                    base::m_query << "FROM ";
                    break;
                case sqlite::WHERE:
                    base::m_query << "WHERE ";
                    break;
                case sqlite::ORDER_BY:
                    base::m_query << "ORDER BY ";
                    break;
                case sqlite::ALL:
                    if (m_active_command == CREATE_TABLE_IF_NOT_EXISTS) {
                        base::m_query << base::m_all_fields_with_types << " ";
                    } else {
                        base::m_query << base::m_all_fields << " ";
                    }
                    break;
                case sqlite::VALUES:
                    base::m_query << "VALUES ";
                    break;
                case sqlite::BETWEEN:
                    base::m_query << "BETWEEN ";
                    break;
                case sqlite::AND:
                    base::m_query << "AND ";
                    break;
                case sqlite::OR:
                    base::m_query << "OR ";
                    break;
                case sqlite::IN:
                    base::m_query << "IN ";
                    break;
                case sqlite::ON:
                    base::m_query << "ON ";
                    break;
                case sqlite::EQUALS:
                    base::m_query << '=';
                    break;
                case sqlite::NOT_EQUALS:
                    base::m_query << "!=";
                    break;
                case sqlite::EMPTY_STRING:
                    base::m_query << "''";
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

        database &operator<<(const bool value) {
            base::m_query << (value ? 1 : 0) << " ";

            return *this;
        }

        database &operator<<(const int value) {
            base::m_query << value << " ";

            return *this;
        }

        database &operator<<(const std::shared_ptr<T> object) {
            base::write_values(object);

            return *this;
        }

        database &operator<<(const std::vector<int> &values) {
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

        database &operator<<(const std::vector<std::string> &values) {
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

        void operator>>(int &value) {
            base::iterate([&](sqlite3_stmt *const statement) {
                value = sqlite3_column_int(statement, 0);
            });
        }

        void operator>>(std::unordered_map<int, std::shared_ptr<T>> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                const auto key = base::get(base::m_int_pointer, statement);
                container[key] = base::make_object(statement);
            });
        }

        void operator>>(std::unordered_map<std::string, std::shared_ptr<T>> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                const auto key = base::get(base::m_string_pointer, statement);
                container[key] = base::make_object(statement);
            });
        }

        void operator>>(std::unordered_set<int> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                container.insert(sqlite3_column_int(statement, 0));
            });
        }

        void operator>>(std::unordered_set<std::string> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                auto text = (char *) sqlite3_column_text(statement, 0);
                if (text) {
                    container.insert(text);
                }
            });
        }

        void operator>>(std::vector<int> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                container.push_back(sqlite3_column_int(statement, 0));
            });
        }

        void operator>>(std::vector<std::shared_ptr<T>> &container) {
            base::iterate([&](sqlite3_stmt *const statement) {
                container.push_back(base::make_object(statement));
            });
        }

    private:

        command m_active_command = NONE;

    private:

        static std::shared_ptr<sqlite::database<T>>
        open(const std::string &path, int flags, const std::string &vfs_name) {
            const auto c_vfs_name = vfs_name.empty() ? nullptr : vfs_name.c_str();
            const auto db = db_cache::open_db(path, flags, c_vfs_name);
            return std::make_shared<sqlite::database<T>>(db);
        }

    };

}