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
#include "../datatype/freetype.h"

namespace jimdb
{
    namespace memorymanagement
    {
        class Page
        {
        public:
            Page(long long header, long long body);
            ~Page();

            void setNext(const long long& id);
            long long getNext() const;

        private:
			//voidptr to memory to static cast as we like
            void* m_header;
			void* m_body;

            //pointer to the free typ chain start
            FreeType* m_free;
            //position of the free typ object start info
            long long* m_freepos;
            //holds the information of free space
            long long m_freeSpace;
            //holds the ID of the next page
            long long m_next;
            long long m_id;

            //id generation with static counter
            static long long m_s_idCounter;
        };
    }
}