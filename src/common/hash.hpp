#include "../assert.h"

namespace jimdb
{
    namespace common
    {
        inline size_t Hash::operator()(const std::string& s, size_t hash) const
        {
#ifndef USE_STD_HASH
            return memory(s.c_str(), s.length(), hash);
#elif
            return std::hash<std::string>()(s);
#endif
        }

        inline size_t Hash::operator()(const char* c, size_t hash) const
        {
#ifndef USE_STD_HASH
            ASSERT(c);
            while (*c)
                hash = byte(static_cast<unsigned char>(* c++), hash);
            return hash;
#elif
            return operator()(std::string(c), hash);
#endif
        }

        inline size_t Hash::byte(const char& c, size_t hash) const
        {
#ifndef USE_STD_HASH
            return hash ^ m_prime * c;
#elif
            return std::hash<char>()(c);
#endif
        }

        inline size_t Hash::memory(const void* data, size_t length, size_t hash) const
        {
#ifndef USE_STD_HASH
            ASSERT(data);
            auto ptr = static_cast<const unsigned char*>(data);
            while (length--)
                hash = byte(*ptr++, hash);
            return hash;
#elif
            //todo this is maybe unsafe.
            return 0;
#endif
        }
    }
}