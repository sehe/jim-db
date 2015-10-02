#pragma once
namespace jimdb {
	namespace common {
		template <typename T>
		inline LogMessage& LogMessage::operator<<(const T& m)
		{
			m_stream << m;
			return *this;
		}
	}
}