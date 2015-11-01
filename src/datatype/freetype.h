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
#include "stringtype.h"
namespace jimdb
{
    namespace memorymanagement
    {
        /**
        \brief Type to reserve Free space

        Take care the data field always
        contains -sizeof(FreeType) "free" value,
        since this get added to the end.
        @author Benjamin Meyer
        @date 01.11.2015 10:34
        */
        class FreeType : public StringType
        {
        public:

            explicit inline FreeType(const size_t& size);
            // returns the "free space" it does include
            // the size of this
            inline size_t getFree() const;
        };
#include "freetype.hpp"
    }
}