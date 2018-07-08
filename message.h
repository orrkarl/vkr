#ifndef CORE_MESSAGING__MESSAGE_H_
#define CORE_MESSAGING__MESSAGE_H_

#include <map>
#include <utility>
#include "field.h"
#include <cstdarg>
#include "error_handling.h"

enum MessageType
{
	
};

class Message
{
private:
	std::map<std::string, AnyField> m_content_;

protected:
	typedef std::pair<std::string, AnyField> Pair;

public:
	virtual ~Message() = default;


	Message(const Message& other) = default;

	Message(Message&& other) noexcept
		: m_content_(std::move(other.m_content_))
	{
	}

	Message& operator=(const Message& other)
	{
		if (this == &other)
			return *this;
		m_content_ = other.m_content_;
		return *this;
	}

	Message& operator=(Message&& other) noexcept
	{
		if (this == &other)
			return *this;
		m_content_ = std::move(other.m_content_);
		return *this;
	}

	explicit Message(std::map<std::string, AnyField> content) : m_content_(std::move(content)) {}

	explicit Message(const int count, Pair content...)
	{
		va_list va;

		va_start(va, content);
		for (auto i = 0; i < count; ++i)
		{
			const auto pair = va_arg(va, Pair);
			m_content_[pair.first] = pair.second;
		}
		va_end(va);
	}

	explicit Message(const int count, va_list content)
	{
		for (auto i = 0; i < count; ++i)
		{
			const auto pair = va_arg(content, Pair);
			m_content_[pair.first] = pair.second;
		}
	}

	AnyField operator[](const std::string& key) const
	{
		ASSERT(HasKey(key));

		return m_content_.at(key);
	}

	bool HasKey(const std::string& key) const
	{
		return m_content_.count(key) != 0;
	}

	virtual MessageType GetType() = 0;
};

#endif // CORE_MESSAGING__MESSAGE_H_
