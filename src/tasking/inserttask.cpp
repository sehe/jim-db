#include "inserttask.h"
#include "../meta/metaindex.h"
#include "../index/pageindex.h"

namespace jimdb
{
    namespace tasking
    {
        InsertTask::InsertTask(const std::shared_ptr<network::IClient>& client,
                               const std::shared_ptr<network::Message> m) : Task(client), m_msg(m) {}


        void InsertTask::operator()()
        {
            //TODO fix this!
            index::PageIndex::getInstance();
            long long id = 0;
            if (index::PageIndex::getInstance().empty())
            {
                auto p = std::make_shared<memorymanagement::Page>(512, 512);
                id = p->getID();
                index::PageIndex::getInstance().add(p->getID(), p);
            }

            //insert into page
            //we already know that its a valid data and valid document here!
            index::PageIndex::getInstance()[id]->insert((*m_msg)()["data"]);

        }
    }
}