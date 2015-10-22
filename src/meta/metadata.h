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

namespace jimdb
{
    namespace meta
    {
        /**
        \brief Typedef a index to a MetaManager

        A Manager for the meta Data is simply just a index of the
        hash and the shared_ptr to the MetaData which should be threadsafe again.

        @author Benjamin Meyer
        @date 22.10.2015 14:00
        */
        typedef index::Index<size_t, std::shared_ptr<MetaData>> MetaManager;

        enum AttributeTypes
        {
            OBJECT, ARRAY, INT, DOUBLE, STRING, BOOL
        };

        /**
        @brief metdata for one object

        A metadata set for objects. The set itself is a index
        so its locked. So if someone changes the index while someone read
        nothing should happen.

        @author Benjamin Meyer
        @date 22.10.2015 11:58
        */
        class MetaData: public index::Index<std::string, AttributeTypes>
        {
        public:
            /**
            \brief Create a new metadata with the object name
            @param[in] name the name of the object
            @author Benjamin Meyer
            @date 22.10.2015 13:29
            */
            explicit inline MetaData(const std::string& name);
            virtual ~MetaData() {};

            size_t getHash() const;
        private:
            const std::string m_objectName;
            size_t m_hash;
        };
    }
}

#include "metadata.hpp"