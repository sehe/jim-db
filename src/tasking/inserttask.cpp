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

#include "InsertTask.h"
#include "../log/logger.h"

InsertTask::InsertTask(const std::shared_ptr<IClient>& client, std::shared_ptr<rapidjson::Document> doc): Task(client),m_doc(doc) {}

InsertTask::~InsertTask() {}

void InsertTask::execute()
{
	//we know that it is insert and it contains a value
	auto& value = m_doc->operator[]("data");
	if (!value.IsObject())
	{
		//throw error to client since its not a obj!

		return;
	}

	for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it)
	{
		LOG_DEBUG << it->name.GetString();
		if (it->value.IsObject())
		{
			processObject(it->value);
		}
	}
}

void InsertTask::processObject(rapidjson::Value& obj) const
{
	for (auto it = obj.MemberBegin(); it != obj.MemberEnd(); ++it)
	{
		auto& value = it->value;
		if (value.IsString())
		{
			LOG_DEBUG << it->name.GetString() << ":" << it->value.GetString();
			processString(it->value);
			continue;
		}

		if (value.IsNumber())
		{
			LOG_DEBUG << it->name.GetString() << ":" << it->value.GetDouble();
			processNumber(value);
			continue;
		}

		if (value.IsObject())
		{
			processObject(value);
			continue;
		}

		if (value.IsArray())
		{
			processArray(value);
			continue;
		}
	}
}

void InsertTask::processArray(rapidjson::Value& obj) const {}

void InsertTask::processString(rapidjson::Value& obj) const {}

void InsertTask::processNumber(rapidjson::Value& obj) const {}
