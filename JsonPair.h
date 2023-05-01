#pragma once

#include <string>
#include <utility>

// A simple pair of JSON strings.
class JsonPair
{
public:

	JsonPair() = default;
	JsonPair(std::pair<std::string, std::string> pair)
		: m_pair(std::move(pair))
	{}

	[[nodiscard]] std::string& First() { return m_pair.first; }
	[[nodiscard]] std::string& Second() { return m_pair.second; }

	[[nodiscard]] std::pair<std::string, std::string>& GetAsStdPair() { return m_pair; }

private:

	std::pair<std::string, std::string> m_pair;
};

