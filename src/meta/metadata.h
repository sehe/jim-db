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
#include "../common/fnvhash.h"
#include "../index/index.h"
#include <vector>

namespace jimdb
{
    namespace meta
    {
        enum AttributeTypes
        {
            OBJECT, ARRAY, INT, DOUBLE, STRING, BOOL
        };

		struct MetaValue
		{
			std::string name;
			AttributeTypes type;
		};

        /**
        @brief metdata for one object

        A metadata set for objects. The set itself is a index
        so its locked. So if someone changes the index while someone read
        nothing should happen.

        @author Benjamin Meyer
        @date 22.10.2015 11:58
        */
        class MetaData: public std::vector<MetaValue>
        {
        public:
            /**
            \brief Create a new metadata with the object name
            @param[in] name the name of the object
            @author Benjamin Meyer
            @date 22.10.2015 13:29
            */
            explicit inline MetaData(const std::string& name);

            size_t getHash() const;
			std::string getName() const;
        private:
            const std::string m_objectName;
            size_t m_hash;
        };
    }
}

#include "metadata.hpp"