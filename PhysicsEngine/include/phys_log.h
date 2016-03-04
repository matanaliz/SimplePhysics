#ifndef PHYS_LOGGER_H
#define PHYS_LOGGER_H
#include <phys_platform.h>

#include <fstream>
#include <iostream>
#include <string>

const std::string kDefaultPath = "test.log";

class PHYS_API filesink
{
private:
	std::ofstream m_stream;
	std::string m_path;

public:
	filesink(const std::string& path = "")
		: m_stream()
		, m_path(path)
	{
		if (0 == m_path.size())
			// TODO Generate file name with current date
			m_path = kDefaultPath;
	}

	~filesink() {};

	template <typename T>
	filesink& operator<<(const T& value)
	{
		m_stream.open(m_path.c_str(), std::fstream::out | std::fstream::app);
		if (m_stream.is_open())
			m_stream << value;
		m_stream.close();
		return *this;
	}

	filesink& operator<<(std::ostream& (*fp)(std::ostream&))
	{
		m_stream.open(m_path.c_str(), std::fstream::out | std::fstream::app);
		if (m_stream.is_open())
			fp(m_stream);
		m_stream.close();
		return *this;
	}

	template <typename T>
	friend filesink& operator<< (filesink& logger, const T& value);
};

template <typename T>
filesink& operator<< (filesink& sink, const T& value)
{
	sink.m_stream.open(sink.m_path.c_str(), std::fstream::out | std::fstream::app);
	if (sink.m_stream.is_open())
		sink.m_stream << value;
	sink.m_stream.close();
	return sink;
}

template <typename SINK>
class PHYS_API basic_logger
{
private:
	SINK& m_sink;

public:
	basic_logger(SINK& stream)
		: m_sink(stream)
	{
	}

	~basic_logger() {};

	template <typename T>
	basic_logger& operator<<(const T& value)
	{
		m_sink << value;
		return *this;
	}

	// To handle std::endl and other std stream manipulators
	basic_logger& operator<<(std::ostream& (*fp)(std::ostream&))
	{
		fp(m_sink);
		return *this;
	}

	template <typename T, class SINK>
	friend basic_logger<SINK>& operator<< (basic_logger<SINK>& logger, const T& value);
};

template <typename T, class SINK>
basic_logger<SINK>& operator<< (basic_logger<SINK>& logger, const T& value)
{
	logger.m_sink << value;
	return logger;
}

#endif // PHYS_LOGGER_H