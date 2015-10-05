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
#include <rapidjson/document.h>

namespace jimdb
{
    namespace common
    {
        /**
        All values that can be obtained. 

		SIZE_OF_ENUM is not obtainable!
        */
        enum ConfigValues
        {
            LOG_LEVEL = 0,
            LOG_FILE,
            THREADS,
            PORT,
			SIZE_OF_ENUM,
        };

        /**
        small mapping function struct
        */
        struct ConfigValuesMapper
        {
            //need the strings for the size
            static const char* EnumString[];

            /**
            @return the configValue as const char*
            */
            static const char* get(const ConfigValues& e);
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

            Parses a config file into the configuration object.
            if it has missing filds this function will add the
            default values to the file and to the config.

            If the file is missing it writes the default config to the file

            \author Benjamin Meyer
            \date 09.07.2015 13:02
            */
            bool init(const std::string& filename);

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

            /** checks if a value exsists. If not, it set the value

            @author Benjamin Meyer
            @date 05.10.2015 10:32
            */
            void setDefault(const ConfigValues& c, const std::string& value);

            rapidjson::Document m_values;

            //member
            static Configuration m_instance;
            //std::map<std::string, std::string> m_values;
        };
    }
}