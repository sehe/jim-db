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
#include <memory>
#include "task.h"
#include "../network/iclient.h"

/**
\brief A simple task to Parse a json request

\author Benjamin Meyer
\date 15.09.2015 14:23
*/
class ParseTask: public Task
{
public:
	ParseTask(std::shared_ptr<IClient> client, std::shared_ptr<Message> s);
	~ParseTask();

	void execute() override;
private:
	std::shared_ptr<Message> m_message;
};
