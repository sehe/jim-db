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
#pragma once
#include <string>

namespace jimdb {
	namespace common {
		class FNVHash
		{
		public:
			inline size_t operator()(const std::string& string);
			inline size_t operator()(const char* c);
		private:
			/* magic 64bit numbers from http://www.isthe.com/chongo/tech/comp/fnv/ */
			static const size_t m_seed = 0xcbf29ce484222325ULL; // 14695981039346656037
			static const size_t m_prime = 0x100000001b3ULL;// 1099511628211
		};
#include "fnvhash.hpp"
	}
}