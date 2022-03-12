/*
	Parse a JSON file into a string and then process the string to retrieve the information
	within the file.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

#define ISTREAM_IT				std::istreambuf_iterator
#define JSON_OBJECT				std::unordered_map<std::string, std::string>
#define JSON_VECTOR				std::vector<JSON_OBJECT>

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

	[[nodiscard]] const JSON_VECTOR& GetObjects() const
	{
		return m_vObjects;
	}

	[[nodiscard]] const JSON_OBJECT& operator[](const int index) const
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
		size_t temp = start;
		start = m_string.find(OBJECT_START, start + MARKER_LENGTH);

		// There may only be one JSON object in the file.
		if (start == std::string::npos)
		{
			start = temp;
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

		JSON_OBJECT object;

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
			object.insert({ key, value });

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
	uint32_t i = 1;

	for (const auto& obj : j.GetObjects())
	{
		std::cout << "Object " << i << std::endl;
		++i;

		for (const auto& x : obj)
		{
			std::cout << x.first << " : " << x.second << std::endl;
		}

		std::cout << std::endl;
	}

	const JSON_OBJECT OBJ = j[0];

	for(const auto& [fir, sec] : OBJ)
	{
		std::cout << fir << " : " << sec << std::endl;
	}

	return 0;
}

