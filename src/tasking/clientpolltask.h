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
#include "../network/clienthandle.h"
#include "Task.h"

/**
\brief Polling Task for clients

Checks for data. If there are some it creates a parsetask out of it. 
if there are none it pushs a a new polltask with the same client to the list.

\author Benjamin Meyer
\date 15.09.2015 14:27
*/
class ClientPollTask : public Task
{
public:
	explicit ClientPollTask(std::shared_ptr<IClient> client);
	~ClientPollTask();

	void execute() override;
};