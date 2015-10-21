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

inline RWLock::RWLock() : m_reader(0), m_writing(false) {}

inline void RWLock::readLock()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_writing)
        m_cond.wait(lock);
    m_reader++;
}

inline void RWLock::readUnlock()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_reader--;
    if (!m_reader)
        m_cond.notify_all();
}

inline void RWLock::writeLock()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_writing || m_reader)
        m_cond.wait(lock);
    m_writing = true;
}

inline void RWLock::writeUnlock()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_writing = false;
    m_cond.notify_all();
}