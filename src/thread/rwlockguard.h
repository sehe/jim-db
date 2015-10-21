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
#include "rwlock.h"

namespace jimdb
{
    namespace tasking
    {
        enum LockType
        {
            READ,
            WRITE
        };

        /**
        \brief a lockguard for R/W Locks

        Can be used with all classes that have a
        readLock/readUnlock and a writeLock/writeUnlock

        @author Benjamin Meyer
        @date 21.10.2015 15:21
        */
        template<typename T = RWLock>
        class RWLockGuard
        {
        public:
            inline RWLockGuard(T& lock, const LockType& l);
            inline ~RWLockGuard();

        private:
            RWLockGuard(const RWLockGuard& ) = delete;
            RWLockGuard& operator=(const RWLockGuard& ) = delete;

            T& m_lock;
            LockType m_type;
        };
#include "rwlockguard.hpp"
    }
}