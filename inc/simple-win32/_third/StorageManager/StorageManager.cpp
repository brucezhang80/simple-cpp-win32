/**
@file	StorageManager.cpp

@brief	Implements the StorageManagers class non-template functions.
 */

#include "StorageManager.h"
#include <Windows.h>
#include <Shlobj.h>
#include <fstream>
#include <sstream>
#pragma comment(lib, "Shell32.lib")
using namespace std;
#ifdef STORMAN_CRYPTED
#include "files.h"
#include "hex.h"
#include "filters.h"
#include "aes.h"
#include "eax.h"
#include "osrng.h"
#pragma comment(lib, "cryptlib.lib")
using namespace CryptoPP;
#endif

map<string, StorageManager::pointerManagerBase*> StorageManager::pointers;
map<string, StorageManager::rawPointer> StorageManager::loadData;
map<unsigned long, StorageManager::StorVarCounter> StorageManager::storVars;
unsigned long StorageManager::lastID = 0;
bool StorageManager::nothrow(false);
string StorageManager::path;
string StorageManager::name;
string StorageManager::loadProtokol;
#ifdef STORMAN_PERSONAL_POLICY
StorageManager::LoadPolicy StorageManager::loadPolicy((StorageManager::LoadPolicy)STORMAN_PERSONAL_POLICY);
#else
StorageManager::LoadPolicy StorageManager::loadPolicy(StorageManager::REPLACE);
#endif
bool StorageManager::autoSave(true);
bool StorageManager::verifyName(true);
bool StorageManager::crypted(false);
vector<void*> StorageManager::corruption;
#ifdef STORMAN_CRYPTED
SecByteBlock StorageManager::key;
SecByteBlock StorageManager::iv(AES::BLOCKSIZE);
#endif

StorageManager StorageManager::instance;

/**
@class	xmlAttribute

@internal
*/
struct xmlAttribute
{
	string name;
	string value;
};

#ifdef STORMAN_CRYPTED
#ifndef PATCH_SecByteBlockSink
#undef max
/**
@class	SecByteBlockSink

@internal
*/
class SecByteBlockSink : public Bufferless<Sink>
{
public:
	SecByteBlockSink(SecByteBlock& sbb) : m_sbb(sbb) {}

	size_t Put2(const byte *inString, size_t length, int /*messageEnd*/, bool /*blocking*/)
	{
		if (!inString || !length) return length;

		const size_t size = m_sbb.size();
		const size_t max = std::numeric_limits<std::size_t>::max() - size;

		if (length > max)
			throw InvalidArgument("SecByteBlockSink: buffer overflow");

		m_sbb.resize(size + length);
		memcpy(m_sbb.begin() + size, inString, length);

		return 0;
	}

private:
	SecByteBlock& m_sbb;
};

#endif
#endif

static vector<xmlAttribute> getAttribs(string text, string &tagName);
static string escapeToXml(const string &ref);
static string escapeFromXml(const string &ref);

StorageManager::StorageManager()
{
#ifdef STORMAN_PERSONAL_NAME
    StorageManager::name = STORMAN_PERSONAL_NAME;
#else
	char buffer[1024];
	DWORD ret = GetModuleFileNameA(NULL, buffer, 1024);
	if (ret < 1024)
	{
        StorageManager::name = string(buffer, ret);
        StorageManager::name.resize(StorageManager::name.rfind('.'));
        StorageManager::name = StorageManager::name.substr(StorageManager::name.rfind('\\') + 1);
	}
#endif

#ifdef STORMAN_PERSONAL_PATH
    StorageManager::path = STORMAN_PERSONAL_PATH;
#else
    char appd[MAX_PATH];
    if(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, appd) == S_OK)
    {
        StorageManager::path = appd + string("\\") + StorageManager::name + string("\\");
        CreateDirectoryA(path.c_str(), NULL);
#ifdef STORMAN_CRYPTED
		StorageManager::path += string("Data.cxml");
#else
        StorageManager::path += string("Data.xml");
#endif
    }
#endif

#ifdef STORMAN_CRYPTED
	try
	{
		StorageManager::crypted = true;
		StorageManager::iv.Assign((byte*)STORMAN_CRYPTED, AES::BLOCKSIZE);
		ArraySource((byte*)STORMAN_CRYPTED, sizeof(STORMAN_CRYPTED), true, new HexDecoder(new SecByteBlockSink(StorageManager::key)));
	}
	catch (Exception e)
	{
		OutputDebugStringA(e.what());
		StorageManager::key = SecByteBlock();
	}
#endif

#ifndef STORMAN_NOLOAD
	StorageManager::load();
#endif
}

/**
@brief	Destructor.

@author	Felix
@date	28.09.2014
 */

StorageManager::~StorageManager()
{
	if (StorageManager::autoSave)
		StorageManager::save();

	for (map<string, pointerManagerBase*>::iterator it = StorageManager::pointers.begin(); it != StorageManager::pointers.end(); it++)
	{
		it->second->close();
		delete it->second;
		it->second = NULL;
	}
	for (map<string, rawPointer>::iterator it = StorageManager::loadData.begin(); it != StorageManager::loadData.end(); it++)
	{
		delete[] it->second.rawData;
		it->second.rawData = NULL;
	}
}

/**
@public

@details Use this function to free a pointer declared on Storage. If the pointer is in Storage, it will be deleted, if not,
		 the function does nothing

@param [in]	storagePointer	The pointer to be freed

@exception nothrow This function will never throw an exception
 */
 
void StorageManager::freeStorage(void *storagePointer)
{
	for (map<string, pointerManagerBase*>::iterator it = StorageManager::pointers.begin(); it != StorageManager::pointers.end(); it++)
	{
		if (it->second->getPtr() == storagePointer)
		{
			delete it->second;
			it->second = NULL;
			StorageManager::pointers.erase(it);
			break;
		}
	}
}

/**
@public

@details Scans the Storage looking for the Variable with the given name, returns true, if the Manager has a variable with this name

@param	[in] name	The Name of the Variable tested.

@exception nothrow This function will never throw an exception

@return	true if the object is in the Storage, false if not.
 */

bool StorageManager::contains(std::string name)
{
	std::map<std::string, pointerManagerBase*>::iterator it1 = StorageManager::pointers.find(name);
	if (it1 != StorageManager::pointers.end())
		return true;

	std::map<std::string, rawPointer>::iterator it2 = StorageManager::loadData.find(name);
	if (it2 != StorageManager::loadData.end())
		return true;

	return false;
}

/**
@public

@details Scans the Storage looking for the variable with the given name. If one was found, it will be deleted and the pointer
		 it that it was will be added to the Corruption list. It will only add the pointer, if it was in use. Data loaded via StorageManager::load,
		 that was not used won't be added to the list. Returns true, if a variable with this name was removed. Call StorageManager::CorruptedPointers
		 to get a reference to the corrupted pointer list. The newst one will always be pushed back to it.

@param	[in] name	The name of the variable to be removed.

@exception nothrow This function will never throw an exception

@return	true if it succeeds, false if it fails.
 */

bool StorageManager::removeVariable(std::string name)
{
	std::map<std::string, pointerManagerBase*>::iterator it1 = StorageManager::pointers.find(name);
	if (it1 != StorageManager::pointers.end())
	{
		StorageManager::corruption.push_back(it1->second->getPtr());
		delete it1->second;
		StorageManager::pointers.erase(it1);
		return true;
	}

	std::map<std::string, rawPointer>::iterator it2 = StorageManager::loadData.find(name);
	if (it2 != StorageManager::loadData.end())
	{
		delete[] it2->second.rawData;
		StorageManager::loadData.erase(it2);
		return true;
	}

	return false;
}

/**
@public

@details Explicitly stores the data in the Storage in the specified file, using a special XML-Format. All Data, that is allocated in the Storage
		 at the time this function is called will be stored in binary form on your Harddrive, wrapped in XML. @n If you want to change the path or name
		 use the give functions

@remark If @ref TANC004 "STORMAN_CRYPTED" is defined, this function will automatically encrypt the data, before saving it

@attention Because of how the Manager works, in some places symbols will be replaced by others (Only within attributes or texts). It will use the normal XML-escape-sequences,
		   The file itself will be written in ASCII, which may be problematic if open by other programs.

@exception nothrow This function will never throw an exception

@return	true if it succeeds, false if it fails.
 */

bool StorageManager::save()
{
#ifdef STORMAN_CRYPTED
	stringstream file;
#else
	ofstream file;
	file.open(StorageManager::path, ios::out);
	if (!file.is_open())
		return false;
#endif

	file << "<?xml version=\"1.0\" encoding=\"ASCII\" standalone=\"no\"?>" << endl;
	file << "<!DOCTYPE StorageManagerData>" << endl;
	file << "<" << StorageManager::name << ">" << endl;

	for (map<string, pointerManagerBase*>::iterator it = StorageManager::pointers.begin(); it != StorageManager::pointers.end(); it++)
	{
		file << "\t<Data VariableName=\"" << escapeToXml(it->first) << "\" DataSize=\"" << it->second->getSize() << "\" TypeName=\"" << escapeToXml(it->second->getID().name()) << "\" TypeHash=\"" << it->second->getID().hash_code() << "\">";
		string tmp((char*)it->second->getPtr(), it->second->getSize());
		tmp = escapeToXml(tmp);
		file.write(tmp.data(), tmp.size());
		file << "</Data>" << endl;
	}

	for (map<string, rawPointer>::iterator it = StorageManager::loadData.begin(); it != StorageManager::loadData.end(); it++)
	{
		file << "\t<Data VariableName=\"" << escapeToXml(it->first) << "\" DataSize=\"" << it->second.dataSize << "\" TypeName=\"" << escapeToXml(it->second.typeName) << "\" TypeHash=\"" << it->second.hashCode << "\">";
		string tmp((char*)it->second.rawData, it->second.dataSize);
		tmp = escapeToXml(tmp);
		file.write(tmp.data(), tmp.size());
		file << "</Data>" << endl;
	}

	for (map<unsigned long, StorVarCounter>::iterator it = StorageManager::storVars.begin(); it != StorageManager::storVars.end(); it++)
	{
		file << "\t<Link VariableID=\"" << it->first << "\" References=\"" << it->second.references << "\" isUnique=\"" << (it->second.unique ? "yes" : "no") << "\" StringSize=\"" << it->second.name.size() << "\">";
		string tmp((char*)it->second.name.c_str(), it->second.name.size());
		tmp = escapeToXml(tmp);
		file.write(tmp.data(), tmp.size());
		file << "</Link>" << endl;
	}

	file << "</" << StorageManager::name << ">";

#ifdef STORMAN_CRYPTED
	try
	{
		EAX<AES>::Encryption cryptor;
		cryptor.SetKeyWithIV(StorageManager::key.data(), StorageManager::key.size(), StorageManager::iv.data(), StorageManager::iv.size());

		FileSource(file, true, new AuthenticatedEncryptionFilter(cryptor, new FileSink(StorageManager::path.c_str())));
		return true;
	}
	catch (Exception e)
	{
		OutputDebugStringA(e.what());
		return false;
	}
#else
	if (file.good())
	{
		file.close();
		return true;
	}
	else
	{
		file.close();
		return false;
	}
#endif
}

/**
@public

@details Used to load Data from the Harddisk into the StorageManager. If the file is a valid StorageManager-xml-file, nothing should go wrong, but because loading is
		 always harder then saving, this function will write a protokol about what it loaded and when and where errors occured. Because of that it won't throw exceptions.
		 If an error is dectected, it will write it to the Protokol and return false. If everything works without problems, it will return true. But every kind of data
		 read in will be added to the protokol. Call StorageManager::getLoadProtocol to get the protocol

@remark If @ref TANC004 "STORMAN_CRYPTED" is defined, this function will automatically decrypt the file before reading it.

@warning The Manager will automatically call this function on it's construction. To prevent this, or to modify the way this will be done, check the following constants:
			@n @ref TANC001 "STORMAN_NOLOAD"
			@n @ref TANC002 "STORMAN_PERSONAL_NAME"
			@n @ref TANC003 "STORMAN_PERSONAL_PATH"
			@n @ref TANC004 "STORMAN_CRYPTED"

@exception nothrow This function will never throw an exception

@return	true if it succeeds, false if it fails.
 */

bool StorageManager::load()
{
	try
	{
		StorageManager::loadProtokol.clear();

		string inread;
#ifdef STORMAN_CRYPTED
		EAX<AES>::Decryption cryptor;
		cryptor.SetKeyWithIV(StorageManager::key.data(), StorageManager::key.size(), StorageManager::iv.data(), StorageManager::iv.size());

		FileSource(StorageManager::path.c_str(), true, new AuthenticatedDecryptionFilter(cryptor, new StringSink(inread)));
		StorageManager::loadProtokol = "File successfully decrypted\n";
#else
		ifstream file;
		file.open(StorageManager::path, ios::in);
		if (!file.is_open())
		{
			StorageManager::loadProtokol = "Unable to open File";
			return false;
		}
		stringstream strStream;
		strStream << file.rdbuf();
		inread = strStream.str();
#endif

		string::size_type begins, ends;
		string node;

		begins = inread.find('<');
		ends = inread.find('>') + 1;
		node = inread.substr(begins, ends - begins);

		while (node.substr(0, 5) == "<?xml")
		{
			StorageManager::loadProtokol.append("Processing Data: ");
			StorageManager::loadProtokol.append(node);
			StorageManager::loadProtokol.append("\n");
			begins = inread.find('<', ends);
			ends = inread.find('>', ends) + 1;
			node = inread.substr(begins, ends - begins);
		}

		if (node != "<!DOCTYPE StorageManagerData>")
		{
			StorageManager::loadProtokol.append("Invalid Document Type detected: ");
			StorageManager::loadProtokol.append(node);
			StorageManager::loadProtokol.append("\n");
			return false;
		}

		begins = inread.find('<', ends);
		ends = inread.find('>', ends) + 1;
		node = inread.substr(begins, ends - begins);
		node = node.substr(1);
		node.resize(node.size() - 1);

		if (StorageManager::verifyName)
		{
			if (node != StorageManager::name)
			{
				StorageManager::loadProtokol.append("Invalid Name found: ");
				StorageManager::loadProtokol.append(node);
				StorageManager::loadProtokol.append("\n");
				return false;
			}
		}
		else
			StorageManager::name = node;

		for (;;)
		{
			begins = inread.find('<', ends);
			ends = inread.find('>', ends) + 1;
			node = inread.substr(begins, ends - begins - 1);

			if (begins == string::npos && ends == string::npos)
			{
				StorageManager::loadProtokol.append("Document corrupted\n");
				return false;
			}

			if (node == ("</" + StorageManager::name))
				break;

			string attrib;
			vector<xmlAttribute> datamap = getAttribs(escapeFromXml(node), attrib);

			if (attrib == "Data")
			{
				rawPointer ptr;
				string ptrName;

				for (vector<string>::size_type i = 0; i < datamap.size(); i++)
				{
					if (datamap[i].name == "VariableName")
						ptrName = datamap[i].value;
					else if (datamap[i].name == "DataSize")
						ptr.dataSize = (size_t)stoull(datamap[i].value);
					else if (datamap[i].name == "TypeName")
						ptr.typeName = datamap[i].value;
					else if (datamap[i].name == "TypeHash")
						ptr.hashCode = (size_t)stoull(datamap[i].value);
					else
						continue;
				}

				begins = inread.find('<', ends);
				node = escapeFromXml(inread.substr(ends, begins - ends));
				ends = inread.find('>', ends) + 1;
				if (inread.substr(begins, ends - begins) != "</Data>" || node.size() != ptr.dataSize)
				{
					StorageManager::loadProtokol.append("Document corrupted\n");
					return false;
				}

				ptr.rawData = (void*) new char[ptr.dataSize];

				if (memcpy_s(ptr.rawData, ptr.dataSize, node.data(), node.size()) != 0)
				{
					StorageManager::loadProtokol.append("Failed to copy data\n");
					return false;
				}

				if (StorageManager::contains(ptrName))
				{
					switch (StorageManager::loadPolicy)
					{
					case LoadPolicy::KEEP:
						delete[] ptr.rawData;
						StorageManager::loadProtokol.append("Loaded data truncated because of LoadPolicy::KEEP\n");
						break;
					case LoadPolicy::REPLACE:
					case LoadPolicy::KEEPPOINTERS:
						pointerManagerBase *oldPtr = StorageManager::pointers.at(ptrName);

						if (StorageManager::loadPolicy == LoadPolicy::KEEPPOINTERS && oldPtr->getSize() == ptr.dataSize && ptr.typeName == oldPtr->getID().name() && ptr.hashCode == oldPtr->getID().hash_code())
						{
							if (memcpy_s(oldPtr->getPtr(), oldPtr->getSize(), ptr.rawData, ptr.dataSize) != 0)
							{
								StorageManager::loadProtokol.append("Failed to copy data\n");
								return false;
							}
							delete[] ptr.rawData;
							StorageManager::loadProtokol.append("Loaded data replaced previous data (same_sized)\n");
						}
						else if (StorageManager::loadPolicy == LoadPolicy::REPLACE)
						{
							StorageManager::corruption.push_back(oldPtr->getPtr());
							StorageManager::freeStorage(oldPtr->getPtr());
							StorageManager::loadData.insert(pair<string, rawPointer>(ptrName, ptr));
							StorageManager::loadProtokol.append("Loaded data replaced previous data. Old Pointers are corrupted.\nCheck corruption list at: ");
							StorageManager::loadProtokol.append(to_string(StorageManager::corruption.size() - 1));
							StorageManager::loadProtokol.append(". position\n");
						}
						else
							StorageManager::loadProtokol.append("Loaded data truncated because of LoadPolicy::KEEPPOINTERS. Either type or size where different\n");

						break;
					}
				}
				else
				{
					StorageManager::loadData.insert(pair<string, rawPointer>(ptrName, ptr));
					StorageManager::loadProtokol.append("Data with Name \"");
					StorageManager::loadProtokol.append(ptrName);
					StorageManager::loadProtokol.append("\" of Type \"");
					StorageManager::loadProtokol.append(ptr.typeName);
					StorageManager::loadProtokol.append("\" successfully loaded\n");
				}
			}
			else if (attrib == "Link")
			{
				StorVarCounter ctr;
				size_t data_size;
				unsigned long ctrID;

				for (vector<string>::size_type i = 0; i < datamap.size(); i++)
				{
					if (datamap[i].name == "VariableID")
						ctrID = (size_t)stoull(datamap[i].value);
					else if (datamap[i].name == "References")
						ctr.references = (size_t)stoull(datamap[i].value);
					else if (datamap[i].name == "isUnique")
					{
						if (datamap[i].value == "yes")
							ctr.unique = true;
						else if (datamap[i].value == "no")
							ctr.unique = false;
						else
						{
							StorageManager::loadProtokol.append("Document corrupted\n");
							return false;
						}
					}
					else if (datamap[i].name == "StringSize")
						data_size = (size_t)stoull(datamap[i].value);
					else
						continue;
				}

				begins = inread.find('<', ends);
				node = escapeFromXml(inread.substr(ends, begins - ends));
				ends = inread.find('>', ends) + 1;
				if (inread.substr(begins, ends - begins) != "</Link>" || node.size() != data_size)
				{
					StorageManager::loadProtokol.append("Document corrupted\n");
					return false;
				}
				else
					ctr.name = node;

				map<unsigned long, StorVarCounter>::iterator it = StorageManager::storVars.find(ctrID);
				if (it != StorageManager::storVars.end())
				{
					switch (StorageManager::loadPolicy)
					{
					case LoadPolicy::KEEP:
					case LoadPolicy::KEEPPOINTERS:
						StorageManager::loadProtokol.append("Loaded link truncated because of LoadPolicy::KEEP\n");
						break;
					case LoadPolicy::REPLACE:
						unsigned long oldID = it->first;

						StorageManager::corruption.push_back((void*)oldID);
						StorageManager::storVars.erase(it);
						StorageManager::storVars.insert(pair<unsigned long, StorVarCounter>(ctrID, ctr));
						StorageManager::loadProtokol.append("Loaded link replaced previous link. Old links are corrupted.\nCheck corruption list at: ");
						StorageManager::loadProtokol.append(to_string(StorageManager::corruption.size() - 1));
						StorageManager::loadProtokol.append(". position\n");

						break;
					}
				}
				else
				{
					StorageManager::storVars.insert(pair<unsigned long, StorVarCounter>(ctrID, ctr));
					StorageManager::loadProtokol.append("link with ID \"");
					StorageManager::loadProtokol.append(to_string(ctrID));
					StorageManager::loadProtokol.append("\" and VarName\"");
					StorageManager::loadProtokol.append(ctr.name);
					StorageManager::loadProtokol.append("\" successfully loaded\n");
				}
			}
			else
			{
				StorageManager::loadProtokol.append("Document corrupted\n");
				return false;
			}
		}

		return true;
	}
#ifdef STORMAN_CRYPTED
	catch (Exception e)
	{
		StorageManager::loadProtokol.append("Loading terminated becaus of CryptoPP-Exception:\n");
		StorageManager::loadProtokol.append(e.what());
		StorageManager::loadProtokol.append("\n");
		return false;
	}
#endif
	catch (exception e)
	{
		StorageManager::loadProtokol.append("Loading terminated becaus of exception:\n");
		StorageManager::loadProtokol.append(e.what());
		StorageManager::loadProtokol.append("\n");
		return false;
	}
}

/**
@public

@details This function will clear the Manager. This means the current one will be discarded and a new one will be created.

@warning Only call this function if neccessary, because all pointers refering to the Storage will be deleted.
		 Pleace notice, thath if StorageManager::autoSave is set, all data will be saved before clearing. And if load was not disabled, 
		 it will instantly reload the file.

@exception nothrow This function will never throw an exception
*/

void StorageManager::clear()
{
	StorageManager::instance = StorageManager();
}

/**
@public

@details Will return the Protokol, created by the StorageManager::load function

@exception nothrow This function will never throw an exception

@return	The load protocol.
 */

string StorageManager::getLoadProtocol()
{
	return StorageManager::loadProtokol;
}

/**
@public

@details Will return the vector, that contains alls pointers, that have been allocetd before,
		 but are now (because of StorageManager::load, e.g.) are invalid.

@exception nothrow This function will never throw an exception

@return	A reference to the coruption vector.
 */

vector<void*> &StorageManager::CorruptedPointers()
{
	return StorageManager::corruption;
}

/**
@public

@details Will set the nothrow parameter. If it's set to true, the Manager won't throw any exceptions.
		This means all exceptions will be caught by the functions themselfes and their return value will
		indicate that something went wrong

@param	[in] value	the throw-value, false if exceptions should be thrown

@exception nothrow This function will never throw an exception
 */

void StorageManager::setNothrow(bool value)
{
	StorageManager::nothrow = value;
}

/**
@public

@details Will get the nothrow parameter. If it's set to true, the Manager won't throw any exceptions.
		This means all exceptions will be caught by the functions themselfes and their return value will
		indicate that something went wrong

@return returns the nothrow parameter
 */

bool StorageManager::getNothrow()
{
	return StorageManager::nothrow;
}

/**
@public

@details Will set the autoSave parameter. If it's set to true, the Manager will automatically save the Storage on it's destruction

@param	[in] value	the autosave-value

@exception nothrow This function will never throw an exception
 */

void StorageManager::setAutoSave(bool value)
{
	StorageManager::autoSave = value;
}

/**
@public

@details Will get the autoSave parameter. If it's set to true, the Manager will automatically save the Storage on it's destruction

@exception nothrow This function will never throw an exception

@return returns the autoSave parameter
 */

bool StorageManager::getAutoSave()
{
	return StorageManager::autoSave;
}

/**
@public

@details Will set the verifyName parameter. If it's set to true, the Manager will compare the name stated in the file loaded with the setted name.
		If they are different, the operation will fail. If set to false, it will obtain the name

@param	[in] value	the setVerify-value, true if the Manager will try to verify

@exception nothrow This function will never throw an exception
 */

void StorageManager::setVerifyName(bool value)
{
	StorageManager::verifyName = value;
}

/**
@public

@details Will get the verifyName parameter. If it's set to true, the Manager will compare the name stated in the file loaded with the setted name.
		If they are different, the operation will fail. If set to false, it will obtain the name

@exception nothrow This function will never throw an exception

@return returns the verifyName parameter
 */

bool StorageManager::getVerifyName()
{
	return StorageManager::verifyName;
}

/**
@public

@details Will set the loadPolicy parameter. Depending on the load policy, the Manager will behave different on how to react to data duplicates.
		 Check StorageManager::LoadPolicy for more information

@param	[in] value	the LoadPolicy

@exception nothrow This function will never throw an exception
 */

void StorageManager::setLoadPolicy(StorageManager::LoadPolicy value)
{
	StorageManager::loadPolicy = value;
}

/**
@public

@details Will get the loadPolicy parameter. Depending on the load policy, the Manager will behave different on how to react to data duplicates.
		 Check StorageManager::LoadPolicy for more information

@exception nothrow This function will never throw an exception

@return returns the loadPolicy parameter
 */

StorageManager::LoadPolicy StorageManager::getLoadPolicy()
{
	return StorageManager::loadPolicy;
}

/**
@public

@details Will set the name of the Programs Data as a group. This is the name, the data will be saved under, if you call
		 StorageManager::save. The StorageManager::verifyName parameter will specify, how the name affects loading

@param	[in] value	the Program's name

@exception nothrow This function will never throw an exception
 */

void StorageManager::setName(string value)
{
	StorageManager::name = value;
}

/**
@public

@details Will get the name of the Programs Data as a group. This is the name, the data will be saved under, if you call
		 StorageManager::save. The StorageManager::verifyName parameter will specify, how the name affects loading

@exception nothrow This function will never throw an exception

@return returns the current name
 */

string StorageManager::getName()
{
	return StorageManager::name;
}

/**
@public

@details Will set the filepath that will be used to save the data, so basically the file to store the data

@param	[in] value	the file to the Manager

@exception nothrow This function will never throw an exception
 */

void StorageManager::setPath(string value)
{
	StorageManager::path = value;
}

/**
@public

@details Will get the filepath that will be used to save the data, so basically the file to store the data

@exception nothrow This function will never throw an exception

@return returns the current path
 */

string StorageManager::getPath()
{
	return StorageManager::path;
}

#ifdef STORMAN_CRYPTED

/**
@public

@details Will generate a new Key and iv, using the <a href="http://www.cryptopp.com/docs/ref/class_auto_seeded_random_pool.html">AutoSeededRandomPool</a>. Use Keylenght to determine the key's size. They can be retrieved via StorageManager::getCrypted.

@param	[in] keylenght	the Lenght, in Bytes, the AES-Key shoudl have

@exception CryptoPP::Exception will be thrown, if an error within the key generation occures. Check <a href="http://www.cryptopp.com/docs/ref/class_exception.html">Exception</a>
*/

void StorageManager::generateRandom(size_t keylenght)
{
	AutoSeededRandomPool rng;
	StorageManager::key.New(keylenght);
	StorageManager::iv.New(AES::BLOCKSIZE);
	rng.GenerateBlock(key.data(), key.size());
	rng.GenerateBlock(iv.data(), iv.size());
}

/**
@public

@details Will set the data related to encryption, including key and iv, using the <a href="http://www.cryptopp.com/docs/ref/class_sec_block.html">SecByteBlock</a>

@attention If key or iv are empty, the will be resetted. If you set value true, always set key and iv valid, too

@param	[in] value	the crypted-value, true if Crypted mode should be used
@param	[in] key	the key, used for AES-En/Decryption
@param	[in] iv		the iv, used to initialize the AES-En/Decryption

@exception nothrow This function will never throw an exception
*/

void StorageManager::setCrypted(bool value, const CryptoPP::SecByteBlock &key, const CryptoPP::SecByteBlock &iv)
{
	StorageManager::crypted = value;
	StorageManager::key = key;
	StorageManager::iv = iv;
}

/**
@public

@details Will return, if Crypted mode is currently active, and will copy the current key and iv to key and iv.
		 Key and iv can have valid values, even if the Manager currently does not use encryption.
		 Key and iv are <a href="http://www.cryptopp.com/docs/ref/class_sec_block.html">SecByteBlocks</a>.

@param	[out] key	the key, used for AES-En/Decryption
@param	[out] iv	the iv, used to initialize the AES-En/Decryption

@exception nothrow This function will never throw an exception

@return returns if currently crypted mode is active
*/

bool StorageManager::getCrypted(CryptoPP::SecByteBlock &key, CryptoPP::SecByteBlock &iv)
{
	key = StorageManager::key;
	iv = StorageManager::iv;
	return StorageManager::crypted;
}

/**
@public

@overload

@details Overloaded function, Will return, if Crypted mode is currently active, without returning giving key and iv.

@exception nothrow This function will never throw an exception

@return returns if currently crypted mode is activ
*/

bool StorageManager::getCrypted()
{
	return StorageManager::crypted;
}

#endif











static vector<xmlAttribute> getAttribs(string text, string &tagName)
{
	if (text.front() != '<')
		return vector<xmlAttribute>();

	vector<xmlAttribute> retVal;
	bool tag = true;
	bool at = false;
	bool val = false;
	bool val_active = false;
	string tmp_at;
	xmlAttribute tmp_compl;
	for (string::size_type i = 1; i < text.size(); i++)
	{
		if (tag)
		{
			if (text[i] == ' ')
				tag = false;
			else if (text[i] == '>')
				return vector<xmlAttribute>();
			else
				tagName.push_back(text[i]);
		}
		else if (at)
		{
			if (text[i] == '=')
			{
				at = false;
				val = true;
				tmp_compl.name = tmp_at;
				tmp_at.clear();
			}
			else if (text[i] != ' ')
				tmp_at.push_back(text[i]);
		}
		else if (val)
		{
			if (text[i] == '\"')
			{
				if (val_active)
				{
					if (text[i - 1] != '\\')
					{
						val = false;
						val_active = false;
						tmp_compl.value = tmp_at;
						retVal.push_back(tmp_compl);
						tmp_at.clear();
						continue;
					}
				}
				else
				{
					val_active = true;
					continue;
				}
			}

			if (val_active)
				tmp_at.push_back(text[i]);
		}
		else if (text[i] != ' ')
		{
			at = true;
			i--;
		}
	}

	return retVal;
}

static string escapeToXml(const string &ref)
{
	string buffer;
	buffer.reserve((string::size_type)(ref.size() * 1.1));
	for (string::size_type pos = 0; pos != ref.size(); ++pos)
	{
		switch (ref[pos])
		{
		case '&':  buffer.append("&amp;");       break;
		case '\"': buffer.append("&quot;");      break;
		case '\'': buffer.append("&apos;");      break;
		case '<':  buffer.append("&lt;");        break;
		case '>':  buffer.append("&gt;");        break;
		default:   buffer.append(&ref[pos], 1);  break;
		}
	}

	return buffer;
}

static string escapeFromXml(const string &ref)
{
	string buffer;
	buffer.reserve((string::size_type)(ref.size() * 0.9));
	bool searching = false;
	string temp;
	for (size_t pos = 0; pos != ref.size(); ++pos)
	{
		if (searching)
		{
			if (ref[pos] == ';')
			{
				if (temp == "amp")
					buffer.append("&");
				else if (temp == "quot")
					buffer.append("\"");
				else if (temp == "apos")
					buffer.append("\'");
				else if (temp == "lt")
					buffer.append("<");
				else if (temp == "gt")
					buffer.append(">");
				else
					buffer.append(temp);
				temp.clear();
				searching = false;
			}
			else
				temp.append(&ref[pos], 1);
		}
		else if (ref[pos] == '&')
			searching = true;
		else
			buffer.append(&ref[pos], 1);
	}

	return buffer;
}
