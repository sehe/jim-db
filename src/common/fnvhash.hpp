inline size_t FNVHash::operator()(const std::string& string)
{
	return (*this)(string.c_str());
}

inline size_t FNVHash::operator()(const char* c)
{
	size_t hash = m_seed;
	while(*c)
	{
		hash ^= m_prime *static_cast<unsigned char>(*c++);
	}
	return hash;
}