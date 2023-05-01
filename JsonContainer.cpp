#include <fstream>
#include <iostream>

#include "JsonContainer.h"
#include "JsonObject.h"
#include "JsonPair.h"

typedef std::vector<JsonObject>::iterator::value_type ObjectIteratorValueType;
typedef std::vector<JsonPair>::iterator::value_type PairIteratorValueType;

// Read JSON file into a string then process into JSON objects.
JsonContainer::JsonContainer(const std::string& filename)
{
	std::ifstream file(filename);
	if (file.is_open())
	{
		m_string = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	}
	file.close();

	ProcessString();
}

[[nodiscard]] const std::string& JsonContainer::operator()(const std::string& key)
{
	for (ObjectIteratorValueType& obj : m_vObjects)
	{
		for (PairIteratorValueType& pair : obj.GetPairs())
		{
			if (pair.First() == key)
			{
				return pair.Second();
			}
		}
	}

	return m_error_KeyNotFOund;
}

[[nodiscard]] JsonObject& JsonContainer::GetObject(const std::string& name)
{
	for (auto& obj : m_vObjects)
	{
		if (obj.GetName() == name)
		{
			return obj;
		}
	}

	// If the object isn't found by name, return the first object in the container.
	return m_vObjects[0];
}

[[nodiscard]] JsonPair& JsonContainer::GetJsonPair(const int objIndex, const int pairIndex)
{
	return m_vObjects[objIndex].GetPairs()[pairIndex];
}

[[nodiscard]] std::pair<std::string, std::string> JsonContainer::GetKeyValuePair(const int objIndex, const int pairIndex)
{
	return m_vObjects[objIndex].GetPairs()[pairIndex].GetAsStdPair();
}

[[nodiscard]] std::vector<JsonObject>::size_type JsonContainer::GetPairCount() const
{
	size_t count = 0;

	for (auto& obj : m_vObjects)
	{
		count += obj.GetPairCount();
	}

	return count;
}

void JsonContainer::ProcessString()
{
	constexpr char OBJECT_START = '{';
	constexpr char OBJECT_END = '}';
	constexpr char QUOTE_MARK = '"';
	constexpr uint32_t MARKER_LENGTH = 1;

	size_t start = m_string.find(OBJECT_START);
	size_t end = m_string.find(OBJECT_END);

	// A valid JSON file should contain at least one '{' and '}', in that order.
	if (end == std::string::npos || start >= end)
	{
		std::cout << "Not a valid JSON file!" << std::endl;
	}

	// Search for an object inside the file.
	const size_t TEMP = start;
	start = m_string.find(OBJECT_START, start + MARKER_LENGTH);

	// There may only be one JSON object in the file.
	if (start == std::string::npos)
	{
		// Move start back to start of file.
		start = TEMP;
		ProcessObject(start, end);
	}
	else // Handle multiple objects.
	{
		// Move markers back to start of string.
		start = TEMP;
		end = start;

		while (start != std::string::npos)
		{
			start = m_string.find(QUOTE_MARK, end + MARKER_LENGTH);
			end = m_string.find(QUOTE_MARK, start + MARKER_LENGTH);

			if (start == std::string::npos)
			{
				break;
			}

			// Read object name.
			std::string objectName;
			ObjectName(objectName, start, end);

			// Leapfrog markers to the next object.
			start = m_string.find(OBJECT_START, end + MARKER_LENGTH);
			end = m_string.find(OBJECT_END, start + MARKER_LENGTH);

			// Fill the object wit pairs, then add to container.
			ProcessObject(start, end, objectName);
		}
	}
}

void JsonContainer::ObjectName(std::string& name, const size_t& start, const size_t& end) const
{
	constexpr uint32_t MARKER_LENGTH = 1;
	name = m_string.substr(start + MARKER_LENGTH, end - (start + MARKER_LENGTH));
}

// Pass the marker positions rather than a sub-string.
void JsonContainer::ProcessObject(const size_t& start, const size_t& end, const std::string& name/* = { "Object 1" }*/)
{
	constexpr char PAIR_START = '"';
	constexpr int MARKER_LENGTH = 1;

	JsonObject object(name);

	// Add JSON pairs to the object until startRead passes the end of this object.
	std::string::size_type startRead = m_string.find(PAIR_START, start + MARKER_LENGTH);
	while (startRead < end)
	{
		// Move end marker to the end of first sub-string.
		std::string::size_type endRead = m_string.find(PAIR_START, startRead + MARKER_LENGTH);
		std::string key = m_string.substr(startRead + MARKER_LENGTH, endRead - (startRead + MARKER_LENGTH));

		// Leapfrog markers to next entry.
		startRead = m_string.find(PAIR_START, endRead + MARKER_LENGTH);
		endRead = m_string.find(PAIR_START, startRead + MARKER_LENGTH);
		std::string value = m_string.substr(startRead + MARKER_LENGTH, endRead - (startRead + MARKER_LENGTH));

		// Add JSON pair to the object.
		JsonPair p({ key, value });
		object[p];

		// Move start marker to next position here to aid end of file validation by loop.
		startRead = m_string.find(PAIR_START, endRead + MARKER_LENGTH);
	}

	// Add JSON object to container.
	m_vObjects.emplace_back(object);
}

