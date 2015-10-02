#include "logmessage.h"
#include "logtimer.h"

template <typename T>
LogTimer& LogTimer::operator<<(const T& m)
{
	LogMessage::operator<<(m);
	return *this;
}


inline LogTimer::LogTimer(const LogTimer& other) : LogMessage(other), m_start(other.m_start) { }

inline LogTimer::LogTimer(LogTimer&& other): LogMessage(std::move(other)), m_start(std::move(other.m_start)) { } 

inline LogTimer& LogTimer::operator=(const LogTimer& other) 
{
	if (this == &other)
		return *this;
	LogMessage::operator =(other);
	m_start = other.m_start;
	return *this;
}


inline LogTimer& LogTimer::operator=(LogTimer&& other)
{
	if (this == &other)
		return *this;
	LogMessage::operator =(std::move(other));
	m_start = std::move(other.m_start);
	return *this;
}
