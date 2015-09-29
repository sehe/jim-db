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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")

#include "ClientHandle.h"
#include <vector>
#include "../tasking/TaskQueue.h"

class TCPServer
{
public:
	explicit TCPServer(TaskQueue& q);
	~TCPServer();
	/**
    * Startup the listenting to a port
    * \return true if started successfull false if not. try restarting it
    */
	bool start();

	/**
    * accepts a new client if a new connection is available.
    * if not it does nothing.
	* If the client list is empty it does a blocking connection
	* \param[in] bool blocking if the accept should block
    */
	int accept(const bool& blocking);
private:

	/**
	\brief check for new connections

	\return true if there is something to read in the socket. so new connection
	\author Benjamin Meyer
	\date 14.09.2015 09:04
	*/
	bool checkConnection();

	static const char DEFAULT_PORT[];
	static const int DEFAULT_BUFFER_SIZE;

	//the listen socket
	SOCKET m_listensocket;
	fd_set m_set;
};
