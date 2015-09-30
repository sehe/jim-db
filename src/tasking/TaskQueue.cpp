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

#include "TaskQueue.h"
#include <mutex>
TaskQueue TaskQueue::m_instance;

TaskQueue::TaskQueue() {}


TaskQueue::~TaskQueue() {}

TaskQueue& TaskQueue::getInstance()
{
	return m_instance;
}

void TaskQueue::push_pack(std::shared_ptr<Task> t)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_tasks.push_back(t);
	//notify that there is one more task, so one thread can work now
	m_cond.notify_one();
}

std::shared_ptr<Task> TaskQueue::pop_front()
{
	//regular lock so noone else acces this area now
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_tasks.size() == 0)
		m_cond.wait(lock);

	auto task = m_tasks.front();
	m_tasks.pop_front();
	return task;
}

int TaskQueue::size() const
{
	return m_tasks.size();
}
