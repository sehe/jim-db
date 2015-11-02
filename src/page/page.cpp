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
#include "../index/objectindex.h"
#include "../datatype/arrayitem.h"
#include "../network/MessageFactory.h"
#include "../meta/metaindex.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/PrettyWriter.h>

namespace jimdb
{
    namespace memorymanagement
    {
        long long Page::m_s_idCounter = 0;
        long long Page::m_objCount = 0;
        //take care order does matter here since header and body need to be init first to set the freepos values!
        Page::Page(long long header, long long body) : m_header(new char[header]), m_body(new char[body]), m_freeSpace(body),
            //set the freepos value ptr to the first header slot
            m_freepos(new(static_cast<char*>(m_header)) long long(0)),
            //set the header free pos ptr to the second  "long long" slot
            m_headerFreePos(new(static_cast<char*>(m_header) + sizeof(long long)) long long(0)), m_next(0),
            m_id(++m_s_idCounter)
        {
            //because of m_freepos and m_headerFreePos
            m_headerSpace = header - 2 * sizeof(long long);

            //add the free type into the body
            m_free = new(&static_cast<char*>(m_body)[0]) FreeType(body);
            ASSERT(m_free->getFree() == body);

            //set the free of the header
            m_headerFree = new(static_cast<char*>(m_header) + 2 * sizeof(long long)) FreeType(header - 2 * sizeof(long long));
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

        bool Page::isLocked() const
        {
            return m_rwLock;
        }

        bool Page::free(const size_t& size)
        {
            //tasking::RWLockGuard<> lock(m_rwLock, tasking::WRITE);
            return find(size).first != nullptr
                   && m_headerSpace >= sizeof(HeaderMetaData);//nullptr = false in c++ 11 but be safe her
        }

        size_t Page::insert(const rapidjson::GenericValue<rapidjson::UTF8<>>& value)
        {
            //never forget to lock ...
            tasking::RWLockGuard<> lock(m_rwLock, tasking::WRITE);

            //here we know this page has one chunk big enough to fitt the whole object
            //including its overhead! so start inserting it.

            //get the object name
            std::string name = value.MemberBegin()->name.GetString();

            //returns the pos of the first element needed for the headerdata

            auto firstElementPos = insertObject(value.MemberBegin()->value, nullptr).first;
            auto l_first = dist(m_body, firstElementPos);


            //insert the header
            auto meta = insertHeader(m_objCount++, 0, common::FNVHash()(name), l_first);
            //push the obj to obj index
            index::ObjectIndex::getInstance().add(meta->getOID(), {m_id, dist(m_header, meta)});
            //done!
            return meta->getOID();
        }

        void Page::setObjCounter(const long long& value) const
        {
            m_objCount = value;
        }


        /** short explaination
        - First we find a new FreeType where we can fit the Type.
        - We get the next (if there is one != 0)
        - we store the size of the free type
        - create the Type and set the prev->next if there is a prev
        - update the freeSpace
        - reduce the size of the "new Free" we are going to create
        - calc the position of the new Free
        - get the next of the free (which is also a FreeType)
        - call update Free
        - create the new freetype at the new position
        - set prev->next if there is a prev
        - set newFree->next = next if there is a next
        - also update the "freepos" for dumping
        */

        std::pair<void*, void*> Page::insertObject(const rapidjson::GenericValue<rapidjson::UTF8<>>& value,
                BaseType<size_t>* last)
        {
            //return ptr to the first element
            void* l_ret = nullptr;
            //prev element ptr
            BaseType<size_t>* l_prev = last;

            //do update this
            //dont forget it pos = m_free to change m_free to the
            //new pos!
            FreeType* l_pos = nullptr;
            //get the members
            for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it)
            {
                switch (it->value.GetType())
                {
                    case rapidjson::kNullType:
                        LOG_WARN << "null type: " << it->name.GetString();
                        continue;

                    case rapidjson::kFalseType:
                    case rapidjson::kTrueType:
                        {
                            //doesnt matter since long long and double are equal on
                            // x64
                            //find pos where the string fits
                            auto chain = find(sizeof(BaseType<bool>));
                            l_pos = chain.first;
                            auto l_next = l_pos->getNext();
                            auto l_size = l_pos->getFree();

                            void* l_new = new (l_pos) BaseType<bool>(it->value.GetBool());

                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));

                            m_freeSpace -= sizeof(BaseType<long long>);
                            l_size -= sizeof(BaseType<long long>);
                            //move the pos backwards by..
                            auto l_newFreePos = reinterpret_cast<char*> (l_pos) + sizeof(BaseType<long long>);

                            FreeType* l_nextptr = nullptr;
                            if (l_next != 0)
                                l_nextptr = reinterpret_cast<FreeType*> (reinterpret_cast<char*> (l_pos) + l_next);

                            updateFree(l_newFreePos, l_size, chain.second, l_nextptr);

                        }
                        break;
                    case rapidjson::kObjectType:
                        {
                            //pos for the obj id
                            //and insert the ID of the obj
                            auto chain = find(sizeof(BaseType<size_t>));
                            l_pos = chain.first;
                            auto l_next = l_pos->getNext();
                            auto l_size = l_pos->getFree();

                            void* l_new = new (l_pos) BaseType<size_t>(common::FNVHash()(it->name.GetString()));

                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));

                            m_freeSpace -= sizeof(BaseType<long long>);
                            l_size -= sizeof(BaseType<long long>);
                            //move the pos backwards by..
                            auto l_newFreePos = reinterpret_cast<char*> (l_pos) + sizeof(BaseType<size_t>);

                            FreeType* l_nextptr = nullptr;
                            if (l_next != 0)
                                l_nextptr = reinterpret_cast<FreeType*> (reinterpret_cast<char*> (l_pos) + l_next);

                            updateFree(l_newFreePos, l_size, chain.second, l_nextptr);

                            // pass the objid Object to the insertobj!
                            // now recursive insert the obj
                            // the second contains the last element inserted
                            // l_pos current contains the last inserted element and get set to the
                            // last element of the obj we insert
                            l_pos = static_cast<FreeType*>(insertObject(it->value, reinterpret_cast<BaseType<size_t>*>(l_pos)).second);
                        }
                        break;

                    case rapidjson::kArrayType:
                        {
                            //nope not yet
                            //insertArray(it->value, l_prev);
                        }
                        break;

                    case rapidjson::kStringType:
                        {
                            // find pos where the string fits
                            // somehow we get here sometimes and it does not fit!
                            // which cant be since we lock the whole page
                            auto chain = find(sizeof(StringType) + strlen(it->value.GetString()));

                            l_pos = chain.first;
                            auto l_next = l_pos->getNext();
                            auto l_size = l_pos->getFree();
                            //add the String Type at the pos of the FreeType
                            auto* l_new = new (l_pos) StringType(it->value.GetString());
                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));

                            m_freeSpace -= (sizeof(StringType) + strlen(it->value.GetString()));
                            l_size -= (sizeof(StringType) + strlen(it->value.GetString()));
                            //move the pos backwards by..
                            auto l_newFreePos = reinterpret_cast<char*> (l_pos) + sizeof(BaseType<size_t>) + strlen(it->value.GetString());

                            FreeType* l_nextptr = nullptr;
                            if (l_next != 0)
                                l_nextptr = reinterpret_cast<FreeType*> (reinterpret_cast<char*> (l_pos) + l_next);

                            updateFree(l_newFreePos, l_size, chain.second, l_nextptr);
                        }
                        break;

                    case rapidjson::kNumberType:
                        {
                            //doesnt matter since long long and double are equal on
                            // x64
                            //find pos where the string fits
                            auto chain = find(sizeof(BaseType<long long>));
                            l_pos = chain.first;
                            auto l_next = l_pos->getNext();
                            auto l_size = l_pos->getFree();

                            void* l_new;
                            if (it->value.IsInt())
                            {
                                //insert INT
                                l_new = new (l_pos) BaseType<long long>(it->value.GetInt64());
                            }
                            else
                            {
                                //INSERT DOUBLE
                                l_new = new (l_pos) BaseType<double>(it->value.GetDouble());
                            }
                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));

                            m_freeSpace -= sizeof(BaseType<long long>);
                            l_size -= sizeof(BaseType<long long>);
                            //move the pos backwards by..
                            auto l_newFreePos = reinterpret_cast<char*> (l_pos) + sizeof(BaseType<long long>);

                            FreeType* l_nextptr = nullptr;
                            if (l_next != 0)
                                l_nextptr = reinterpret_cast<FreeType*> (reinterpret_cast<char*> (l_pos) + l_next);

                            updateFree(l_newFreePos, l_size, chain.second, l_nextptr);
                        }
                        break;
                    default:
                        LOG_WARN << "Unknown member Type: " << it->name.GetString() << ":" << it->value.GetType();
                        continue;
                }
                //so first element is set now, store it to return it.
                if(l_ret == nullptr)
                {
                    l_ret = l_pos;
                }
                //prev is the l_pos now so cast it to this;
                l_prev = reinterpret_cast<BaseType<size_t>*>(l_pos);
            }
            //if we get here its in!
            return{ l_ret, l_pos };
        }

        std::pair<FreeType*, FreeType*> Page::find(const size_t& size)
        {
            if (size >= m_freeSpace)
                return {nullptr , nullptr};
            //copy free ptr
            FreeType* l_prev = nullptr;
            auto l_free = m_free;
            //ASSERT(*m_freepos == dist(m_body, m_free));
            while(true)
            {
                //we need at least one more spot for FreeType
                //else we lose chunks
                if (l_free->getFree() > size + sizeof(FreeType))
                    return{ l_free, l_prev };

                if (l_free->getNext() != 0)
                {
                    l_prev = l_free;//set the previous
                    auto l_temp = reinterpret_cast<char*>(l_free);
                    l_temp += l_free->getNext();
                    l_free = reinterpret_cast<FreeType*>(l_temp);
                    continue;
                }
                //if we get here we have no space for that!
                return{ nullptr , nullptr };
            }
        }

        std::ptrdiff_t Page::dist(const void* p1, const void* p2) const
        {
            //dist from 1 to 2 is  2-1 ....
            return static_cast<const char*>(p2) - static_cast<const char*>(p1);
        }

        void Page::updateFree(void* pos, const size_t& size,  FreeType* prev, FreeType* next)
        {
            FreeType* l_free = new(pos) FreeType(size);
            //set the pref
            if (prev != nullptr)
            {
                prev->setNext(prev - l_free);
            }
            else
            {
                //if there is no prev we got the first one to be updated!
                m_free = l_free;
                //also update the freepos
                *m_freepos = dist(m_body, m_free);
            }

            //if next set it
            if(next != nullptr)
                l_free->setNext(l_free - next);
        }

        void* Page::insertArray(const rapidjson::GenericValue<rapidjson::UTF8<>>& arr, BaseType<size_t>* prev)
        {
            void* l_ret = nullptr;
            for (auto arrayIt = arr.Begin(); arrayIt != arr.End(); ++arrayIt)
            {
                FreeType* l_pos = nullptr;

                switch (arrayIt->GetType())
                {
                    case rapidjson::kNullType:
                        break;
                    case rapidjson::kFalseType:
                    case rapidjson::kTrueType:
                        {
                            //doesnt matter since long long and double are equal on
                            // x64
                            //find pos where the string fits
                            auto chain = find(sizeof(ArrayItem<bool>));
                            l_pos = chain.first;
                            auto l_next = l_pos->getNext();
                            auto l_size = l_pos->getFree();

                            void* l_new = new (l_pos) ArrayItem<bool>(arr.GetBool(), BOOL);

                            if (prev != nullptr)
                                prev->setNext(dist(prev, l_new));

                            m_freeSpace -= sizeof(BaseType<long long>);
                            l_size -= sizeof(BaseType<long long>);
                            //move the pos backwards by..
                            auto l_newFreePos = reinterpret_cast<char*> (l_pos) + sizeof(ArrayItem<bool>);

                            FreeType* l_nextptr = nullptr;
                            if (l_next != 0)
                                l_nextptr = reinterpret_cast<FreeType*> (reinterpret_cast<char*> (l_pos) + l_next);

                            updateFree(l_newFreePos, l_size, chain.second, l_nextptr);
                        }
                        break;
                    case rapidjson::kObjectType:
                        break;
                    case rapidjson::kArrayType:
                        break;
                    case rapidjson::kStringType:
                        break;
                    case rapidjson::kNumberType:
                        break;
                    default:
                        break;
                }
                //at the last "FreeType" we inserted the Last element
                l_ret = l_pos;
            }
            return l_ret;
        }


        HeaderMetaData* Page::insertHeader(const size_t& id, const size_t& hash, const size_t& type, const size_t& pos)
        {
            //get the spot and its next and size
            auto l_pair = findHeaderSpot();
            auto l_next = l_pair.first->getNext();
            auto l_size = l_pair.first->getFree();

            //create the new header at the position of the first element returned
            auto l_meta = new(reinterpret_cast<char*>(l_pair.first)) HeaderMetaData(id, hash, type, pos);

            //move the "old" by the size of headermeta data ->
            auto l_newFreePos = reinterpret_cast<char*> (l_pair.first) + sizeof(HeaderMetaData);

            l_size -= sizeof(HeaderMetaData);
            m_headerSpace -= sizeof(HeaderMetaData);

            FreeType* l_nextptr = nullptr;
            if (l_next != 0)
                l_nextptr = reinterpret_cast<FreeType*> (reinterpret_cast<char*> (l_pair.first) + l_next);

            updateFreeHeader(l_newFreePos, l_size, l_pair.second, l_nextptr);
            return l_meta;
        }

        std::pair<FreeType*, FreeType*> Page::findHeaderSpot()
        {
            if (sizeof(HeaderMetaData) >= m_headerSpace)
                return{ nullptr , nullptr };
            //copy free ptr
            FreeType* l_prev = nullptr;
            auto l_free = m_headerFree;
            while (true)
            {
                if (l_free->getFree() > sizeof(HeaderMetaData))
                    return{ l_free, l_prev };

                if (l_free->getNext() != 0)
                {
                    l_prev = l_free;//set the previous
                    auto l_temp = reinterpret_cast<char*>(l_free);
                    l_temp += l_free->getNext();
                    l_free = reinterpret_cast<FreeType*>(l_temp);
                    continue;
                }
                //if we get here we have no space for that!
                return{ nullptr , nullptr };
            }
        }

        void Page::updateFreeHeader(void* pos, const size_t& size, FreeType* prev, FreeType* next)
        {
            FreeType* l_free = new(pos) FreeType(size);
            //set the pref
            if (prev != nullptr)
            {
                prev->setNext(prev - l_free);
            }
            else
            {
                //if there is no prev we got the first one to be updated!
                m_headerFree = l_free;
                //also update the freepos
                *m_headerFreePos = dist(m_free, m_body);
            }

            //if next set it
            if (next != nullptr)
                l_free->setNext(l_free - next);
        }

        void* Page::buildObject(const size_t& hash, void* start, rapidjson::Value& l_obj,
                                rapidjson::MemoryPoolAllocator<>& aloc)
        {
            //get the meta information of the object type
            //to build it
            auto& l_metaIdx = meta::MetaIndex::getInstance();
            //get the meta dataset
            auto& l_meta = l_metaIdx[hash];

            //now we are already in an object here with l_obj!
            auto l_ptr = start;
            for (auto it = l_meta->begin(); it != l_meta->end(); ++it)
            {
                //create the name value
                rapidjson::Value l_name(it->name.c_str(), it->name.length(), aloc);
                //create the value we are going to add
                rapidjson::Value l_value;
                //now start building it up again
                switch (it->type)
                {
                    case meta::OBJECT:
                        {
                            auto l_data = static_cast<BaseType<size_t>*>(l_ptr);
                            //get the hash to optain the metadata
                            auto l_hash = l_data->getData();
                            //set to object and create the inner object
                            l_value.SetObject();

                            //get the start pointer which is the "next" element
                            //and call recursive
                            l_ptr = static_cast<BaseType<size_t>*>(buildObject(l_hash,
                                                                   reinterpret_cast<char*>(l_data) + l_data->getNext(), l_value, aloc));
                        }
                        break;
                    case meta::ARRAY:
                        break;
                    case meta::INT:
                        {
                            //create the data
                            auto l_data = static_cast<BaseType<long long>*>(l_ptr);
                            //with length attribute it's faster ;)
                            l_value = l_data->getData();
                        }
                        break;
                    case meta::DOUBLE:
                        {
                            //create the data
                            auto l_data = static_cast<BaseType<double>*>(l_ptr);
                            //with length attribute it's faster ;)
                            l_value = l_data->getData();
                        }
                        break;
                    case meta::STRING:
                        {
                            //create the data
                            auto l_data = static_cast<StringType*>(l_ptr);
                            //with length attribute it's faster
                            l_value.SetString(l_data->getString()->c_str(), l_data->getString()->length(), aloc);
                        }
                        break;
                    case meta::BOOL:
                        {
                            //create the data
                            auto l_data = static_cast<BaseType<bool>*>(l_ptr);
                            l_value = l_data->getData();
                        }
                        break;
                    default:
                        break;
                }
                l_obj.AddMember(l_name, l_value, aloc);
                //update the lptr
                l_ptr = static_cast<char*>(l_ptr) + static_cast<BaseType<size_t>*>(l_ptr)->getNext();
            }
            //return the l_ptr which current shows to the next lement. //see line above
            return l_ptr;
        }

        std::shared_ptr<std::string> Page::getJSONObject(const long long& headerpos)
        {
			//reading the Page
            tasking::RWLockGuard<> lock(m_rwLock, tasking::READ);
            //get the header first
            auto l_header = reinterpret_cast<HeaderMetaData*>(static_cast<char*>(m_header) + headerpos);
            //get the type
            auto l_objectType = l_header->getObjektType();

            //get the idx
            auto& l_metaIdx = meta::MetaIndex::getInstance();
            //get the meta dataset
            auto& l_meta = l_metaIdx[l_objectType];

            //create the document
            rapidjson::Document l_obj;
            l_obj.SetObject();

            //now generate the inner part of the object
            rapidjson::Value l_value;
            l_value.SetObject();
            //calc the start id
            void* start = static_cast<char*>(m_body) + l_header->getPos();
            buildObject(l_objectType, start, l_value, l_obj.GetAllocator());

            //generate name
            auto l_objName = l_meta->getName();
            rapidjson::Value l_name(l_objName.c_str(), l_objName.length(), l_obj.GetAllocator());
            //now add the inner object
            l_obj.AddMember(l_name, l_value, l_obj.GetAllocator());

            //convert to string
            rapidjson::StringBuffer strbuf;
            rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
            l_obj.Accept(writer);
            return std::make_shared<std::string>(strbuf.GetString());
        }
    }
}