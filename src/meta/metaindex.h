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
#include "../index/index.h"
#include "metadata.h"

namespace jimdb
{
    namespace meta
    {
        /**
        \brief Typedef a index to a MetaManager

        A Manager for the meta Data is simply just a index of the
        hash and the shared_ptr to the MetaData which should be threadsafe again.

        @author Benjamin Meyer
        @date 22.10.2015 14:00
        */
        class MetaIndex : public index::Index<size_t, std::shared_ptr<MetaData>>
        {
        public:
            static MetaIndex& getInstance();
        private:
            static MetaIndex m_instance;
        };
    }
}