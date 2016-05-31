#ifndef __CHECK_FOR_DLL_EXPORTED_SYMBOLS_H__
#define __CHECK_FOR_DLL_EXPORTED_SYMBOLS_H__
#pragma once

enum ECFESResult
{
    eCFES_OK,
    eCFES_MissingFunctions,
    eCFES_ErrorOpeningFile,
    eCFES_ErrorReadingFile,
    eCFES_InvalidDosHeader,
    eCFES_InvalidNTHeader,
    eCFES_NotDLL,
    eCFES_DLLStructureError,
};
 
// Checks if the specified file is a real DLL that exports all 
// the specified functions.
ECFESResult CheckForExportedSymbols(LPCTSTR dll_path, 
    const char* symbols[], int symbol_count);
 
#endif
