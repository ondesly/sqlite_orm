//
//  commands.h
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 19.12.2022.
//  Copyright © 2019-2023 Dmitrii Torkhov. All rights reserved.
//

#pragma once

namespace sqlite {

    enum class command {
        NONE,
        PRAGMA,
        TABLE_INFO,
        SELECT,
        CREATE_TABLE_IF_NOT_EXISTS,
        ALTER_TABLE,
        CREATE_INDEX_IF_NOT_EXISTS,
        INSERT_OR_REPLACE_INTO,
        UPDATE,
        DELETE,
        ADD_COLUMN,
        SET,
        COUNT,
        FROM,
        WHERE,
        ORDER_BY,
        ALL,
        VALUES,
        BETWEEN,
        AND,
        OR,
        IN,
        ON,
        EQUALS,
        NOT_EQUALS,
        EMPTY_STRING,
        ASC,
        DESC,
        LIMIT
    };

    static constexpr auto NONE = command::NONE;
    static constexpr auto PRAGMA = command::PRAGMA;
    static constexpr auto TABLE_INFO = command::TABLE_INFO;
    static constexpr auto SELECT = command::SELECT;
    static constexpr auto CREATE_TABLE_IF_NOT_EXISTS = command::CREATE_TABLE_IF_NOT_EXISTS;
    static constexpr auto ALTER_TABLE = command::ALTER_TABLE;
    static constexpr auto CREATE_INDEX_IF_NOT_EXISTS = command::CREATE_INDEX_IF_NOT_EXISTS;
    static constexpr auto INSERT_OR_REPLACE_INTO = command::INSERT_OR_REPLACE_INTO;
    static constexpr auto UPDATE = command::UPDATE;
    static constexpr auto DELETE = command::DELETE;
    static constexpr auto ADD_COLUMN = command::ADD_COLUMN;
    static constexpr auto SET = command::SET;
    static constexpr auto COUNT = command::COUNT;
    static constexpr auto FROM = command::FROM;
    static constexpr auto WHERE = command::WHERE;
    static constexpr auto ORDER_BY = command::ORDER_BY;
    static constexpr auto ALL = command::ALL;
    static constexpr auto VALUES = command::VALUES;
    static constexpr auto BETWEEN = command::BETWEEN;
    static constexpr auto AND = command::AND;
    static constexpr auto OR = command::OR;
    static constexpr auto IN = command::IN;
    static constexpr auto ON = command::ON;
    static constexpr auto EQUALS = command::EQUALS;
    static constexpr auto NOT_EQUALS = command::NOT_EQUALS;
    static constexpr auto EMPTY_STRING = command::EMPTY_STRING;
    static constexpr auto ASC = command::ASC;
    static constexpr auto DESC = command::DESC;
    static constexpr auto LIMIT = command::LIMIT;

}
