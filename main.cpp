/*
	Parse a JSON file into a string and then process the string to retrieve the information
	within the file.
*/
#include <iostream>

#include "JsonContainer.h"
#include "JsonObject.h"
#include "JsonPair.h"

typedef std::vector<JsonObject>::iterator::value_type ObjectIteratorValueType;
typedef std::vector<JsonPair>::iterator::value_type PairIteratorValueType;

// Test harness.
int main()
{
	// Construct from JSON file.
	JsonContainer j("JSON/array.json");

	// Print all objects and their pairs.
	for(ObjectIteratorValueType& obj : j.GetObjects())
	{
		for(PairIteratorValueType& a : obj.GetPairs())
		{
			std::cout << a.First() << " : " << a.Second() << std::endl;
		}

		std::cout << std::endl;
	}
	std::cout << std::endl;

	// Print the total object and pair counts.
	int a = j.GetObjectCount();
	int b = j.GetPairCount();
	std::cout << "Object Count: " << a << "\t" << "Pair Count: " << b << std::endl;
	std::cout << std::endl;

	// Print pair count per object.
	for (auto& obj : j.GetObjects())
	{
		std::cout << obj.GetPairCount() << std::endl;
	}
	std::cout << std::endl;

	// Retrieve and print a key : value pair.
	std::pair<std::string, std::string> pair = j.GetKeyValuePair(0,2);
	std::cout << "Key: " << pair.first << "\t" << "Value: " << pair.second << std::endl;
	std::cout << std::endl;

	// Retrieve and print a value using object index and key name. Print an error message if not found.
	std::cout << j[0]["first"] << std::endl;
	std::cout << j[1]["new key"] << std::endl;
	std::cout << std::endl;

	// Retrieve and print a value using key name. Print an error message if not found.
	std::cout << j("third") << std::endl;
	std::cout << j("new key 2") << std::endl;
	std::cout << std::endl;

	// Retrieve and print a value using object name and key name.
	JsonObject obj = j.GetObject("Object 1");
	std::cout << j["Object 1"]["first"] << std::endl;
	std::cout << j["Object 2"]["seventh"] << std::endl;
	std::cout << std::endl;

	// Rerieve an object's name, then print it and all of its key : value pairs.
	std::cout << obj.GetName() << std::endl;
	for(auto& p : obj.GetPairs())
	{
		std::cout << p.First() << " : " << p.Second() << std::endl;
	}
	
	return 0;
}

