/*
	Parse a JSON file into a string and then process the string to retrieve the information
	within the file.
*/

#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

#define ISTREAM_IT				std::istreambuf_iterator
#define JSON_VECTOR				std::vector<JsonObject>

class JsonPair
{
public:

	JsonPair() = default;

	JsonPair(std::pair<std::string, std::string> pair)
		: m_pair(std::move(pair))
	{}

	[[nodiscard]] std::string& First()
	{
		return m_pair.first;
	}

	[[nodiscard]] std::string& Second()
	{
		return m_pair.second;
	}

private:

	std::pair<std::string, std::string> m_pair;
};

class JsonObject
{
public:

	// Add key : value pair to container.
	void operator[](JsonPair& pair)
	{
		m_vPairs.emplace_back(pair);
	}

	// Look up a value based on its key.
	[[nodiscard]] std::string& operator[](const std::string& key)
	{
		for (JsonPair& pair : m_vPairs)
		{
			if (pair.First() == key)
			{
				return pair.Second();
			}
		}

		// If the key isn't in the vector, mimic std::map by adding it, then run again.
		m_vPairs.emplace_back(JsonPair({ key, "key not in container" }));
		return this->operator[](key);
	}

	[[nodiscard]] std::vector<JsonPair>& GetPairs()
	{
		return m_vPairs;
	}

private:

	std::vector<JsonPair> m_vPairs;
};

class JSonContainer
{
public:

	// Read JSON file into a string then process into JSON objects.
	JSonContainer(const std::string& filename)
	{
		std::ifstream file(filename);
		if(file.is_open())
		{
			m_string = std::string(ISTREAM_IT<char>(file), ISTREAM_IT<char>());
		}
		file.close();

		ProcessString();
	}

	[[nodiscard]] const std::string& GetString() const
	{
		return m_string;
	}

	[[nodiscard]] JSON_VECTOR& GetObjects()
	{
		return m_vObjects;
	}

	[[nodiscard]] JsonObject& operator[](const int index)
	{
		return m_vObjects[index];
	}

private:

	std::string m_string;
	JSON_VECTOR m_vObjects;

	// Read string and create JSON objects from it.
	// String must follow standard JSON format.
	void ProcessString()
	{
		constexpr char OBJECT_START = '{';
		constexpr char OBJECT_END	= '}';
		constexpr uint32_t MARKER_LENGTH = 1;

		size_t start = m_string.find(OBJECT_START);
		size_t end = m_string.find(OBJECT_END);

		// A valid JSON file should contain at least one '{' and '}', in that order.
		if (end == std::string::npos || start >= end)
		{
			std::cout << "Not a valid JSON file!" << std::endl;
		}

		// Move start to the first object within the JSON file.
		// end will already be in the right place.
		const size_t TEMP = start;
		start = m_string.find(OBJECT_START, start + MARKER_LENGTH);

		// There may only be one JSON object in the file.
		if (start == std::string::npos)
		{
			start = TEMP;
			ProcessObject(start, end);
		}
		else // Handle multiple objects.
		{
			while (start != std::string::npos)
			{
				ProcessObject(start, end);

				// Leapfrog markers to the next object.
				start = m_string.find(OBJECT_START, end + MARKER_LENGTH);
				end = m_string.find(OBJECT_END, start + MARKER_LENGTH);
			}
		}
	}

	// Pass the marker positions rather than a sub-string.
	void ProcessObject(const size_t& start, const size_t& end)
	{
		constexpr char PAIR_START = '"';
		constexpr int MARKER_LENGTH = 1;

		JsonObject object;

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
};

int main()
{
	JSonContainer j("JSON/objects.json");

	for(auto& obj : j.GetObjects())
	{
		for(auto& a : obj.GetPairs())
		{
			std::cout << a.First() << " : " << a.Second() << std::endl;
		}

		std::cout << std::endl;
	}

	std::cout << j[0]["third"] << std::endl;
	std::cout << j[1]["new key"] << std::endl;
	
	return 0;
}

