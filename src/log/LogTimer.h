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
#include "LogMessage.h"
#include <chrono>
#include <utility>

/**
\brief Extends the regular LogMessage by a µs timer

It starts a timer and ends it when its distroyed. While it is Distroyed the
time between start and end is taken and appended to the log data to know the execution time of the
current scope.

Use it like this
\code
{
	LogTimer timer;
	timer << "something";
	//do something you want to mesure
}//here it will be logged
\endcode
\author Benjamin Meyer
*/
class LogTimer: public LogMessage
{
public:

    explicit LogTimer(const LoggerTypes &type, const std::string &file = "",
                      const int &i = 0);
    ~LogTimer();

    template<typename T> LogTimer &operator<<(const T &m);

    LogTimer(const LogTimer &other)
        : LogMessage(other),
          m_start(other.m_start)
    {
    }

    LogTimer(LogTimer &&other)
        : LogMessage(std::move(other)),
          m_start(std::move(other.m_start))
    {
    }

    LogTimer &operator=(const LogTimer &other)
    {
        if (this == &other)
            return *this;
        LogMessage::operator =(other);
        m_start = other.m_start;
        return *this;
    }

    LogTimer &operator=(LogTimer &&other)
    {
        if (this == &other)
            return *this;
        LogMessage::operator =(std::move(other));
        m_start = std::move(other.m_start);
        return *this;
    }

private:
    std::chrono::high_resolution_clock::time_point m_start;
};

template <typename T>
LogTimer &LogTimer::operator<<(const T &m)
{
    LogMessage::operator<<(m);
    return *this;
}
