#include "CmdArgs.h"
#include <iostream>
namespace jimdb
{
    namespace common
    {
        CmdArgs CmdArgs::m_instance;

        void CmdArgs::init(int argc, char* argv[])
        {
            for (auto i = 1; i < argc; ++i)
            {
                m_args.push_back(argv[i]);
            }
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
            error += "needs a second parameter!";
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