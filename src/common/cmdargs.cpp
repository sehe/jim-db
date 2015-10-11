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

#include "CmdArgs.h"
#include <iostream>
namespace jimdb
{
    namespace common
    {
        CmdArgs CmdArgs::m_instance;

        void CmdArgs::init(const int &argc, char* argv[])
        {
            //pushback all values
            m_args.insert(m_args.end(), argv + 1, argv + argc);
        }

        CmdArgs& CmdArgs::getInstance()
        {
            return m_instance;
        }

        CmdArgs::~CmdArgs() {}

        std::string CmdArgs::operator[](const std::string& v)
        {
            auto it = find(m_args.begin(), m_args.end(), v);
            if(it != m_args.end() && ++it != m_args.end())
            {
                return *it; //return the value
            }
            auto error = v;
            error +=  " needs a second parameter! -config FILENAME";
            throw std::runtime_error(error);
        }

        bool CmdArgs::contains(const std::string& v)
        {
            if (find(m_args.begin(), m_args.end(), v) != m_args.end())
            {
                return true;
            }
            return false;
        }
    }
}
