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

#include "ParseTask.h"
#include "../rapidjson/document.h"
#include "../log/Logger.h"
#include "TaskQueue.h"
#include "ClientPollTask.h"

ParseTask::ParseTask(std::shared_ptr<ClientHandle> client, std::shared_ptr<std::string> s) :Task(client)
{
	m_string = s;
}

void ParseTask::execute()
{
	LOG_SCOPE_TIME << "Parsing and took: " << *m_string;
	auto doc = std::make_shared<rapidjson::Document>();
	doc->Parse(m_string->c_str());
	if (doc->HasParseError())
	{
		LOG_ERROR << "Failed parsing: " << *m_string;
		//TODO return some error to client
	}
	//todo generate new task with taskFactory
	//TODO validate

	//we are done so current just make a poll task again
	TaskQueue::getInstance().push_pack(std::make_shared<ClientPollTask>(m_client));
}


ParseTask::~ParseTask()
{
}
