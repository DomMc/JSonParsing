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

	std::string& First()
	{
		return m_pair.first;
	}

	[[nodiscard]] std::string& Second()
	{
		return m_pair.second;
	}

	[[nodiscard]] std::pair<std::string, std::string>& GetStdPair()
	{
		return m_pair;
	}

private:

	std::pair<std::string, std::string> m_pair;
};

class JsonObject
{
public:
	
	JsonObject(const std::string& name)
		: m_name(name)
	{}

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
		m_vPairs.emplace_back(JsonPair({ key, m_error_KeyNotFound }));
		return this->operator[](key);
	}

	[[nodiscard]] std::vector<JsonPair>& GetPairs()
	{
		return m_vPairs;
	}

	[[nodiscard]] std::vector<JsonPair>::size_type GetPairCount() const
	{
		return m_vPairs.size();
	}

	std::string& GetName()
	{
		return m_name;
	}

private:

	std::vector<JsonPair> m_vPairs;
	std::string m_name;
	std::string m_error_KeyNotFound = "Key not found in container!";
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

	[[nodiscard]] const std::string& operator()(const std::string& key)
	{
		for(auto& obj : m_vObjects)
		{
			for (auto& pair : obj.GetPairs())
			{
				if (pair.First() == key)
				{
					return pair.Second();
				}
			}
		}

		return m_error_KeyNotFOund;
	}

	[[nodiscard]] JSON_VECTOR& GetObjects()
	{
		return m_vObjects;
	}

	[[nodiscard]] JsonObject& GetObject(const std::string& name)
	{
		for(auto& obj : m_vObjects)
		{
			if(obj.GetName() == name)
			{
				return obj;
			}
		}

		// If the object isn't found by name, return the first object in the container.
		return m_vObjects[0];
	}

	[[nodiscard]] JsonObject& operator[](const int index)
	{
		return m_vObjects[index];
	}

	[[nodiscard]] JsonPair& GetJsonPair(const int objIndex, const int pairIndex)
	{
		return m_vObjects[objIndex].GetPairs()[pairIndex];
	}

	[[nodiscard]] std::pair<std::string, std::string> GetKeyValuePair(const int objIndex, const int pairIndex)
	{
		return m_vObjects[objIndex].GetPairs()[pairIndex].GetStdPair();
	}

	[[nodiscard]] std::vector<JsonObject>::size_type GetObjectCount() const
	{
		return m_vObjects.size();
	}

	[[nodiscard]] std::vector<JsonObject>::size_type GetPairCount() const
	{
		size_t count = 0;

		for(auto& obj : m_vObjects)
		{
			count += obj.GetPairCount();
		}

		return count;
	}

private:

	JSON_VECTOR m_vObjects;
	std::string m_string;
	std::string m_error_KeyNotFOund = "Key not found in container!";

	// Read string and create JSON objects from it.
	// String must follow standard JSON format.
	void ProcessString()
	{
		constexpr char OBJECT_START = '{';
		constexpr char OBJECT_END	= '}';
		constexpr char QUOTE_MARK	= '"';
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

				if(start == std::string::npos)
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

	void ObjectName(std::string& name, const size_t& start, const size_t& end) const
	{
		constexpr uint32_t MARKER_LENGTH = 1;
		name = m_string.substr(start + MARKER_LENGTH, end - (start + MARKER_LENGTH));
	}

	// Pass the marker positions rather than a sub-string.
	void ProcessObject(const size_t& start, const size_t& end, const std::string& name = { "Object 1" })
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

	int a = j.GetObjectCount();
	int b = j.GetPairCount();
	std::cout << "Object Count: " << a << "\t" << "Pair Count: " << b << std::endl << std::endl;

	std::pair<std::string, std::string> pair = j.GetKeyValuePair(0,2);
	std::cout << "Key: " << pair.first << "\t" << "Value: " << pair.second << std::endl;

	std::cout << j[0]["first"] << std::endl;
	std::cout << j[1]["new key"] << std::endl;

	std::cout << j("third") << std::endl;
	std::cout << j("new key 2") << std::endl;

	JsonObject obj = j.GetObject("Object 1");

	std::cout << std::endl << obj.GetName() << std::endl;
	for(auto& p : obj.GetPairs())
	{
		std::cout << p.First() << " : " << p.Second() << std::endl;
	}
	
	return 0;
}

