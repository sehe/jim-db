// /**
// ############################################################################
// # GPL License                                                              #
// #                                                                          #
// # This file is part of the JIM-DB.                                         #
// # Copyright (c) 2015, Benjamin Meyer, <benjamin.meyer@tu-clausthal.de>     #
// # This program is free software: you can redistribute it and/or modify     #
// # it under the terms of the GNU General Public License as                  #
// # published by the Free Software Foundation, either version 3 of the       #
// # License, or (at your option) any later version.                          #
// #                                                                          #
// # This program is distributed in the hope that it will be useful,          #
// # but WITHOUT ANY WARRANTY; without even the implied warranty of           #
// # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
// # GNU General Public License for more details.                             #
// #                                                                          #
// # You should have received a copy of the GNU General Public License        #
// # along with this program. If not, see <http://www.gnu.org/licenses/>.     #
// ############################################################################
// **/
#include <memory>

inline ArrayItemString::ArrayItemString(const std::string& s) : ArrayItem<long long>(0, STRING)
{
    m_data.size = s.size();
    m_next = 0;
    //now memcpy the data into memory
    memcpy(this + 1, s.c_str(), s.size()); //cpy the string
}

inline std::shared_ptr<std::string> ArrayItemString::getString() const
{
    auto string = std::make_shared<std::string>();
    //get the char array
    string->append(reinterpret_cast<const char*>(this + 1), m_data.size);//appand it
    return string;//return the shared_ptr as copy
}
