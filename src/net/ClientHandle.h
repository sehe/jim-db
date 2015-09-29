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
#include <WinSock2.h>
#include <ostream>
#include<memory>

/**
\breif A Simple client handler

It allows to receive and send data to the underlaying sock
Make sure not to WSACleanup here! This would terminate the whole service.

\author Benjamin Meyer
*/
class ClientHandle
{
public:
	explicit ClientHandle(const SOCKET& s,const sockaddr_storage& add);
	~ClientHandle();

	/**
    * It need to be nullterminating!
    */
	bool operator<<(std::shared_ptr<std::string> s);
	bool send(std::shared_ptr<std::string> s);
	bool hasData();
	bool isConnected() const;

	/**
	\brief Get data from client

	\return returns false if getData failed or the cleint disconnected
	\author Benjamin Meyer
	\date 09.09.2015 14:00
	*/
	bool getData(std::shared_ptr<std::string> ptr);

	int getSockID() const;

private:
	SOCKET m_sock;
	sockaddr_storage m_addr;
	std::string m_user;
	std::string m_address;
	bool m_connected;

	bool checkRetValRecv(const int& i);
	bool handShake();
};
