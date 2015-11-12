﻿// /**
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
#pragma once
#include "task.h"
#include <vector>

namespace jimdb
{
    namespace tasking
    {
        class InsertTask : public Task
        {
        public:
            explicit InsertTask(const std::shared_ptr<network::IClient>& client, const std::shared_ptr<network::Message> m);
            void operator()() override;
        //private:
            std::shared_ptr<network::Message> m_msg;

            /**
            \brief insert a meta and returns the size WITH page overhead

            only insert if the meta does not exsist
            @author Benjamin Meyer
            @date 28.10.2015 15:40
            */
            static size_t checkSizeAndMeta(const std::string& name, const rapidjson::GenericValue<rapidjson::UTF8<>>& val);

            /**
            \brief calculates the size of the array with overhead

            Also include inner object with a id
            @author Benjamin Meyer
            @date 28.10.2015 16:29
            */
            static size_t checkSizeArray(const rapidjson::GenericValue<rapidjson::UTF8<>>& val);

            //vector of inner object ids which get insterted
            // while creation of the meta
            std::vector<size_t> m_innerIDs;
        };
    }
}
