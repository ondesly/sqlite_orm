//
// Created by ondesly on 2019-05-21.
// Copyright (c) 2019 ondesly. All rights reserved.
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
#include "column.h"
#include "sqlite3.h"

namespace sqlite {

    typedef unsigned short command;

    static const command NONE = 0;
    static const command SELECT = NONE + 1;
    static const command CREATE_TABLE_IF_NOT_EXISTS = SELECT + 1;
    static const command CREATE_INDEX_IF_NOT_EXISTS = CREATE_TABLE_IF_NOT_EXISTS + 1;
    static const command INSERT_OR_REPLACE_INTO = CREATE_INDEX_IF_NOT_EXISTS + 1;
    static const command UPDATE = INSERT_OR_REPLACE_INTO + 1;
    static const command DELETE = UPDATE + 1;
    static const command SET = DELETE + 1;
    static const command COUNT = SET + 1;
    static const command FROM = COUNT + 1;
    static const command WHERE = FROM + 1;
    static const command ORDER_BY = WHERE + 1;
    static const command ALL = ORDER_BY + 1;
    static const command VALUES = ALL + 1;
    static const command BETWEEN = VALUES + 1;
    static const command AND = BETWEEN + 1;
    static const command OR = AND + 1;
    static const command IN = OR + 1;
    static const command ON = IN + 1;

    template<class T>
    class database : public base_database<T> {

        using base = base_database<T>;

    public:

        static std::shared_ptr<sqlite::database<T>> create(sqlite3 *const db) {
            return std::make_shared<sqlite::database<T>>(db);
        }

    public:

        database(sqlite3 *const db) : base(db) {

        }

    public:

        // Locks

        void lock() {
            m_mutex.lock();
        }

        void unlock() {
            m_mutex.unlock();
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
                default:
                    break;
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
            for (int value : values) {
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

        std::mutex m_mutex;

    };

}