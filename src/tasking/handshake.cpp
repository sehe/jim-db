#include "handshake.h"
#include "../log/logger.h"
#include "taskqueue.h"
#include "requesttask.h"
#include "../network/messagefactory.h"

class RequestTask;
namespace jimdb
{
    namespace tasking
    {
        HandshakeTask::HandshakeTask(const std::shared_ptr<network::IClient> client) : Task(client) { }

        void HandshakeTask::operator()()
        {
            //sending a handshake HI and wait 1s to return a hi as shake
            m_client->send(network::MessageFactory().generate(network::HANDSHAKE));
            if (!m_client->hasData())
            {
                LOG_WARN << "handshake Failed";
                m_client->close(); //close the soc
                return; //return on failur
            }
            auto mes = m_client->getData();
            //check if handshaje is valid
            if (std::string("hi") == mes->operator()()["data"].GetString())
                LOG_DEBUG << "Handshake successful";
            else
            {
                LOG_WARN << "handshake Failed";
                m_client->close(); //close the soc
                return; //return on failur
            }
            //if handshake is valid do something
            TaskQueue::getInstance().push_pack(std::make_shared<RequestTask>(m_client));
        }
    }
}