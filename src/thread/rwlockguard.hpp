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
template<typename T>
inline RWLockGuard<T>::RWLockGuard(T& lock, const LockType& l) : m_lock(lock), m_type(l)
{
    switch (m_type)
    {
        case READ:
            m_lock.readLock();
            break;
        case WRITE:
            m_lock.writeLock();
            break;
    }
}

template<typename T>
inline RWLockGuard<T>::~RWLockGuard()
{
    switch (m_type)
    {
        case READ:
            m_lock.readUnlock();
            break;
        case WRITE:
            m_lock.writeUnlock();
            break;
    }
}