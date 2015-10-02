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

#include "parsetask.h"

ParseTask::ParseTask(std::shared_ptr<IClient> client, std::shared_ptr<Message> s) :Task(client)
{
	m_message = s;
}

void ParseTask::execute()
{
	/**
	auto doc = std::make_shared<rapidjson::Document>();
	{
		LOG_SCOPE_TIME << "Parsing and took: " << m_message->getMessage();
		doc->ParseInsitu(m_message->getMessage());
	}

	if (doc->HasParseError())
	{
		LOG_ERROR << "Failed parsing: " << *m_message->getMessage();
		//TODO return some error to client
	}

	//check if data and type exists
	if (doc->FindMember("type") == doc->MemberEnd() || doc->FindMember("data") == doc->MemberEnd())
	{
		LOG_DEBUG << "invalid JSON. Missing type or Value";
		//TODO send some information
		return;
	}

	if(!doc->operator[]("data").IsObject())
	{
		LOG_DEBUG << "invalid JSON. Data is no object";
		//TODO send some information
		return;
	}

	if (doc->FindMember("type")->value == "insert")
	{
		LOG_DEBUG << "inserting";
		//TaskQueue::getInstance().push_pack(std::make_shared<InsertTask>(m_client, doc));
	}
	*/
}


ParseTask::~ParseTask() {}
