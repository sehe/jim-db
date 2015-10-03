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
#include <string>
#include <map>
namespace jimdb
{
    namespace common
    {
        /**\brief Config Enum
         *
         * Make sure to define all values at the Mapping!
         *@author Benjamin Meyer
         *@date Dienstag, 28. Juli 2015
         */
		enum ConfigValues
		{
			LOG_LEVEL = 0,
			LOG_FILE,
			THREADS,
			PORT,
        };

        /**
        * \brief The Configuration class
        *
        * \author Benjamin Meyer
        */
        class Configuration
        {
        public:
            static std::string m_logFileName;
            static Configuration& getInstance();

            /**
            \brief Parse config file into the config

            Parses a config file into the configuration object. Make sure it has all needed fields defined!
            \author Benjamin Meyer
            \date 09.07.2015 13:02
            */
            bool parse(const std::string& file);

            /**
            \brief check if it contains a value for a key
            \param[in] key the key to check
            \return true if the value to the key exsists
            \author Benjamin Meyer
            \date 09.07.2015 13:03
            */
            bool contains(const std::string& key) const;

            /**\brief Returns the value to the key
            *
            * If the key does not exist in the config, it return the default value
            \param[in] key
            \return the value as string
            *@author Benjamin Meyer
            *@date Dienstag, 28. Juli 2015
            */
            std::string get(const ConfigValues& key);

            /**
            \brief get a value to a key Take care it can fail if its not parseable!
            \param[in] key
            \return the value as Int
            \author Benjamin Meyer
            \date Donnerstag, 23. Juli 2015
            */
            int getInt(const ConfigValues& key);
            bool isNumber(const ConfigValues& key);

            friend std::ostream& operator<<(std::ostream& os, const Configuration& obj);

        private:
            Configuration() { };

            ~Configuration() { };

            //no copy no move no swap
            Configuration(const Configuration&) = delete;
            Configuration(Configuration&&) = delete;
            Configuration& operator=(const Configuration& other) = delete;
            Configuration& operator=(Configuration& other) = delete;

            struct ConfigMapping
            {
                std::string configKeyString;
                std::string defaultValue;

                ConfigMapping(const std::string& key, const std::string& def) :configKeyString(key), defaultValue(def) { };
            };

            //member
            static Configuration m_instance;
            std::map<std::string, std::string> m_values;
            const static std::map<ConfigValues, ConfigMapping> m_mapping;
        };
    }
}