//
//  base_database.h
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 22.05.2019.
//  Copyright © 2019-2023 Dmitrii Torkhov. All rights reserved.
//

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include "column.h"
#include "sqlite3.h"

namespace sqlite {

    template<class T>
    class base_database {
    public:

        explicit base_database(sqlite3 *const db) : m_db(db) {}

    public:

        void set_fields(const std::vector<sqlite::column<T>> &fields) {
            m_fields = fields;

            //

            for (size_t i = 0; i < m_fields.size(); ++i) {
                const auto &f = m_fields[i];
                m_all_fields += f.get_name();
                m_all_fields += ",";
            }
            m_all_fields.pop_back();

            //

            const auto &id = m_fields.front();
            m_all_fields_with_types += id.get_name();
            m_all_fields_with_types += " integer primary key";

            for (size_t i = 1; i < m_fields.size(); ++i) {
                const auto &f = m_fields[i];
                m_all_fields_with_types += ", ";
                m_all_fields_with_types += f.get_name();

                switch (f.get_type()) {
                    case sqlite::column<T>::type::INT:
                        m_all_fields_with_types += " integer";
                        break;
                    case sqlite::column<T>::type::STRING:
                        m_all_fields_with_types += " text";
                        break;
                }
            }
        }

        std::string get_query() const {
            return m_query.str();
        }

    protected:

        sqlite3 *m_db;

        std::stringstream m_query;

        std::vector<sqlite::column<T>> m_fields;
        std::string m_all_fields;
        std::string m_all_fields_with_types;

        int T::*m_int_pointer;
        std::string T::*m_string_pointer;

    protected:

        void iterate(const std::function<void(sqlite3_stmt *)> &fn) {
            sqlite3_stmt *statement;
            if (sqlite3_prepare(m_db, m_query.str().c_str(), -1, &statement, nullptr) == SQLITE_OK) {
                while (sqlite3_step(statement) == SQLITE_ROW) {
                    fn(statement);
                }

                sqlite3_finalize(statement);
            }

            m_query.str("");
        }

        auto find(int T::* const pointer) {
            return std::find_if(m_fields.begin(), m_fields.end(), [pointer](const column<T> &a) {
                return a.equals(pointer);
            });
        }

        auto find(std::string T::* const pointer) {
            return std::find_if(m_fields.begin(), m_fields.end(), [pointer](const column<T> &a) {
                return a.equals(pointer);
            });
        }

        int find_column(int T::* const pointer) {
            const auto it = find(pointer);
            if (it == m_fields.end()) {
                return 0;
            } else {
                return it - m_fields.begin();
            }
        }

        int find_column(std::string T::* const pointer) {
            const auto it = find(pointer);
            if (it == m_fields.end()) {
                return 0;
            } else {
                return it - m_fields.begin();
            }
        }

        int get(int T::* const pointer, sqlite3_stmt *statement) {
            const auto col = find_column(pointer);
            return sqlite3_column_int(statement, col);
        }

        std::string get(std::string T::* const pointer, sqlite3_stmt *statement) {
            const auto col = find_column(pointer);
            const auto text = sqlite3_column_text(statement, col);

            if (text) {
                return reinterpret_cast<const std::string::value_type *>(text);
            } else {
                return "";
            }
        }

        inline int convert(long long l) {
            if (l >> 32 > 0) {
                return int(l / 1000);
            } else {
                return int(l);
            }
        }

        std::shared_ptr<T> make_object(sqlite3_stmt *statement) {
            auto object = std::make_shared<T>();

            for (int i = 0; i < m_fields.size(); ++i) {
                const auto &f = m_fields[i];

                switch (f.get_type()) {
                    case sqlite::column<T>::type::INT: {
                        auto p = f.get_int_pointer();
                        (*object).*p = convert(sqlite3_column_int64(statement, i));
                        break;
                    }
                    case sqlite::column<T>::type::STRING: {
                        auto text = (char *) sqlite3_column_text(statement, i);
                        if (text) {
                            auto p = f.get_string_pointer();
                            (*object).*p = text;
                        }
                        break;
                    }
                }
            }

            return object;
        }

        void write_values(const std::shared_ptr<T> &object) {
            for (size_t i = 1; i < m_fields.size(); ++i) {
                const auto &f = m_fields[i];

                m_query << ',';

                switch (f.get_type()) {
                    case sqlite::column<T>::type::INT: {
                        auto p = f.get_int_pointer();
                        m_query << (*object).*p;
                        break;
                    }
                    case sqlite::column<T>::type::STRING: {
                        auto p = f.get_string_pointer();
                        m_query << "'" << (*object).*p << "'";
                        break;
                    }
                }
            }
        }

        void exec() {
            sqlite3_exec(m_db, m_query.str().c_str(), nullptr, nullptr, nullptr);

            m_query.str("");
        }

    };

}