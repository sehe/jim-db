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
#include <memory>
#include "../datatype/freetype.h"
#include <rapidjson/document.h>

namespace jimdb
{
    namespace memorymanagement
    {
        /**
        \brief The page Class

        It holds data and header information.

        @author Benjamin Meyer
        @date 23.10.2015 16:34
        */
        class Page
        {
        public:
            Page(long long header, long long body);
            ~Page();

			long long getID();
            void setNext(const long long& id);
            long long getNext() const;

            long long free() const;
            /**
            \brief Insert a json obj to the page and checks for the meta data

            it also creates the metadata for the objects if needed
            @author Benjamin Meyer
            @date 23.10.2015 17:26
            */
            void insert(const rapidjson::GenericValue<rapidjson::UTF8<>>& value);

        private:
            //voidptr to memory to static cast as we like
            void* m_header;
            void* m_body;

            //pointer to the free typ chain start
            FreeType* m_free;

            //position of the free typ object start info
            long long* m_freepos;

            //offset to the next free header position
            long long m_headerFreePos;
            long long m_headerSpace;

            //holds the information of free space
            long long m_freeSpace;
            //holds the ID of the next page
            long long m_next;
            long long m_id;

            //id generation with static counter
            static long long m_s_idCounter;

            /**
            \brief Method for inserting meta data

            Recursiv method to insert data into the Meta data set.
            Also checks if its a new one or not.

            @param[in] name the name of the new object
            @param[in] value the rapidjson generic value "the object" itself
            @author Benjamin Meyer
            @date 23.10.2015 15:31
            */
            void insertMeta(const std::string& name, const rapidjson::GenericValue<rapidjson::UTF8<>>& value) const;



            void inserHeader(const size_t& id, const size_t& hash, const size_t& type, const size_t& pos);

            /**
            \brief add data to the body

            add a string to the body and returns it's offset
            so the offset can be used to set the header start pos right
            @author Benjamin Meyer
            @date 23.10.2015 16:50
            */
            //template<typename T>
            //long long insert(const T& s, const long long& next);
        };
    }
}
#include "page.hpp"