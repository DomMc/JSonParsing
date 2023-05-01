#pragma once

#include <string>
#include <utility>
#include <vector>

class JsonPair;

// An object made up of related JSON pairs.
class JsonObject
{
public:

	JsonObject(const std::string& name);

	// Add key : value pair to container.
	void operator[](JsonPair& pair);
	
	// Look up a value based on its key.
	[[nodiscard]] std::string& operator[](const std::string& key);

	[[nodiscard]] std::vector<JsonPair>& GetPairs() { return m_vPairs; }
	[[nodiscard]] std::vector<JsonPair>::size_type GetPairCount() const { return m_vPairs.size(); }
	[[nodiscard]] std::string& GetName() { return m_name; }

private:

	std::vector<JsonPair> m_vPairs;
	std::string m_name;
	std::string m_error_KeyNotFound = "Key not found in container!";
};

