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

#include "taskqueue.h"
#include <mutex>

namespace jimdb
{
    namespace tasking
    {
        TaskQueue TaskQueue::m_instance;

        TaskQueue::TaskQueue(): m_maxSize(0) {}

        TaskQueue::~TaskQueue() {}

        TaskQueue& TaskQueue::getInstance()
        {
            return m_instance;
        }

        void TaskQueue::setMaxSize(const size_t& size)
        {
			//make sure noone else uses the queue obj right now
			std::unique_lock<std::mutex> lock(m_mutex);
            m_maxSize = size;
        }

        size_t TaskQueue::size() const
        {
            return m_tasks.size();
        }
    }
}