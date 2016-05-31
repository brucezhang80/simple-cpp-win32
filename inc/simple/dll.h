#ifndef DLL_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define DLL_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	"_third/MemoryModule/MemoryModule.h"

enum DCSResult
{
	DCS_OK, 
	DCS_MissingFunctions,
	DCS_ErrorOpeningFile,
	DCS_ErrorReadingFile,
	DCS_InvalidDosHeader,
	DCS_InvalidNTHeader,
	DCS_NotDLL,
	DCS_DLLStructureError,
};
	
// Checks if the specified file is a real DLL that exports all 
// the specified functions.
DCSResult	dll_check_symbols(const char* dll_path, const char* symbols[], int symbol_count);

#endif
