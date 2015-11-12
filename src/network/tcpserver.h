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

//link against winsock2 32 lib
#pragma comment(lib, "Ws2_32.lib") 

#include "../tasking/taskqueue.h"
#include "iserver.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

namespace jimdb
{
    namespace network
    {
        class TCPServer : public IServer
        {
        public:
            explicit TCPServer(tasking::TaskQueue& q);
            ~TCPServer();
            /**
            * Startup the listenting to a port
            * \return true if started successfull false if not. try restarting it
            */
            bool start() override;

            /**
            * accepts a new client if a new connection is available.
            * if not it does nothing.
            * If the client list is empty it does a blocking connection
            * \param[in] bool blocking if the accept should block
            */
            int accept(const bool& blocking) override;
        private:

            /**
            \brief check for new connections

            \return true if there is something to read in the socket. so new connection
            \author Benjamin Meyer
            \date 14.09.2015 09:04
            */
            bool checkConnection();

            //the listen socket
            SOCKET m_listensocket;
            fd_set m_set;
        };
    }
}
