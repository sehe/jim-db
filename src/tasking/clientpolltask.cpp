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

#include "clientpolltask.h"
#include "taskqueue.h"
#include "parsetask.h"


ClientPollTask::ClientPollTask(std::shared_ptr<IClient> client):Task(client) {}


ClientPollTask::~ClientPollTask() {}


void ClientPollTask::execute()
{
	if (!m_client->isConnected())
	{
		//nothing todo here so the task is over
		return;
	}
	//we are connected check for data
	if (m_client->hasData())
	{
		//get into the next task which is parsing
		TaskQueue::getInstance().push_pack(std::make_shared<ParseTask>(m_client, m_client->getData()));
		return;
	}
	TaskQueue::getInstance().push_pack(std::make_shared<ClientPollTask>(m_client));
}
