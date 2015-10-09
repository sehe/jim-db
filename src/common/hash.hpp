#include "../assert.h"

namespace jimdb
{
    namespace common
    {
        inline size_t Hash::operator()(const std::string& s, size_t hash) const
        {
            return memory(s.c_str(), s.length(), hash);
        }

        inline size_t Hash::operator()(const char* c, size_t hash) const
        {
            ASSERT(c);
            while (*c)
                hash = byte(static_cast<unsigned char>(* c++), hash);
            return hash;
        }

        inline size_t Hash::byte(const char& c, size_t hash) const
        {
            return (c ^ hash) * m_prime;
        }

        inline size_t Hash::memory(const void* data, size_t length, size_t hash) const
        {
			ASSERT(data);
	        auto ptr = static_cast<const unsigned char*>(data);
            while (length--)     
				hash = byte(*ptr++, hash);
            return hash;
        }
    }
}