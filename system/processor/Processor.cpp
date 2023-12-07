#include "../../ginc.h"
#include "Processor.h"
#include <intrin.h>

CProcessorInfo::CProcessorInfo()
{
	Initialize();
}

CProcessorInfo::~CProcessorInfo()
{

}

void CProcessorInfo::Initialize()
{
	// Reference StackOverflow
	// 4 is essentially hardcoded due to the __cpuid function requirements.
	// NOTE: Results are limited to whatever the sizeof(int) * 4 is...

	std::array<int, 4> integerBuffer = {};
	constexpr size_t sizeofIntegerBuffer = sizeof(int) * integerBuffer.size();
	std::array<char, 64> charBuffer = {};

	// The information you wanna query __cpuid for.
	// https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=vs-2019
	constexpr std::array<unsigned int, 3> functionIds = {
		// Manufacturer
		//  EX: "Intel(R) Core(TM"
		0x80000002,
		// Model
		//  EX: ") i7-8700K CPU @"
		0x80000003,
		// Clockspeed
		//  EX: " 3.70GHz"
		0x80000004
	};

	_strProcessorInfo = wxT("");
	for (int id : functionIds)
	{
		// Get the data for the current ID.
		__cpuid(integerBuffer.data(), id);

		// Copy the raw data from the integer buffer into the character buffer
		std::memcpy(charBuffer.data(), integerBuffer.data(), sizeofIntegerBuffer);

		// Copy that data into a std::string
		_strProcessorInfo += wxString::Format(wxT("%s"), charBuffer.data()).Trim(true);
	}
}
