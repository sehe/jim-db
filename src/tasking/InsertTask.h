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

#pragma once
#include "Task.h"
#include "../rapidjson/document.h"

class InsertTask:public Task
{
public:
	explicit InsertTask(const std::shared_ptr<ClientHandle>& client, std::shared_ptr<rapidjson::Document> doc);

	~InsertTask();
	void execute() override;
private:
	std::shared_ptr<rapidjson::Document> m_doc;

	void processObject(rapidjson::Value& obj) const;
	void processArray(rapidjson::Value& obj) const;
	void processString(rapidjson::Value& obj) const;
	void processNumber(rapidjson::Value& obj) const;
};
