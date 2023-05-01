#pragma once

#include <string>
#include <utility>
#include <vector>

class JsonObject;
class JsonPair;

// A container for JSON objects.
class JsonContainer
{
public:

	// Read JSON file into a string then process into JSON objects.
	JsonContainer(const std::string& filename);

	[[nodiscard]] const std::string& operator()(const std::string& key);
	[[nodiscard]] JsonObject& operator[](const int index) { return m_vObjects[index]; }
	[[nodiscard]] JsonObject& operator[](const std::string& name) { return this->GetObject(name); }
	
	[[nodiscard]] const std::string& GetString() const { return m_string; }
	[[nodiscard]] std::vector<JsonObject>& GetObjects() { return m_vObjects; }
	[[nodiscard]] JsonObject& GetObject(const std::string& name);

	[[nodiscard]] JsonPair& GetJsonPair(const int objIndex, const int pairIndex);
	[[nodiscard]] std::pair<std::string, std::string> GetKeyValuePair(const int objIndex, const int pairIndex);

	[[nodiscard]] std::vector<JsonObject>::size_type GetObjectCount() const { return m_vObjects.size(); }
	[[nodiscard]] std::vector<JsonObject>::size_type GetPairCount() const;

private:

	std::vector<JsonObject> m_vObjects;
	std::string m_string;
	std::string m_error_KeyNotFOund = "Key not found in container!";

	// Read string and create JSON objects from it.
	// String must follow standard JSON format.
	void ProcessString();
	void ObjectName(std::string& name, const size_t& start, const size_t& end) const;

	// Pass the marker positions rather than a sub-string.
	void ProcessObject(const size_t& start, const size_t& end, const std::string& name = { "Object 1" });
};

