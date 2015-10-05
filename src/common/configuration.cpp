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

#include "configuration.h"
#include <fstream>
#include "../log/logger.h"
#include <algorithm>
#include <cctype>
#include "assert.h"
#include <iostream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/PrettyWriter.h>

namespace jimdb
{
    namespace common
    {
        Configuration Configuration::m_instance;

        const char* ConfigValuesMapper::EnumString[] =
        {
            "loglevel",
            "logfile",
            "thread",
            "port",
        };

        //check if valid numer must be the size of the enum!
        static_assert(sizeof(ConfigValuesMapper::EnumString) / sizeof(char*) == SIZE_OF_ENUM, "size dont match!");

        const char* ConfigValuesMapper::get(const ConfigValues& e)
        {
            return EnumString[e];
        }

        Configuration& Configuration::getInstance()
        {
            return m_instance;
        }

        bool Configuration::init(const std::string& filename)
        {
            //parse first
            std::ifstream configFile(filename);

            //if the file doenst exsist scip reading it
            if(configFile || configFile.is_open())
            {
                std::stringstream stream;
                stream << configFile.rdbuf();//get full string
                m_values.Parse(stream.str().c_str());//parse the file into json doc
                configFile.close();
            }
            else
            {
                throw std::runtime_error("missing config file");
            }

            if (m_values.HasParseError())
                throw std::runtime_error("config cannot be parsed - syntax error");

            //if it is no obj (empty file or no file or invalid json parsed create new obj)
            if (!m_values.IsObject())
                throw std::runtime_error("document is not an object");

            //check for default values or values that are not set
			check(LOG_FILE);
			check(LOG_LEVEL);
			check(THREADS); //0 means take system default
			check(PORT);

            return true;
        }

        std::string Configuration::get(const ConfigValues& key)
        {
            return m_values[ConfigValuesMapper::get(key)].GetString();
        }

        int Configuration::getInt(const ConfigValues& key)
        {
            return stoi(get(key));
        }

        bool Configuration::isNumber(const ConfigValues& key)
        {
            auto s = get(key);
            return !s.empty() && std::find_if(s.begin(), s.end(), [](char c)
            {
                return !isdigit(c);
            }) == s.end();
        }

        std::string Configuration::generate() const
        {
			rapidjson::Document doc;
			auto &alloc = doc.GetAllocator();
			doc.SetObject();
			
			rapidjson::Value name;
			name.SetString(ConfigValuesMapper::get(LOG_LEVEL), alloc);
			doc.AddMember(name, "5", doc.GetAllocator());

			name.SetString(ConfigValuesMapper::get(LOG_FILE), alloc);
			doc.AddMember(name, "default.log", doc.GetAllocator());

			name.SetString(ConfigValuesMapper::get(THREADS), alloc);
			doc.AddMember(name, "0", doc.GetAllocator());

			name.SetString(ConfigValuesMapper::get(PORT), alloc);
			doc.AddMember(name, "6060", doc.GetAllocator());

            rapidjson::StringBuffer strbuf;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
			doc.Accept(writer);
            return strbuf.GetString();
        }

        void Configuration::check(const ConfigValues& c)
        {
            if (m_values.FindMember(ConfigValuesMapper::get(c)) == m_values.MemberEnd())
            {
                std::string error = "Missing Config value: ";
                error += ConfigValuesMapper::get(c);
                throw new std::runtime_error(error);
            }
        }

        std::ostream& operator<<(std::ostream& os, const Configuration& obj)
        {
            rapidjson::StringBuffer strbuf;
            rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
            obj.m_values.Accept(writer);
            os << strbuf.GetString();
            return os;
        }
    }
}
