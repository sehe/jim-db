#include "handshake.h"
#include "../log/logger.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "taskqueue.h"
#include "requesttask.h"

class RequestTask;

HandshakeTask::HandshakeTask(const std::shared_ptr<IClient> client) : Task(client) { }

void HandshakeTask::execute()
{
	rapidjson::Document doc;
	doc.SetObject();

	doc.AddMember("type", "handshake", doc.GetAllocator());
	doc.AddMember("data", "hi", doc.GetAllocator());

	// Convert JSON document to string
	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	doc.Accept(writer);

	auto handshake = std::make_shared<std::string>(strbuf.GetString());
	//sending a handshake HI and wait 1s to return a hi as shake
	m_client->send(handshake);
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
