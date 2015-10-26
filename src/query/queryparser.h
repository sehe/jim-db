/**
############################################################################
# GPL License                                                              #
#                                                                          #
# This file is part of the JIM-DB.                                         #
# Copyright (c) 2015, Benjamin Meyer, <benjamin.meyer@tu-clausthal.de>     #
# This program is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU General Public License as                  #
# published by the Free Software Foundation, either version 3 of the       #
# License, or (at your option) any later version.                          #
#                                                                          #
# This program is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of           #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
# GNU General Public License for more details.                             #
#                                                                          #
# You should have received a copy of the GNU General Public License        #
# along with this program. If not, see <http://www.gnu.org/licenses/>.     #
############################################################################
**/
#pragma once
#include <string>
#include <vector>

namespace jimdb
{
    namespace query
    {
        /**
        \brief A Condition Object of a query

        @author Benjamin Meyer
        @date 26.10.2015 17:59
        */
        class Condition
        {
        public:
            Condition(const bool& eq, const std::string& name, const std::string& value);
            bool m_equal;
            std::string m_name;
            std::string m_value;
        };

        /**
        \brief The QueryParser

        BODY
        @author Benjamin Meyer
        @date 26.10.2015 17:59
        */
        class QueryParser
        {
        public:
            void parse(std::string& s);
        private:
            std::vector<Condition> m_conditions;

            static const char m_delimiter;

            enum EQueryType
            {
                SELECT,
                DELETE_R,
                INSERT
            } m_type;

            static const char* m_queryTypes[];

            //names for the conditions
            enum ECondition
            {
                WHERE,
                OR,
                AND,
                NOTEQUAL,
                EQUAL
            };
            static const char* m_condTypes[];

            //vector of and and ors
            std::vector<ECondition> m_andOr;

            void parseConditions(std::stringstream& ss);

            //delete the " of a string using rvalue
            std::string& erase(std::string&& s) const;
            std::string& erase(std::string& s) const;
        };
    }
}