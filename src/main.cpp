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

/** \mainpage Wellcome to JIM-DB

\section Introduction

JIM-DB stands for JSON-In-Memory database. It is a System to store objects inside
of memory and allow to querry them.
\author Benjamin Meyer
\date DATE
*/

#include "net/TCPServer.h"
#include "log/Logger.h"
#include <thread>
#include <list>
#include "configuration.h"
#include "tasking/TaskQueue.h"
#include "thread/Worker.h"
#include "dataTyps/StringType.h"
#include "dataTyps/BaseType.h"
#pragma comment(lib,"user32.lib")
#include <iostream>
#include "assert.h"
//forward declare
BOOL WINAPI ConsoleHandler(DWORD CEvent);

int main(int argc, char* argv[])
{
	std::string configFile = "config.cfg";
	if (argc > 1)
		configFile = argv[1];
	//get the config instance
	auto& cfg = Configuration::getInstance();
	if (!cfg.parse(configFile))
	{
		std::cout << "Missing a config file!" << std::endl;
		return 0;
	}
	//set the loglevel of the config or the default log level
	Logger::getInstance().setLogLevel(cfg.getInt(LOG_LEVEL));
	LOG_INFO << cfg; //print out the config

	//setup the console handle:
	if (SetConsoleCtrlHandler(
		static_cast<PHANDLER_ROUTINE>(ConsoleHandler), TRUE) == FALSE)
	{
		LOG_WARN << "Unable to install console handler!";
	}

	
	//checking type
	char buffer[100];
	auto typ0 = new(&buffer) StringType("Testing a Type");
	//put a second behind the first. since the first is 16bit + text ... don't forget the class itself!
	auto typ1 = new(&buffer[14 + sizeof(StringType)]) StringType("One more Type");
	auto typ2 = new(&buffer[27 + 2 * sizeof(BaseType<double>)]) BaseType<double>(22.15);
	LOG_INFO << *typ0->getString();
	LOG_INFO << *typ1->getString();
	LOG_INFO << typ2->getData(); 
	LOG_INFO << typ0->getData(); //size of type0
	//no need for deletion since buffer will be deleted and everything is inside the buffer.
	

	auto& tasks = TaskQueue::getInstance();

	TCPServer tcpServer(tasks);
	tcpServer.start();

	//start the workers
	auto threads = cfg.getInt(THREADS);
	//if the config value is 0 take hardware conc.
	if (threads == 0)
	{
		threads = std::thread::hardware_concurrency() - 1; //since the mainthread
	}

	LOG_INFO << "Starting: " << threads + 1 << " Workers";
	std::list<std::unique_ptr<Worker>> m_workers;
	for (auto i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		m_workers.push_back(std::make_unique<Worker>(tasks));
	}

	//use this as acceptor and handshaker 
	//do nothing else here!
	while (true)
	{
		tcpServer.accept(true); //call accept blocking
	}
}


BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch (CEvent)
	{
	case CTRL_CLOSE_EVENT:
		MessageBox(nullptr,
		           "Program being closed!", "CEvent", MB_OK);
	}
	return true;
}
