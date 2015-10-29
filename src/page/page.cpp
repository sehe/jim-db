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

        bool Page::insert(const rapidjson::GenericValue<rapidjson::UTF8<>>& value)
        {
            //never forget to lock ...
            tasking::RWLockGuard<> lock(m_rwLock, tasking::WRITE);
            //get the object name
            std::string name = value.MemberBegin()->name.GetString();
            //TODO Insert header here!!
            //TODO how to get first obj?! heck!

            bool l_worked = insertObject(value.MemberBegin()->value, nullptr);

            return l_worked;
        }

        void Page::inserHeader(const size_t& id, const size_t& hash, const size_t& type, const size_t& pos)
        {
            //push into the header
            new(&static_cast<char*>(m_header)[m_headerFreePos]) HeaderMetaData(id, hash, type, pos);
            m_headerFreePos += sizeof(HeaderMetaData);
        }

        bool Page::insertObject(const rapidjson::GenericValue<rapidjson::UTF8<>>& value, BaseType<size_t>* l_last)
        {
            //get the members
            for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it)
            {
                //do update this
                //dont forget it pos = m_free to change m_free to the
                //new pos!
                FreeType* l_pos = nullptr;
                switch (it->value.GetType())
                {
                    case rapidjson::kNullType:
                        LOG_WARN << "null type: " << it->name.GetString();
                        break;

                    case rapidjson::kFalseType:
                    case rapidjson::kTrueType:
                        l_pos = find(sizeof(BaseType<bool>));

                        break;

                    case rapidjson::kObjectType:
                        //pos for the obj id
                        l_pos = find(sizeof(BaseType<size_t>));
                        //TODO insert obj id HERE
                        // pass the objid Object to the insertobj!
                        // now recursive insert the obj
                        insertObject(it->value, nullptr);
                        break;

                    case rapidjson::kArrayType:
                        //pos for the array size
                        l_pos = find(sizeof(BaseType<size_t>));
                        //and insert array after this

                        break;

                    case rapidjson::kStringType:
                        //find pos where the string fits
                        l_pos = find(sizeof(BaseType<size_t>) + strlen(it->value.GetString()));
                        break;

                    case rapidjson::kNumberType:
                        //doesnt matter since long long and double are equal on
                        // x64
                        l_pos = find(sizeof(BaseType<long long>));
                        if (it->value.IsInt())
                        {
                            //insert INT
                        }
                        else
                        {
                            //INSERT DOUBLE
                        }
                        break;
                    default:
                        LOG_WARN << "Unknown member Type: " << it->name.GetString() << ":" << it->value.GetType();
                        break;
                }
            }
            //if we get here its in!
            return true;
        }

        FreeType* Page::find(const size_t& size)
        {
            if (size >= m_freeSpace)
                return nullptr;
            //copy free ptr
            auto l_free = m_free;
            while(true)
            {
                if (l_free->getFree() > size)
                    return l_free;

                if (l_free->getNext() != 0)
                {
                    auto l_temp = reinterpret_cast<char*>(l_free);
                    l_temp += l_free->getNext();
                    l_free = reinterpret_cast<FreeType*>(l_temp);
                    continue;
                }
                //if we get here we have no space for that!
                return nullptr;
            }
        }

        std::ptrdiff_t Page::dist(const void* p1, const void* p2) const
        {
            return static_cast<const char*>(p1) - static_cast<const char*>(p2);
        }
    }
}