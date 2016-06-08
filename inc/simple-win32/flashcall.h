#ifndef FLASHCALL_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define FLASHCALL_H_66BC65DB_AFF6_43C8_8654_D1A2801635E2

#include <string>
#include <iosfwd>

class	DynamicValue;
class	DynamicData;

void	FlashCallXML_ClearArguments	(std::deque<DynamicData*>& arguments);

bool	FlashCallXML_SerializeValue	(std::ostream& os, const DynamicValue& value);
bool	FlashCallXML_SerializeData	(std::ostream& os, DynamicData& data);
bool	FlashCallXML_SerializeInvoke(std::ostream& os, const std::string& func_name, const std::deque<DynamicData*>& args);
bool	FlashCallXML_SerializeInvoke(std::ostream& os, const std::string& func_name, DynamicData* args, size_t args_sum);

bool	FlashCallXML_DeSerializeData	(const char* pszXML, DynamicData& data);
bool	FlashCallXML_DeSerializeInvoke	(const char* pszXML, std::string& func_name, std::deque<DynamicData*>& arguments);

#endif
