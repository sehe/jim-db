#include "../assert.h"

namespace jimdb
{
    namespace common
    {
#ifndef USE_STD_HASH

        inline size_t Hash::operator()(const std::string& s, const size_t& hash) const
        {
            return operator()(s.c_str(), hash);
        }

        inline size_t Hash::operator()(const char* c, size_t hash) const
        {
            ASSERT(c);
            while (*c)
                hash ^= m_prime * static_cast<unsigned char>(*c++);
            return hash;
        }

#elif
        inline size_t Hash::operator()(const std::string& s, const size_t& hash) const
        {
            return std::hash<std::string>()(s);
        }

        inline size_t Hash::operator()(const char* c, size_t hash) const
        {
            return operator()(std::string(c), hash);
        }
#endif
    }
}
