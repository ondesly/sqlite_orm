//
// Created by ondesly on 2019-05-21.
// Copyright (c) 2019 ondesly. All rights reserved.
//

#pragma once

#include <string>

namespace sqlite {

    template<class T>
    class column {
    public:

        enum class type {
            INT,
            STRING
        };

    public:

        column(int T::* const i, const std::string &name) : m_pointer(i), m_name(name), m_type(type::INT) {

        }

        column(std::string T::* const s, const std::string &name)
                : m_pointer(s), m_name(name), m_type(type::STRING) {

        }

    public:

        int T::* get_int_pointer() const {
            return m_pointer.m_i;
        }

        std::string T::* get_string_pointer() const {
            return m_pointer.m_s;
        }

        const std::string &get_name() const {
            return m_name;
        }

        type get_type() const {
            return m_type;
        }

        bool equals(int T::* const pointer) const {
            if (m_type == type::INT) {
                return m_pointer.m_i == pointer;
            } else {
                return false;
            }
        }

        bool equals(std::string T::* const pointer) const {
            if (m_type == type::STRING) {
                return m_pointer.m_s == pointer;
            } else {
                return false;
            }
        }

    private:

        union pointer_u {

            int T::* m_i;
            std::string T::* m_s;

            pointer_u(int T::* const i) : m_i(i) {
            }

            pointer_u(std::string T::* const s) : m_s(s) {
            }

        };

    private:

        pointer_u m_pointer;

        std::string m_name;

        column::type m_type;

    };

}