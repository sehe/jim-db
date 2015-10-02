#include "logmessage.h"

template <typename T>
inline LogMessage& LogMessage::operator<<(const T& m)
{
	m_stream << m;
	return *this;
}
