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
#include "log/logger.h"
#include <algorithm>
#include <cctype>

namespace jimdb
{
	namespace common
	{
		Configuration Configuration::m_instance;
		//definition of the mapping. ALWAYS map all enum values!
		const std::map<ConfigValues, Configuration::ConfigMapping> Configuration::m_mapping =
		{
			{LOG_LEVEL,{"logLevel", "5"}},
			{LOG_FILE,{"logFile", "defaultLogFile.txt"}},
			{THREADS,{"threads", "0"}},
		};


		Configuration& Configuration::getInstance()
		{
			return m_instance;
		}

		bool Configuration::parse(const std::string& file)
		{
			std::ifstream configFile(file, std::ios_base::in);
			if (!configFile || !configFile.is_open())
			{
				// do not use logger here since it depends on the config which is not parsed yet!
				//LOG_ERROR << "Couldn't open file:" << file;
				return false;
			}

			//check for utf-8 signatur
			char a, b, c;
			a = configFile.get();
			b = configFile.get();
			c = configFile.get();
			if (a != static_cast<char>(0xEF) || b != static_cast<char>(0xBB)
				|| c != static_cast<char>(0xBF))
			{
				configFile.seekg(0);
			}
			else
			{
				// do not use logger here since it depends on the config which is not parsed yet!
				//LOG_WARN << "file contains the 'UTF-8 signature' script this";
			}

			std::string line;
			while (std::getline(configFile, line))
			{
				//check if no = or # at the beginning so no key value pair or comment
				if (line.find("=") != std::string::npos && line.compare(0, 1, "#") != 0)
				{
					std::string delim = "=";
					auto key = line.substr(0, line.find(delim));
					auto value = line.substr(line.find(delim) + 1, line.length());

					//push back the value to the key
					m_values[key] = value;
				}
			}
			configFile.close();
			return true;
		}

		bool Configuration::contains(const std::string& key) const
		{
			if (m_values.count(key))
				return true;
			return false;
		}

		std::string Configuration::get(const ConfigValues& key)
		{
			if (contains(m_mapping.at(key).configKeyString))
			{
				return m_values[m_mapping.at(key).configKeyString];
			}
			//return default value if not exsist
			LOG_WARN << "The config does not contain: " << m_mapping.at(key).configKeyString;
			return m_mapping.at(key).defaultValue;
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

		std::ostream& operator<<(std::ostream& os, const Configuration& obj)
		{
			os << "Configuration:";
			for (auto& element : obj.m_values)
			{
				os << std::endl << element.first << "->" << element.second;
			}
			return os;
		}
	}
}
