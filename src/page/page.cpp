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
#include "../network/messagefactory.h"
#include "../meta/metaindex.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include "../datatype/arrayitemstring.h"
#include "../datatype/arraytype.h"

namespace jimdb
{
    namespace memorymanagement
    {
        int64_t Page::m_s_idCounter = 0;
        int64_t Page::m_objCount = 0;
        //take care order does matter here since header and body need to be init first to set the freepos values!
        Page::Page(int64_t header, int64_t body) : m_header(new char[header]), m_body(new char[body]), m_freeSpace(body),
            //set the freepos value ptr to the first header slot
            m_freepos(new(static_cast<char*>(m_header)) int64_t(0)),
            //set the header free pos ptr to the second  "int64_t" slot
            m_headerFreePos(new(static_cast<char*>(m_header) + sizeof(int64_t)) int64_t(0)), m_next(0),
            m_id(++m_s_idCounter)
        {
            //because of m_freepos and m_headerFreePos
            m_headerSpace = header - 2 * sizeof(int64_t);

            //add the free type into the body
            m_free = new(&static_cast<char*>(m_body)[0]) FreeType(body);
            ASSERT(m_free->getFree() == body);

            //set the free of the header
            m_headerFree = new(static_cast<char*>(m_header) + 2 * sizeof(int64_t)) FreeType(header - 2 * sizeof(int64_t));
        }

        Page::~Page()
        {
            //clean up all data
            delete[] m_header;
            delete[] m_body;
        }

        int64_t Page::getID()
        {
            tasking::RWLockGuard<> lock(m_rwLock, tasking::READ);
            return m_s_idCounter;
        }

        void Page::setNext(const int64_t& id)
        {
            m_next = id;
        }

        int64_t Page::getNext() const
        {
            return m_next;
        }

        int64_t Page::free()
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
            //tasking::RWLockGuard<> lock(m_rwLock, tasking::READ);
            //find without alocing here!
            return find(size, false) != nullptr
                   //do not aloc
                   && findHeaderPosition(false) != nullptr; //nullptr = false in c++ 11 but be safe her
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

        void Page::setObjCounter(const int64_t& value) const
        {
            m_objCount = value;
        }


        std::shared_ptr<std::string> Page::getJSONObject(const int64_t& headerpos)
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


        /** short explaination
        - First we find a new FreeType where we can fit the Type.
        	- this automatically alocates the space and we can fit it
        	  without taking care of the freetype. Everything happens in free with aloc flag true
        - create the new Type at the position we got returned
        - update the previous next pointer to "us"
        - done
        */

        std::pair<void*, void*> Page::insertObject(const rapidjson::GenericValue<rapidjson::UTF8<>>& value,
                BaseType<size_t>* last)
        {
            //return ptr to the first element
            void* l_ret = nullptr;
            //prev element ptr
            BaseType<size_t>* l_prev = last;

            //position pointer
            void* l_pos = nullptr;
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
                            //doesnt matter since int64_t and double are equal on
                            // x64
                            //find pos where the string fits
                            l_pos = find(sizeof(BaseType<bool>));

                            void* l_new = new (l_pos) BaseType<bool>(it->value.GetBool());

                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));
                        }
                        break;
                    case rapidjson::kObjectType:
                        {
                            //pos for the obj id
                            //and insert the ID of the obj
                            l_pos = find(sizeof(BaseType<size_t>));
                            std::string name = it->name.GetString();
                            void* l_new = new (l_pos) BaseType<size_t>(common::FNVHash()(name));

                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));
							//TODO something strange happens here!

                            // pass the objid Object to the insertobj!
                            // now recursive insert the obj
                            // the second contains the last element inserted
                            // l_pos current contains the last inserted element and get set to the
                            // last element of the obj we insert
                            l_pos = (insertObject(it->value, reinterpret_cast<BaseType<size_t>*>(l_new)).second);
                        }
                        break;

                    case rapidjson::kArrayType:
                        {/**
                            //now insert values
                            l_pos = find(sizeof(ArrayType));

                            //insert the number of elements which follows
                            size_t l_size = it->value.Size();
                            void* l_new = new(l_pos) ArrayType(l_size);
                            //update prev
                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));
                            //insert elements

                            l_pos = insertArray(it->value, static_cast<BaseType<size_t>*>(l_new));
                        */
						}
                        break;

                    case rapidjson::kStringType:
                        {
                            // find pos where the string fits
                            // somehow we get here sometimes and it does not fit!
                            // which cant be since we lock the whole page
                            l_pos = find(sizeof(StringType) + strlen(it->value.GetString()));

                            //add the String Type at the pos of the FreeType
                            auto* l_new = new (l_pos) StringType(it->value.GetString());
                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));
                        }
                        break;

                    case rapidjson::kNumberType:
                        {
                            //doesnt matter since int64_t and double are equal on
                            // x64
                            //find pos where the string fits
                            l_pos = find(sizeof(BaseType<int64_t>));

                            void* l_new;
                            if (it->value.IsInt())
                            {
                                //insert INT
                                l_new = new (l_pos) BaseType<int64_t>(it->value.GetInt64());
                            }
                            else
                            {
                                //INSERT DOUBLE
                                l_new = new (l_pos) BaseType<double>(it->value.GetDouble());
                            }
                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));
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

        void* Page::find(const size_t& size, bool aloc)
        {
            //we cant fit it
            if (size >= m_freeSpace)
                return nullptr;
            //now get the next free slot which could fit it
            FreeType* l_prev = nullptr;
            auto l_freePtr = m_free;
            FreeType* l_ret = nullptr;
            while(true)
            {
                //return the position we we sure can fit it
                //without losing any chunks
                if (l_freePtr->getFree() > size + sizeof(FreeType))
                {
                    l_ret = l_freePtr;//safe the spot
                    break;
                }
                if(l_freePtr->getFree() == size)
                {
                    //if it exactly fit we also take the spot
                    l_ret = l_freePtr;
                    break;
                }
                if (l_freePtr->getNext() != 0)
                {
                    //else it does not fit and we need to go to the next

                    l_prev = l_freePtr;//set the previous
                    auto l_temp = reinterpret_cast<char*>(l_freePtr);
                    l_temp += l_freePtr->getNext();
                    l_freePtr = reinterpret_cast<FreeType*>(l_temp);
                    continue;
                }
                //if we get here we have no space for that!
                return nullptr;
            }
            //now we are here and need to aloc
            if(aloc)
            {
                m_freeSpace -= size;

                //now check if we lose (cuz it fits) one or need to create a new freeType
                if (l_ret->getFree() == size)
                {
                    //l_ret will be "deleted" out of the chain
                    if (l_prev == nullptr)
                    {
                        //if prev is null we have the start
                        //so update the m_free ptr
                        m_free = reinterpret_cast<FreeType*>(reinterpret_cast<char*>(l_ret) + l_ret->getNext());
                    }
                    else
                    {
                        //set prev next to the distance
                        //of prev -> to next + the next of that
                        l_prev->setNext(dist(l_prev, l_ret) + l_ret->getNext());
                    }
                }
                else
                {

                    //we regular can fit the size and do not need to "delete" one
                    //throw in the "new" free at l_ret pos and create a new freetype "after" it
                    // with the same FreeSize - size

                    //store the next since we new on that position now!
                    auto l_next = l_ret->getNext();

                    //check if we had the head if so update it
                    if (l_prev == nullptr)
                    {
                        m_free = new(reinterpret_cast<char*>(l_ret) + size) FreeType(l_ret->getFree() - size);
                        *m_freepos = dist(m_body, m_free);
                        m_free->setNext(dist(m_free, l_ret) + l_next);
                    }
                    else
                    {
                        auto l_newF = new(reinterpret_cast<char*>(l_ret) + size) FreeType(l_ret->getFree() - size);
                        //next is relativ to the l_ret and of that the next soo...
                        l_newF->setNext(dist(l_newF, l_ret) + l_next);
                        //update the prev
                        l_prev->setNext(dist(l_prev, l_newF));
                    }
                }
            }
            return l_ret;
        }


        std::ptrdiff_t Page::dist(const void* const p1, const void* const p2) const
        {
            //dist from 1 to 2 is  2-1 ....
            return static_cast<const char*>(p2) - static_cast<const char*>(p1);
        }

        void* Page::insertArray(const rapidjson::GenericValue<rapidjson::UTF8<>>& arr, BaseType<size_t>* prev)
        {
			ArrayItem<size_t>* l_prev = reinterpret_cast<ArrayItem<size_t>*>(prev);
            void* l_ret = nullptr;
            for (auto arrayIt = arr.Begin(); arrayIt != arr.End(); ++arrayIt)
            {
                void* l_pos = nullptr;

                switch (arrayIt->GetType())
                {
                    case rapidjson::kNullType:
                        break;
                    case rapidjson::kFalseType:
                    case rapidjson::kTrueType:
                        {
                            //doesnt matter since int64_t and double are equal on
                            // x64
                            //find pos where the string fits
                            l_pos = find(sizeof(ArrayItem<bool>));

                            void* l_new = new (l_pos) ArrayItem<bool>(arrayIt->GetBool(), BOOL);

                            if (prev != nullptr)
                                prev->setNext(dist(prev, l_new));
                        }
                        break;
                    case rapidjson::kObjectType:
                        break;
                    case rapidjson::kArrayType:
                        {
                            l_pos = find(sizeof(ArrayType));
                            //add the number of elements
                            void* l_new = new(l_pos) ArrayType(arrayIt->Size());
                            if (prev != nullptr)
                                prev->setNext(dist(prev, l_new));
                            l_pos = insertArray(*arrayIt, static_cast<BaseType<size_t>*>(l_pos));
                        }
                        break;
                    case rapidjson::kStringType:
                        {
                            l_pos = find(sizeof(ArrayItemString) + strlen(arrayIt->GetString()));

                            void* l_new = new (l_pos) ArrayItemString(arrayIt->GetString());

                            if (prev != nullptr)
                                prev->setNext(dist(prev, l_new));
                        }
                        break;
                    case rapidjson::kNumberType:
                        {
                            //doesnt matter since int64_t and double are equal on
                            // x64
                            //find pos where the string fits
                            l_pos = find(sizeof(ArrayItem<int64_t>));

                            void* l_new;
                            if (arrayIt->IsInt())
                            {
                                //insert INT
                                l_new = new (l_pos) ArrayItem<int64_t>(arrayIt->GetInt64(), INT);
                            }
                            else
                            {
                                //INSERT DOUBLE
                                l_new = new (l_pos) ArrayItem<double>(arrayIt->GetDouble(), DOUBLE);
                            }

                            if (l_prev != nullptr)
                                l_prev->setNext(dist(l_prev, l_new));
                        }
                        break;
                    default:
                        break;
                }
                //at the last "FreeType" we inserted the Last element
                l_ret = l_pos;
                //update the prev
                l_prev = reinterpret_cast<ArrayItem<size_t>*>(l_pos);
            }
            return l_prev;
        }


        HeaderMetaData* Page::insertHeader(const size_t& id, const size_t& hash, const size_t& type, const size_t& pos)
        {
            //create the new header at the position of the first element returned
            auto l_meta = new(findHeaderPosition()) HeaderMetaData(id, hash, type, pos);
            return l_meta;
        }

        void* Page::findHeaderPosition(bool aloc)
        {
            //we cant fit it
            if (sizeof(HeaderMetaData) >= m_headerSpace)
                return nullptr;
            //now get the next free slot which could fit it
            FreeType* l_prev = nullptr;
            auto l_freePtr = m_headerFree;
            FreeType* l_ret = nullptr;
            while (true)
            {
                //return the position we we sure can fit it
                //without losing any chunks
                if (l_freePtr->getFree() > sizeof(HeaderMetaData) + sizeof(FreeType))
                {
                    l_ret = l_freePtr;//safe the spot
                    break;
                }
                if (l_freePtr->getFree() == sizeof(HeaderMetaData))
                {
                    //if it exactly fit we also take the spot
                    l_ret = l_freePtr;
                    break;
                }
                if (l_freePtr->getNext() != 0)
                {
                    //else it does not fit and we need to go to the next

                    l_prev = l_freePtr;//set the previous
                    auto l_temp = reinterpret_cast<char*>(l_freePtr);
                    l_temp += l_freePtr->getNext();
                    l_freePtr = reinterpret_cast<FreeType*>(l_temp);
                    continue;
                }
                //if we get here we have no space for that!
                return nullptr;
            }

            if (aloc)
            {
                m_headerSpace -= sizeof(HeaderMetaData);

                //now check if we lose (cuz it fits) one or need to create a new freeType
                if (l_ret->getFree() == sizeof(HeaderMetaData))
                {
                    //l_ret will be "deleted" out of the chain
                    if (l_prev == nullptr)
                    {
                        //if prev is null we have the start
                        //so update the m_free ptr
                        m_headerFree = reinterpret_cast<FreeType*>(reinterpret_cast<char*>(l_ret) + l_ret->getNext());
                    }
                    else
                    {
                        //set prev next to the distance
                        //of prev -> to next + the next of that
                        l_prev->setNext(dist(l_prev, l_ret) + l_ret->getNext());
                    }
                }
                else
                {
                    //we regular can fit the size and do not need to "delete" one
                    //throw in the "new" free at l_ret pos and create a new freetype "after" it
                    // with the same FreeSize - size

                    //store the next since we new on that position now!
                    auto l_next = l_ret->getNext();

                    //check if we had the head if so update it
                    if (l_prev == nullptr)
                    {
                        m_headerFree = new(reinterpret_cast<char*>(l_ret) + sizeof(HeaderMetaData)) FreeType(l_ret->getFree()
                                - sizeof(HeaderMetaData));
                        *m_freepos = dist(m_body, m_free);
                        m_headerFree->setNext(dist(m_free, l_ret) + l_next);
                    }
                    else
                    {
                        auto l_newF = new(reinterpret_cast<char*>(l_ret) + sizeof(HeaderMetaData)) FreeType(l_ret->getFree()
                                - sizeof(HeaderMetaData));
                        //next is relativ to the l_ret and of that the next soo...
                        l_newF->setNext(dist(l_newF, l_ret) + l_next);
                        //update the prev
                        l_prev->setNext(dist(l_prev, l_newF));
                    }
                }
            }
            return l_ret;
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
                                                                   (reinterpret_cast<char*>(l_data) + l_data->getNext()), l_value, aloc));
                        }
                        break;
                    case meta::ARRAY:
                        {
                            l_value.SetArray();
                            auto l_data = static_cast<ArrayType*>(l_ptr);
                            //get the hash to optain the metadata
                            auto l_size = l_data->size();
                            l_ptr = buildArray(l_size, static_cast<char*>(l_ptr) + l_data->getNext(), l_value, aloc);
                        }
                        break;
                    case meta::INT:
                        {
                            //create the data
                            auto l_data = static_cast<BaseType<int64_t>*>(l_ptr);
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

        void* Page::buildArray(const int64_t& elemCount, void* start, rapidjson::Value& toAdd,
                               rapidjson::MemoryPoolAllocator<>& aloc)
        {
            auto l_ptr = start;
			ArrayItem<size_t>* l_element = static_cast<ArrayItem<size_t>*>(l_ptr);
            // we know every element which follows is a Array Element
            // we also know that toAdd is already a array!
            rapidjson::Value l_value;
            for (auto i = 0; i < elemCount; ++i)
            {
                switch (l_element->getType())
                {
                    case INT:
                        {
                            l_value = static_cast<ArrayItem<int64_t>*>(l_ptr)->getData();
                            toAdd.PushBack(l_value, aloc);
                        }
                        break;
                    case BOOL:
                        {
                            l_value = static_cast<ArrayItem<bool>*>(l_ptr)->getData();
                            toAdd.PushBack(l_value, aloc);
                        }
                        break;
                    case OBJECT:
                        break;
                    case ARRAY:
                        {
                            l_value.SetArray();
                            l_ptr = buildArray(static_cast<ArrayType*>(l_ptr)->size(), l_ptr, l_value, aloc);
                            toAdd.PushBack(l_value, aloc);
                        }
                        break;
                    case DOUBLE:
                        {
                            l_value = static_cast<ArrayItem<double>*>(l_ptr)->getData();
                            toAdd.PushBack(l_value, aloc);
                        }
                        break;
                    case STRING:
                        {
                            auto l_data = static_cast<ArrayItemString*>(l_ptr)->getString();
                            l_value.SetString(l_data->c_str(), l_data->length(), aloc);
                            toAdd.PushBack(l_value, aloc);
                        }
                        break;
                    case INVALID:
                        break;
                    default:
                        break;
                }
                //update the lptr to the next element;
                l_ptr = static_cast<char*>(l_ptr) + static_cast<ArrayItem<size_t>*>(l_ptr)->getNext();
            }
            return l_ptr;
        }
    }
}
