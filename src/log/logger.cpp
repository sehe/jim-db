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

#include "logger.h"
#include "../common/configuration.h"
#include <string>
#include <sstream>
#include <mutex>
#include <iostream>

namespace jimdb
{
    namespace common
    {
        std::ofstream* Logger::m_file = nullptr;
        Logger Logger::m_instance;
        tasking::SpinLock Logger::m_lock;
        const std::string Logger::DEFAULT_LOG_FILE = "defaultLogFile.txt";

        LogMessage Logger::Log(LoggerTypes type, const std::string& file, const int& i)
        {
            return LogMessage(type, file, i);
        }

        LogTimer Logger::Timer(const std::string& file, const int& i)
        {
            return LogTimer(TIMER, file, i);
        }

        Logger& Logger::getInstance()
        {
            if (m_file == nullptr)
                //use a fixed name so it can always be used!
                setLogFile("startup.log");
            return m_instance;
        }

        void Logger::setLogLevel(const int& i)
        {
            m_logLevel = i;
        }

        int Logger::getLogLevel() const
        {
            return m_logLevel;
        }

        void Logger::setLogFile(const std::string& filename)
        {
            if (m_file != nullptr)
            {
                m_file->flush();
                delete m_file;
            }
            m_file = new std::ofstream(filename, std::ofstream::out | std::ofstream::app);
        }

        void Logger::operator<<(const std::ostringstream& message) const
        {
            std::lock_guard<tasking::SpinLock> lock(m_lock);
            std::cout << message.str() << "\n";
            if (m_file != nullptr)
            {
                *m_file << message.str() << "\n";
                m_file->flush();
            }
        }

        Logger::~Logger()
        {
            //clean up
            if(m_file != nullptr)
                m_file->flush();
            delete m_file;
        }
    }
}
