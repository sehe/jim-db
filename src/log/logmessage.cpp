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

#include "logmessage.h"
#include "logger.h"
#include <ctime>
#include <thread>
namespace jimdb {
	namespace common {
		std::map<LoggerTypes, std::string> LogMessage::m_enum_string_mapping = { {DEBUG, "debug"},{INFO, "info"},{WARNING, "warn"},{ERROR_L, "error"},{EXCAPTION, "excaption"},{TIMER, "timer"} };

		LogMessage::LogMessage(const LoggerTypes& type, const std::string& file,
			const int& i) : m_stream(), m_type(type)
		{
			//build the logstring
			m_stream << "[" + m_enum_string_mapping[type] + "][File:" + file + "][Line:" +
				std::to_string(i) + "][thread:" << std::this_thread::get_id() <<
				"]" + currentDateTime() + " ";
		}

		LogMessage::~LogMessage()
		{
			//push the message at the end
			if (Logger::getInstance().getLogLevel() >= m_type)
				Logger::getInstance() << m_stream;
		}

		// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
		std::string LogMessage::currentDateTime() const
		{
			auto now = time(nullptr);
			struct tm tstruct;
			char buf[80];
			tstruct = *localtime(&now);
			strftime(buf, sizeof(buf), "[%d-%m-%Y][%X]", &tstruct);
			return buf;
		}

		LogMessage::LogMessage(const LogMessage& other) :m_type(other.m_type)
		{
			m_stream << other.m_stream.str();
		}

		LogMessage::LogMessage(LogMessage&& other) : m_stream(std::move(other.m_stream)), m_type(other.m_type) {}

		LogMessage& LogMessage::operator=(const LogMessage& other)
		{
			if (this == &other)
				return *this;
			m_stream << other.m_stream.str();
			m_type = other.m_type;
			return *this;
		}

		LogMessage& LogMessage::operator=(LogMessage&& other)
		{
			if (this == &other)
				return *this;
			m_stream = std::move(other.m_stream);
			m_type = other.m_type;
			return *this;
		}
	}
}