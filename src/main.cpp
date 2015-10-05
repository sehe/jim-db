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

#include "network/tcpserver.h"
#include "log/logger.h"
#include <thread>
#include <list>
#include "common/configuration.h"
#include "tasking/taskqueue.h"
#include "thread/worker.h"
#pragma comment(lib,"user32.lib")
#include <iostream>
#include "common/CmdArgs.h"

//forward declare
BOOL WINAPI ConsoleHandler(DWORD CEvent);

int main(int argc, char* argv[])
{
    auto& args = jimdb::common::CmdArgs::getInstance();
    args.init(argc, argv);

    if (args.contains("-h"))
        std::cout << "todo print some help if help is needed!" << std::endl;

    if (args.contains("-generate"))
    {
        std::ofstream file(args["-generate"]);
        if (!file || !file.is_open())
            throw std::runtime_error("couldn't generate config");
        file << jimdb::common::Configuration::getInstance().generate();
        file.flush();
        std::cout << "generated example configuration" << std::endl;
        return EXIT_SUCCESS;
    }

    //get the config instance
    auto& cfg = jimdb::common::Configuration::getInstance();
    cfg.init(args["-config"]);

    //set the loglevel of the config or the default log level
    jimdb::common::Logger::getInstance().setLogLevel(cfg.getInt(jimdb::common::LOG_LEVEL));
    LOG_INFO << cfg; //print out the config

    //setup the console handle:
    if (SetConsoleCtrlHandler(
                static_cast<PHANDLER_ROUTINE>(ConsoleHandler), TRUE) == FALSE)
    {
        LOG_WARN << "Unable to install console handler!";
    }

    auto& tasks = jimdb::tasking::TaskQueue::getInstance();
    std::shared_ptr<jimdb::network::IServer> tcpServer = std::make_shared<jimdb::network::TCPServer>(tasks);
    tcpServer->start();
    //start the workers
    auto threads = cfg.getInt(jimdb::common::THREADS);
    //if the config value is 0 take hardware conc.
    if (threads == 0)
    {
        threads = std::thread::hardware_concurrency() - 1; //since the mainthread
    }

    LOG_INFO << "Starting: " << threads + 1 << " Workers";
    std::list<std::unique_ptr<jimdb::tasking::Worker>> m_workers;
    for (auto i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        m_workers.push_back(std::make_unique<jimdb::tasking::Worker>(tasks));
    }
    //use this as acceptor and handshaker
    //do nothing else here!
    while (true)
    {
        tcpServer->accept(true); //call accept blocking
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
