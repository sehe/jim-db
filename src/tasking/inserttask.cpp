#include "inserttask.h"
#include "../meta/metaindex.h"

namespace jimdb
{
    namespace tasking
    {
        InsertTask::InsertTask(const std::shared_ptr<network::IClient>& client,
                               const std::shared_ptr<network::Message> m) : Task(client), m_msg(m) {}


        void InsertTask::operator()()
        {
            //get the object first
            //we already know that its a valid data and valid document here!
            auto& data = (*m_msg)()["data"];

            auto it = data.MemberBegin();
            if (it->value.IsObject())
            {
                LOG_DEBUG << "Object to insert name: " << it->name.GetString();
            }
            else
            {
                LOG_WARN << "Insert: data field is missing Object member";
                return;
            }

            rapidjson::GenericValue<rapidjson::UTF8<>>& objectToInsert =  it->value;
            auto objectName = it->name.GetString();
            //generate meta data:
            auto& meta = meta::MetaIndex::getInstance();
            //insert new obj
            insertMeta(objectName, objectToInsert);

        }

        void InsertTask::insertMeta(const std::string& name, const rapidjson::GenericValue<rapidjson::UTF8<>>& value) const
        {
            auto& meta = meta::MetaIndex::getInstance();
            //check if we need to insert a new meta
            //scip this part if it already contains
            if (!meta.contains(common::FNVHash()(name)))
            {
                auto newMeta = std::make_shared<meta::MetaData>(name);
                for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it)
                {
                    switch(it->value.GetType())
                    {
                        case rapidjson::kNullType:
                            LOG_WARN << "null type: " << it->name.GetString();
                            break;

                        case rapidjson::kFalseType:
                        case rapidjson::kTrueType:
                            newMeta->add(it->name.GetString(), meta::BOOL);
                            break;

                        case rapidjson::kObjectType:
                            //add to the current meta
                            newMeta->add(it->name.GetString(), meta::OBJECT);
                            //now check if the obj already exsist
                            //else create it or skip
                            insertMeta(it->name.GetString(), it->value);
                            break;

                        case rapidjson::kArrayType:
                            newMeta->add(it->name.GetString(), meta::ARRAY);
                            break;

                        case rapidjson::kStringType:
                            newMeta->add(it->name.GetString(), meta::STRING);
                            break;

                        case rapidjson::kNumberType:
                            if (it->value.IsInt())
                                newMeta->add(it->name.GetString(), meta::INT); //number
                            else
                                newMeta->add(it->name.GetString(), meta::DOUBLE);//floatingpoint
                            break;
                        default:
                            LOG_WARN << "Unknown member Type: " << it->name.GetString() << ":" << it->value.GetType();
                            break;
                    }
                }
                //add the new metadata to the meta;
                meta.add(newMeta->getHash(), newMeta);
            }
        }
    }
}