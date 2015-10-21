// /**
// ############################################################################
// # GPL License                                                              #
// #                                                                          #
// # This file is part of the JIM-DB.                                         #
// # Copyright (c) 2015, Benjamin Meyer, <benjamin.meyer@tu-clausthal.de>     #
// # This program is free software: you can redistribute it and/or modify     #
// # it under the terms of the GNU General Public License as                  #
// # published by the Free Software Foundation, either version 3 of the       #
// # License, or (at your option) any later version.                          #
// #                                                                          #
// # This program is distributed in the hope that it will be useful,          #
// # but WITHOUT ANY WARRANTY; without even the implied warranty of           #
// # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
// # GNU General Public License for more details.                             #
// #                                                                          #
// # You should have received a copy of the GNU General Public License        #
// # along with this program. If not, see <http://www.gnu.org/licenses/>.     #
// ############################################################################
// **/
#pragma once
#include <mutex>
namespace jimdb
{
    namespace tasking
    {
        /**
        @brief a read write lock

        A simple/naiv read write lock which notifys all if lock is free again.

        Take care it is not fair! Readlock can starve write lock!

        @see http://stackoverflow.com/questions/12033188/how-would-a-readers-writer-lock-be-implemented-in-c11
        @author Benjamin Meyer
        @date 21.10.2015 14:19
        */
        class RWLock
        {
        public:
            inline RWLock();
            inline void readLock();
            inline void readUnlock();
            inline void writeLock();
            inline void writeUnlock();
        private:
            int m_reader;
            bool m_writing;

            std::condition_variable m_cond;
            std::mutex m_mutex;
        };
#include "rwlock.hpp"
    }
}