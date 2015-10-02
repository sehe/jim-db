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
namespace jimdb
{
    namespace network
    {
        class IServer
        {
        public:
            virtual ~IServer() { }

            /**
            * accepts a new client if a new connection is available.
            * if not it does nothing.
            * If the client list is empty it does a blocking connection
            * \param[in] bool blocking if the accept should block
            */
            virtual int accept(const bool& blocking) = 0;

            /**
            *\brief start the server
            *
            *\author Benjamin Meyer
            *\date 30.09.2015 19:41
            */
            virtual bool start() = 0;
        };
    }
}