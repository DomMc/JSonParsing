#include "JsonObject.h"
#include "JsonPair.h"

JsonObject::JsonObject(const std::string& name)
	: m_name(name)
{

}

// Add key : value pair to container.
void JsonObject::operator[](JsonPair& pair)
{
	m_vPairs.emplace_back(pair);
}

// Look up a value based on its key.
std::string& JsonObject::operator[](const std::string& key)
{
	for (JsonPair& pair : m_vPairs)
	{
		if (pair.First() == key)
		{
			return pair.Second();
		}
	}

	// If the key isn't in the vector, mimic std::map by adding it, then run again.
	m_vPairs.emplace_back(JsonPair({ key, m_error_KeyNotFound }));
	return this->operator[](key);
}

