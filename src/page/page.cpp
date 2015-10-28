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

#include "page.h"
#include "../assert.h"
#include "../meta/metadata.h"
#include "../datatype/headermetadata.h"
#include "../meta/metaindex.h"

namespace jimdb
{
    namespace memorymanagement
    {
        long long Page::m_s_idCounter = 0;

        Page::Page(long long header, long long body) : m_freeSpace(body), m_next(0), m_id(++m_s_idCounter)
        {
            m_header = new char[header];
            m_body = new char[body];
            //put free type info in header
            //so at pos 0 is a long long with the pos of the free type
            m_freepos = new(&static_cast<char*>(m_header)[0]) long long(0);
            //add the free ofset from the header
            m_headerFreePos = sizeof(long long);
            m_headerSpace = header - sizeof(long long);

            //throw in the free type into the body
            m_free = new(&static_cast<char*>(m_body)[0]) FreeType(body - sizeof(FreeType));
            ASSERT(m_free->getFree() == body);
        }

        Page::~Page()
        {
            //clean up all data
            delete[] m_header;
            delete[] m_body;
        }

        long long Page::getID()
        {
            tasking::RWLockGuard<> lock(m_rwLock, tasking::READ);
            return m_s_idCounter;
        }

        void Page::setNext(const long long& id)
        {
            m_next = id;
        }

        long long Page::getNext() const
        {
            return m_next;
        }

        long long Page::free()
        {
            tasking::RWLockGuard<> lock(m_rwLock, tasking::READ);
            return m_freeSpace;
        }

        void Page::insert(const rapidjson::GenericValue<rapidjson::UTF8<>>& value)
        {
        }

        void Page::inserHeader(const size_t& id, const size_t& hash, const size_t& type, const size_t& pos)
        {
            //push into the header
            new(&static_cast<char*>(m_header)[m_headerFreePos]) HeaderMetaData(id, hash, type, pos);
            m_headerFreePos += sizeof(HeaderMetaData);
        }
    }
}