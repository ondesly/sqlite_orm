//
//  commands.h
//  sqlite_orm
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 19.12.2022.
//  Copyright © 2019-2022 Dmitrii Torkhov. All rights reserved.
//

#pragma once

namespace sqlite {

        using command = unsigned short;

        const command NONE = 0;
        const command SELECT = NONE + 1;
        const command CREATE_TABLE_IF_NOT_EXISTS = SELECT + 1;
        const command CREATE_INDEX_IF_NOT_EXISTS = CREATE_TABLE_IF_NOT_EXISTS + 1;
        const command INSERT_OR_REPLACE_INTO = CREATE_INDEX_IF_NOT_EXISTS + 1;
        const command UPDATE = INSERT_OR_REPLACE_INTO + 1;
        const command DELETE = UPDATE + 1;
        const command SET = DELETE + 1;
        const command COUNT = SET + 1;
        const command FROM = COUNT + 1;
        const command WHERE = FROM + 1;
        const command ORDER_BY = WHERE + 1;
        const command ALL = ORDER_BY + 1;
        const command VALUES = ALL + 1;
        const command BETWEEN = VALUES + 1;
        const command AND = BETWEEN + 1;
        const command OR = AND + 1;
        const command IN = OR + 1;
        const command ON = IN + 1;
        const command EQUALS = ON + 1;
        const command NOT_EQUALS = EQUALS + 1;
        const command EMPTY_STRING = NOT_EQUALS + 1;

}