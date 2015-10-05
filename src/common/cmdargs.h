#pragma once
#include <map>
#include <vector>

namespace jimdb
{
    namespace common
    {
        /**
        \brief Class to parse and obtain cmd arguments

        @author Benjamin Meyer
        @date 05.10.2015 12:58
        */
        class CmdArgs
        {
        public:

			static CmdArgs& getInstance();
            void init(int argc, char* argv[]);
            ~CmdArgs();

            std::string operator[](const std::string& v);

            /**
            @return true if args contain a value for example --h
            @author Benjamin Meyer
            @date 05.10.2015 12:56
            */
            bool contains(const std::string& v);
        private:
            std::vector<std::string> m_args;
			static CmdArgs m_instance;
        };
    }
}
