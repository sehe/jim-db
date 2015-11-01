#include "findtask.h"
#include "../index/objectindex.h"

namespace jimdb
{
    namespace tasking
    {
        FindTask::FindTask(const std::shared_ptr<network::IClient>& client,
                           const std::shared_ptr<network::Message> m) : Task(client), m_msg(m) {}

        void FindTask::operator()()
        {
            //optain the oid
            auto& l_data = (*m_msg)()["data"];
            if(l_data.FindMember("oid__") == l_data.MemberEnd())
            {
                LOG_WARN << "invalid find task. no oid__";
                return;
            }

            if(!l_data["oid__"].IsInt64())
            {
                LOG_WARN << "invalid find task. oid__ is no int";
                return;
            }

            auto l_oid = l_data["oid__"].GetInt64();

            LOG_DEBUG << "looking for oid__: " << l_oid;

            if(!index::ObjectIndex::getInstance().contains(l_oid))
            {
                LOG_WARN << "invalid find task. oid not found";
                return;
            }

            auto& l_meta = index::ObjectIndex::getInstance()[l_oid];
            LOG_DEBUG << l_oid << " found on page: " << l_meta.m_page << " header pos: " << l_meta.m_pos;
        }
    }
}