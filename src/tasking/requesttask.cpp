/**
############################################################################
# GPL License                                                              #
#                                                                          #
# This file is part of the JIM - DB.                                         #
# Copyright(c) 2015, Benjamin Meyer, <benjamin.meyer@tu-clausthal.de>     #
# This program is free software : you can redistribute it and / or modify     #
# it under the terms of the GNU General Public License as                  #
# published by the Free Software Foundation, either version 3 of the       #
# License, or (at your option) any later version.                          #
#                                                                          #
# This program is distributed in the hope that it will be useful, #
# but WITHOUT ANY WARRANTY; without even the implied warranty of           #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the            #
# GNU General Public License for more details.                             #
#                                                                          #
# You should have received a copy of the GNU General Public License        #
# along with this program.If not, see <http://www.gnu.org/licenses/>.     #
############################################################################
**/


#include "requesttask.h"
#include "../log/logger.h"

namespace jimdb
{
	namespace tasking
	{
		RequestTask::RequestTask(const std::shared_ptr<network::IClient> client) : Task(client) { }

		void RequestTask::operator()()
		{
			//this tasks follows right after a handshake so we should have data
			//to be safe check for data
			if (!m_client->hasData())
				return;

			auto message = m_client->getData();

			//check the json
			auto& doc = (*message)();
			if (doc.FindMember("type") == doc.MemberEnd() || doc.FindMember("data") == doc.MemberEnd())
			{
				LOG_WARN << "invalid JSON";
				//TODO send info back
				return;
			}

			if (!doc["type"].IsString() || !doc["data"].IsObject())
			{
				LOG_WARN << "invalid JSON";
				//todo send info back
				return;
			}

			if (doc["type"].GetString() == std::string("insert"))
			{
				LOG_DEBUG << "insert message";

				return;
			}

			if (doc["type"].GetString() == std::string("delete"))
			{
				LOG_DEBUG << "delete message";

				return;
			}

			if (doc["type"].GetString() == std::string("query"))
			{
				LOG_DEBUG << "query message";

				return;
			}
			//todo do something with the request
		}
	}
}
