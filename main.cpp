
#include <iostream>
#include <string>
#include <utility>
#include <fstream>

int main()
{
	std::string filename = "C:/Users/dommc/Desktop/SaveFiles/Simi.sav";
	std::string inString;

	std::ifstream file(filename);
	if (file.is_open())
	{
		inString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	}
	file.close();

	constexpr char OBJECT_START = ':';
	constexpr char OBJECT_END = ' ';
	constexpr char OBJECT_END_ALT = '	';
	constexpr char LINE_END_MARKER = '#';
	constexpr char DATA_END_MARKER = '}';
	constexpr char FILE_END_MARKER = '{';

	constexpr std::string::size_type MARKER = 1;
	constexpr std::string::size_type SPACE = 1;

	// Position read markers
	std::string::size_type startRead	= inString.find(OBJECT_START) + MARKER + SPACE;
	std::string::size_type endRead		= inString.find(OBJECT_END, startRead);
	std::string::size_type lineEnd		= inString.find(LINE_END_MARKER, startRead);
	std::string::size_type dataEnd		= inString.find(DATA_END_MARKER, startRead);
	std::string::size_type fileEnd		= inString.find(FILE_END_MARKER, startRead);

	// Prepare to write data
	std::string outString = "";
	std::string delimiter = ",";

	// Example string
	// #Action: Highlight Target:BP_Target_C_2147482264	Seconds: 43	Partial: 0.685673	TargetX: 71.11575	TargetY: 36.595354	TargetZ: -68.963895	MouseX: 976.0	MouseY: 322.0	Key Required: S
	// #Action: Destroy Target:BP_Target_C_2147482264	Seconds:44	Partial: 0.310433	TargetX: 71.11575	TargetY: 36.595354	TargetZ: -68.963895	MouseX: 1009.0	MouseY: 307.0	Deviation Squared: 952745.5625	Key Required: S	Key Pressed: S

	while (dataEnd < fileEnd)
	{
		outString.append("NEW SESSION,");

		while (startRead < dataEnd)
		{
			uint32_t variableCount = 1;

			while (startRead < lineEnd)
			{
				// Write result into output string.
				outString.append(inString.substr(startRead, endRead - startRead));
				outString.append(delimiter);
				
				variableCount++;
				
				// Jump to the start of the next variable.
				startRead = inString.find(OBJECT_START, startRead);
				startRead += variableCount > 2 ? MARKER + SPACE : MARKER;

				// Reposition the end marker
				std::string::size_type end		= inString.find(OBJECT_END, startRead);
				std::string::size_type endAlt	= inString.find(OBJECT_END_ALT, startRead);
				
				// Pick the nearest end of variable delimiter. Both space and tab are valid! :-(
				endRead = end < endAlt ? end : endAlt;

				// Special case for the end of a line
				if (endRead > lineEnd)
				{
					endRead = startRead + SPACE;
				}
			}

			// Find the next line delimiter
			lineEnd = inString.find(LINE_END_MARKER, startRead);
			outString.append("\n");

			std::cout << outString;
			outString.clear();
		}

		// Find the next data delimiter
		dataEnd = inString.find(DATA_END_MARKER, startRead);

		// Find the next line delimiter
		lineEnd = inString.find(LINE_END_MARKER, startRead);

		// Find the next variable
		startRead = inString.find(OBJECT_START, lineEnd);
		startRead += MARKER;

		// Reposition the end of entry marker
		endRead = inString.find(OBJECT_END, startRead);
	}
	

	return 0;
}

