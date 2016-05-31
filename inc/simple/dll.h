#ifndef DLL_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define DLL_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include	"_third/MemoryModule/MemoryModule.h"

enum DCSResult
{
	CFES_OK, 
	CFES_MissingFunctions,
	CFES_ErrorOpeningFile,
	CFES_ErrorReadingFile,
	CFES_InvalidDosHeader,
	CFES_InvalidNTHeader,
	CFES_NotDLL,
	CFES_DLLStructureError,
};
	
// Checks if the specified file is a real DLL that exports all 
// the specified functions.
DCSResult	dll_check_symbols(const char* dll_path, const char* symbols[], int symbol_count);

#endif
