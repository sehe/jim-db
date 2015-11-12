﻿#include "inserttask.h"
#include "../meta/metaindex.h"
#include "../index/pageindex.h"
#include "../datatype/arrayitem.h"
#include "../common/configuration.h"
#include "../network/messagefactory.h"
#include "../assert.h"

namespace jimdb
{
    namespace tasking
    {
        InsertTask::InsertTask(const std::shared_ptr<network::IClient>& client,
                               const std::shared_ptr<network::Message> m) : Task(client), m_msg(m) {}


        void InsertTask::operator()()
        {
            //insert into page
            //we already know that its a valid data and valid document here!
            auto& dat = (*m_msg)()["data"];
            auto it = dat.MemberBegin();
            if (!it->value.IsObject())
            {
                LOG_WARN << "Insert: data field is missing Object member";
                return;
            }

            //insert it to meta and get the size of the data in memory
            auto l_objSize = checkSizeAndMeta(it->name.GetString(), it->value);

            //Now estimate the next page which has space for l_objSize

            // get a page which could fit the obj and which is not locked
            // if its locked we create a new page
            // so there are depending on the machine up to cpu max
            // locked pages
            auto l_page = index::PageIndex::getInstance().find(l_objSize);

            if(l_page == nullptr)
            {
                //if the ptr is still nullptr we need to create a new Page
                //well does not fit in any page
                auto& cfg = common::Configuration::getInstance();
                l_page = std::make_shared<memorymanagement::Page>(cfg[common::PAGE_HEADER].GetInt64(),
                         cfg[common::PAGE_BODY].GetInt64());
                //add the new page
                index::PageIndex::getInstance().add(l_page->getID(), l_page);
            }
            //insert the obj to the page
            auto oid = l_page->insert(dat);

            //generate answer and return it.
            network::MessageFactory factory;
            m_client->send(factory.generateResultInsert(oid));
        }

        size_t InsertTask::checkSizeAndMeta(const std::string& name, const rapidjson::GenericValue<rapidjson::UTF8<>>& value)
        {
            auto& meta = meta::MetaIndex::getInstance();
            auto l_metaExsist = meta.contains(common::FNVHash()(name));
            size_t l_objSize = 0;

            //check if we need to insert a new meta
            //scip this part if it already contains
            auto newMeta = std::make_shared<meta::MetaData>(name);

            //if not still iterate for size
            for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it)
            {
                switch (it->value.GetType())
                {
                    case rapidjson::kNullType:
                        LOG_WARN << "null type: " << it->name.GetString();
                        break;

                    case rapidjson::kFalseType:
                    case rapidjson::kTrueType:
                        {
                            if (!l_metaExsist)
                                newMeta->push_back({ it->name.GetString(), meta::BOOL });
                        }
                        break;

                    case rapidjson::kObjectType:
                        {
                            //add to the current meta
                            if (!l_metaExsist)
                                newMeta->push_back({ it->name.GetString(), meta::OBJECT });
                            //now check if the obj already exsist
                            //else create it or skip
                            l_objSize += checkSizeAndMeta(it->name.GetString(), it->value);
                            //also add the size of the new obj to it
                        }
                        break;

                    case rapidjson::kArrayType:
                        {
                            if (!l_metaExsist)
                                newMeta->push_back({ it->name.GetString(), meta::ARRAY });

                            l_objSize += checkSizeArray(it->value);
                        }
                        break;

                    case rapidjson::kStringType:
                        {
                            if (!l_metaExsist)
                                newMeta->push_back({ it->name.GetString(), meta::STRING });
                            //add the length of the string
                            l_objSize += strlen(it->value.GetString());
                        }
                        break;

                    case rapidjson::kNumberType:
                        {
                            if (!l_metaExsist)
                            {
                                if (it->value.IsInt())
                                    newMeta->push_back({ it->name.GetString(), meta::INT }); //number
                                else
                                    newMeta->push_back({ it->name.GetString(), meta::DOUBLE });//floatingpoint

                            }
                            break;
                        default:
                            LOG_WARN << "Unknown member Type: " << it->name.GetString() << ":" << it->value.GetType();
                            break;
                        }
                }
                //add the general size of a regular object
                //since it get added for every object
                //for example every inner object has a filed with the obj id
                //every array has a filed with the count of inner objects and so on.
                //so all we do is adding the base and the rest get added in the loop above
                l_objSize += sizeof(memorymanagement::BaseType<size_t>);

            }

            //add the new metadata to the meta;
            if (!l_metaExsist)
                meta.add(newMeta->getHash(), newMeta);

            return l_objSize;
        }


        size_t InsertTask::checkSizeArray(const rapidjson::GenericValue<rapidjson::UTF8<>>& val)
        {
            //we got a array over here
            size_t l_arrSize = 0;

            //not member begin since we are in a array!
            //only values here!
            for (auto it = val.Begin(); it != val.End(); ++it)
            {
                switch (it->GetType())
                {
                    //nothing todo here
                    case rapidjson::kNullType:
                    case rapidjson::kFalseType:
                    case rapidjson::kTrueType:
                    case rapidjson::kNumberType:
                        break;

                    case rapidjson::kObjectType:
                        //so now we got an object without a name!!!

                        //now check if the obj already exsist
                        //else create it or skip
                        //TODO FIX THIS!!! NEED A ID HERE
                        l_arrSize += checkSizeAndMeta("", *it);
                        //also add the size of the new obj to it
                        break;

                    case rapidjson::kArrayType:
                        l_arrSize += checkSizeArray(*it);
                        break;

                    case rapidjson::kStringType:
                        //add the length of the string
                        l_arrSize += strlen(it->GetString());
                        break;
                    default:
                        LOG_WARN << "Unknown member Type in array ";
                        break;
                }
                l_arrSize += sizeof(memorymanagement::ArrayItem<long long>);
            }
            return l_arrSize;
        }
    }
}
