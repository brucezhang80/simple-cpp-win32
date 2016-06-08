/**
@file	StorageManager.h

@brief	Declares the StorageManager class.

@author	Felix
@date	28.09.2014
 */
 
/**
@mainpage StorageManager homepage

@section INTRO_SEC Introduction
		Welcome to the StorageManager documentation. The StorageManager is a very small class, only including one file with 2 basic classes.
		Go to the Module page to get an overview of the classes.

@section REASON_SEC The Idea
		The StorageManager was designed because I often had the problem, that I wanted to stored single variables, settings, or similar, after the
        program ends. So I developed this class to make it as easy as possible. The StorageManager simulates a new memory section, that works like the Heap,
		except that, if you don't delete the pointers, they will be saved and restored, when you close and restart the program.
		Thats basically it. But of course there are some more features.

@section CRYPT_SEC The Crypted Mode
		The StorageManager supports encryption of the data stored, too. But this Feature needs <a href="http://www.cryptopp.com/">CryptoPP</a> to be included to you project. But don't worry, if you
		follow the steps how to create the lib-file, you can use the feature without much knowledge of the library. Check @ref TANC004 "STORMAN_CRYPTED" for further information

@author Felix Barz
@date 23.10.2014
*/

#ifndef STORAGEMANAGER_H

/**
@def	STORAGEMANAGER_H

@brief	A macro is defined to prevent multiple includes.
 */

#define STORAGEMANAGER_H

/**
@defgroup MAIN_GROUP Storage Manager Module

@brief The Storage Manager Module contains all classes that are used for this library

@details In short, the StorageManager was created to easily save and manage data beyond the lifespan of the program. Check the StorageManager class for further information.

@{*//** @} */

#include <string>
#include <map>
#include <new>
#include <typeindex>
#include <cassert>
#include <exception>
#include <vector>
#ifdef STORMAN_CRYPTED
#include "SecBlock.h"
#include "aes.h"
#endif

/**
@class	spacer

@brief	Spacer class. Used to allocate memory without allocating an array.
@details This class is used internally to allow the allocation of any given class, using it's size as the template-argument of the class

@tparam	size	Size (in Bytes) of the Spacer
 */

template <size_t size>
class spacer
{
	/** @brief	The space[size]. */
	char space[size];
};

/**
@addtogroup MAIN_GROUP

@{
*/

/**

@class	StorageVariable

@brief	Storage Variable class. Use it to create a reference to variables created on storage

@tparam	T	Generic type parameter, equals the type of the pointer referred to.

@details  This class is basically a wrapper class for pointers created on Storage. It can be used to save "pointers" beyond the execution of the program.
		  @n
		  @b Usage: Create a normale pointer on the Storage, using e.g. StorageManager::getVariable. Then create an object of this class with the created pointers name as parameter.
		  From now on you can use the StorageVariable to access this pointer. After the execution of a Program, Pointers will be invalid, so storing them is useless and can cause the program to crash.
		  A StorageVariable, however, can be stored without any problems. You can use a StorageVariable inside another class, that was created on the storage, to basically store the data of the pointer
		  referred by the StorageVariable. Check the example file shown below.

@example StorageVariableExample.cpp
 */

template <typename T>
class StorageVariable
{
	friend class StorageManager;
private:
	/** @brief	The identifier. */
	unsigned long id;
public:

	/**
	@typedef	T myType
	
	@brief	Defines an alias representing type of the StorageVariable.
	 */

	typedef T myType;


	/** @brief	Default Constructor */
	StorageVariable();
	/** @brief	Constructor */
	StorageVariable(std::string name, bool setUnique = false);
	/** @brief	Copy Constructor */
	StorageVariable(const StorageVariable<T> &other);
	/** @brief	Destructor */
	~StorageVariable();
	
	/** @brief	Checks if this reference is unique */
	bool isUnique() const;
	/** @brief	Checks if this reference is valid */
	bool isValid() const;

	/** @brief	Checks if this reference is valid */
	operator bool() const;
	/** @brief	Checks if this reference is invalid */
	bool operator!() const;

	/** @brief	Returns the pointer to the referenced object */
	T *getPtr();
	/** @brief	returns the size of the referenced object */
	size_t getSize() const;

	/** @brief	 Returns the pointer to the referenced object*/
	operator T*();

	/** @brief	Assignment operator. */
	StorageVariable<T> &operator=(const StorageVariable<T> &value);
	/** @brief	Returns the value of the pointer to the referenced object */
	T &operator*();
	/** @brief	Returns the pointer of the pointer to the referenced object to access class members */
	T* operator->();
};

/**
@class	StorageManager

@brief	A Static class, that simulates a fourth area to declare Variables, which is the Harddrive to keep them alive beyond the Program.

@details The StorageManager is a static class, used to manage data created on the "Storage". The Storage is used as a synonym for data, that should be placed on the Harddrive. @n
         Of course, the Storage is not directly allocted on the harddrive, because this would cause massive slowdown effects. Als long as the program is running, the data will be stored at the heap.
		 The StorageManager has it's own functions to declare and manage variables, that should be placed on the harddrive, and is optimized to do that. It also provides the StorageVariable class, which allows
		 easy access to the Storage. Notice that once you added the StorageManager.cpp file to you project, a StorageManager will automatically be created. The Manager is placed at the datasegment, so it will be
		 constructed before the main routine starts and deleted after it ends. If not specified different, it will load data stored on the Harddrive on construction and save it on destruction.
		 You can use a group of Macros (check @ref TANC001 "STORMAN_NOLOAD") or the memberfunction StorageManager::setAutoSave to modify these.
		 @n
		 With the StorageManager you are able to allocate and free memory, similar to the standard C++ functions (which are internally used). The main differenc is, that if you don't delete them before the program ends, the Manager will take care of them,
		 meaning it will save them permanently on the Harddrive and then free the allocated Memory. And next time you start your program, it will automaticaly load them. The functions to do this are mainly StorageManager::getVariable
		 and StorageManager::getArray. StorageManager::removeVariable can be usefull, too.

@warning Pleace notice: On loading and saving, the Manager won't call any constructors or destructors. When The program ends, the space allocated will be freed, but without calling destructors.
	     You have to make sure, that all data stored is fitted to be stored, so it has to be data, that doesn't need to do any actions on construction or destruction, that aren't saved with the data. Or
		 will get invalid after the Program closes. The Manager works best with primitive data types, but if you follow these rules, it can do much more.

@attention The Manager also supports a crypted mode. But this requires CrytoPP. Check @ref TANC004 "STORMAN_CRYPTED"

@remark If you want to dive a little deeper into the topic, especially about storing Classes that contain stored Variables, check StorageVariable
 */

class StorageManager
{
	template <typename T> friend class StorageVariable;
public:

	/**
	@enum	LoadPolicy
	
	@brief	Possible behaviors to load the data.

	@details Can be used to specify the way, the StorageManager will behave on loading data via StorageManager::load @n Notice that you can not affect the first load, that is done on creation of the Manager
	 */

	enum LoadPolicy
	{
		REPLACE = 0,		/*!< =0 Always replaces already created variables, default behaivior. */
		KEEP = 1,			/*!< =1 Always keeps already created variables, loaded data will be truncated. */
		KEEPPOINTERS = 2	/*!< =2 Behaves like StorageManager::REPLACE, but only if type and size are the same.Will not change the variables address. */
	};
private:

	//Internal subclasses

	class rawPointer
	{
	public:
		void *rawData;
		std::string typeName;
		size_t hashCode;
		size_t dataSize;
	};

	class pointerManagerBase
	{
	public:
		virtual ~pointerManagerBase() {}
		virtual operator void*() { return this->getPtr(); }
		virtual void * getPtr() = 0;
		virtual void setPtr(void *pointer) = 0;
		virtual std::type_index getID() const = 0;
		virtual size_t getSize() const = 0;
		virtual void setSize(size_t byteSize) = 0;
		virtual size_t typeSize() const = 0;
		virtual bool isArray() const = 0;
		virtual bool getOwner() const = 0;
		virtual void setOwner(bool isOwner) = 0;
		virtual void close() = 0;
	};

	template <typename T>
	class pointerManager : public pointerManagerBase
	{
	public:
		T *dataptr;
		bool owner;
		pointerManager(T* data, bool isOwner = true) : dataptr(data), owner(isOwner) {}
		virtual ~pointerManager() { if (owner) delete this->dataptr; this->dataptr = NULL; }
		virtual void * getPtr() { return (void*)this->dataptr; }
		virtual void setPtr(void *pointer) { this->dataptr = (T*)pointer; }
		virtual std::type_index getID() const { return std::type_index(typeid(T)); }
		virtual size_t getSize() const { return sizeof(T); }
        virtual void setSize(size_t byteSize) { (void*)&byteSize; return; }
		virtual size_t typeSize() const { return sizeof(T); }
		virtual bool isArray() const { return false; }
		virtual bool getOwner() const { return this->owner; }
		virtual void setOwner(bool isOwner) { this->owner = isOwner; }
		virtual void close() { if (owner) delete (spacer<sizeof(T)>*)this->dataptr; this->dataptr = NULL; }
	};

	template <typename T>
	class pointerArrayManager : public pointerManager<T>
	{
	public:
		size_t elements;
		pointerArrayManager(T* data, size_t num, bool isOwner = true) : pointerManager<T>(data, isOwner), elements(num) {}
		virtual ~pointerArrayManager() { if (owner) delete[] this->dataptr; this->dataptr = NULL; }
		virtual size_t getSize() const { return sizeof(T)*this->elements; }
		virtual void setSize(size_t byteSize) { this->elements = byteSize / sizeof(T); }
		virtual bool isArray() const { return true; }
		virtual void close() { if (owner) delete[] (spacer<sizeof(T)>*)this->dataptr; this->dataptr = NULL; }
	};

	struct StorVarCounter
	{
		std::string name;
		size_t references;
		bool unique;
	};

	static StorageManager instance;

	//DataMembers
	static std::map<std::string, pointerManagerBase*> pointers;
	static std::map<std::string, rawPointer> loadData;
	static std::map<unsigned long, StorVarCounter> storVars;
	static unsigned long lastID;
	static bool nothrow;
	static std::string path;
	static std::string name;
	static std::string loadProtokol;
	static LoadPolicy loadPolicy;
	static bool autoSave;
	static bool verifyName;
	static bool crypted;
	static std::vector<void*> corruption;

#ifdef STORMAN_CRYPTED
	static CryptoPP::SecByteBlock key;
	static CryptoPP::SecByteBlock iv;
#endif

	//Private (Helper) Functions
	StorageManager();
	StorageManager(const StorageManager &other) = delete;
	~StorageManager();

	/** @brief Moves data from load Space to active space.*/
	template <typename T>
	static T *revivePointer(std::string name);
public:

	/** @brief Alloctes new storage, using the new-operator.*/
	template<typename T, typename... Args>
	static T* allocateStorage(const std::string &varName, Args... params);
	/** @brief Alloctes new storage, using the new[]-operator.*/
	template<typename T>
	static T* allocateStorageArray(const std::string &varName, size_t size);
	/** @brief Frees the storage pointed by storagePointer.*/
	static void freeStorage(void *storagePointer);

	/** @brief Checks, if a variable with this name exists on storage.*/
	static bool contains(std::string name);

	/** @brief Returns the pointer to the variable specified by name.*/
	template <typename T>
	static T *getPointer(std::string name);
	/** @brief Moves a pointer from heap to storage.*/
	template <typename T>
	static bool obtainPointer(std::string name, T *pointer, size_t size, bool replace = true);
	/** @brief Creates a copy of the pointers data.*/
	template <typename T>
	static bool copyPointer(std::string name, T *pointer, size_t size, bool replace = true);
	/** @brief Observes a pointer.*/
	template <typename T>
	static bool observePointer(std::string name, T *pointer, size_t size, bool replace = true);
	/** @brief Takes the pointer from Storage and moves it to the heap*/
	template <typename T>
	static T *releasePointer(std::string name, size_t *size = NULL, bool *isArray = NULL);

	/** @brief Gets a pointer at the Storage, either an existing one or creates a new one*/
	template <typename T, typename... Args>
	static T* getVariable(std::string name, bool enforce = false, Args... params);
	/** @brief Gets an arry pointer at the Storage, either an existing one or creates a new one*/
	template <typename T>
	static T* getArray(std::string name, size_t *size, bool enforce = false, bool enforce_size = false);
	/** @brief Permanently deletes the variable name from the storage.*/
	static bool removeVariable(std::string name);

	/** @brief Creates a StorageVariable to the given Name.*/
	template <typename T, typename... Args>
	static StorageVariable<T> createVariable(std::string name, bool unique = false, bool enforce = false, Args... params);
	/** @brief Creates a StorageVariable to the given Name, array version.*/
	template <typename T>
	static StorageVariable<T> createArray(std::string name, size_t *size, bool unique = false, bool enforce = false, bool enforce_size = false);	//Creates a StorageVariable to the given Name. internally behaves like getArray, but will return a storageVariable instead of a pointer
	/** @brief Removes 1 reference to the variable used by var and makes var invalid.*/
	template <typename T>
	static size_t derefVariable(StorageVariable<T> &var);


	/** @brief Sets the nothrow parameter.*/
	static void setNothrow(bool value);
	/** @brief Gets the nothrow parameter.*/
	static bool getNothrow();
	/** @brief Sets the AutoSave parameter.*/
	static void setAutoSave(bool value);
	/** @brief Gets the AutoSave parameter.*/
	static bool getAutoSave();
	/** @brief Sets the VerifyName parameter.*/
	static void setVerifyName(bool value);
	/** @brief Gets the VerifyName parameter.*/
	static bool getVerifyName();
	/** @brief Sets the LoadPolicy parameter.*/
	static void setLoadPolicy(LoadPolicy value);
	/** @brief Gets the LoadPolicy parameter.*/
	static LoadPolicy getLoadPolicy();
	/** @brief Sets the Name parameter.*/
	static void setName(std::string value);
	/** @brief Gets the Name parameter.*/
	static std::string getName();
	/** @brief Sets the Path parameter.*/
	static void setPath(std::string value);
	/** @brief Gets the Path parameter.*/
	static std::string getPath();

	/** @brief Saves all the Storage data to the Harddrive.*/
	static bool save();
	/** @brief Loads all available data from the Harddrive to the Storage.*/
	static bool load();
	/** @brief Completly clears the manager.*/
	static void clear();

#ifdef STORMAN_CRYPTED
	/** @brief Randomly generates a key and an iv.*/
	static void generateRandom(size_t keylenght = CryptoPP::AES::DEFAULT_KEYLENGTH);
	/** @brief Sets if the file should be crypted with a key.*/
	static void setCrypted(bool value, const CryptoPP::SecByteBlock &key = CryptoPP::SecByteBlock(), const CryptoPP::SecByteBlock &iv = CryptoPP::SecByteBlock());
	/** @brief Gets if the file should be crypted and if needed it's key*/
	static bool getCrypted();
	static bool getCrypted(CryptoPP::SecByteBlock &key, CryptoPP::SecByteBlock &iv);
#endif

	/** @brief Returns the loading-protokol.*/
	static std::string getLoadProtocol();
	/** @brief Returns a reference to a vector, that stores all the pointers, which were deleted on loading.*/
	static std::vector<void*> &CorruptedPointers();
};

/**
@}
*/

	//TEMPLATES

/**
@public
@details This function is the equivalent of the new-operator for the Storage. It will create a pointer with the given parameters on the Storage.
		 Additionally, you need to name the pointer to identify it. If the allocation via new fails, or if a varibale with the name already exists,
		 the function will fail, either throwing an exception or returning NULL.

@tparam	T   	Pointer type parameter.
@tparam	Args	Variadic template.
@param	[in] varName	Name of the variable.
@param	[in] params 	Variable arguments as parameters give to the constructor of the object.(optional)

@exception std::exception Throws a normal exception, if a variable with this name already exists
@exception std::bad_alloc Throws a bad_alloc, if the new-operator fails (will rethrow this exception)

@return	NULL-ptr if it fails, else a T*.
 */

template<typename T, typename... Args>
T* StorageManager::allocateStorage(const std::string &varName, Args... params)
{
	T *ret = NULL;
	try
	{
		if (StorageManager::contains(varName))
			throw std::exception("Variable already exists, use getPointer to access it or delete it");
		ret = new T(params...);
		StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(varName, new pointerManager<T>(ret)));
		return ret;
	}
	catch (...)
	{
		if (ret != NULL)
			delete ret;
		if (StorageManager::nothrow)
			return NULL;
		else
			throw;
	}
}

/**
@public

@details This function is the equivalent of the new[]-operator for the Storage. It will create a pointer to an arry with the default constructor on the Storage.
		 Additionally, you need to name the pointer to identify it. If the allocation via new fails, or if a varibale with the name already exists,
		 the function will fail, either throwing an exception or returning NULL.

@tparam	T   	Pointer type parameter.
@param	[in] varName	Name of the variable.
@param	[in] size 	The size (in number of objects inside the array), that the arry should have

@exception std::exception Throws a normal exception, if a variable with this name already exists
@exception std::bad_alloc Throws a bad_alloc, if the new-operator fails (will rethrow this exception)

@return	NULL-ptr if it fails, else a T*.
 */

template<typename T>
T* StorageManager::allocateStorageArray(const std::string &varName, size_t size)
{
	T *ret = NULL;
	try
	{
		if (StorageManager::contains(varName))
			throw std::exception("Variable already exists, use getPointer to access it or delete it");
		ret = new T[size];
		StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(varName, new pointerArrayManager<T>(ret, size)));
		return ret;
	}
	catch (...)
	{
		if (ret != NULL)
			delete[] ret;
		if (StorageManager::nothrow)
			return NULL;
		else
			throw;
	}
}

/**
@public

@details	Scans the Storage for a Variable with the given Name, and if it exists, returns the pointer to it. Otherwise retuens the NULL-pointer.
			If the Variable was only loaded, not used yet, this function will call StorageManager::revivePointer, to move the Variable from load Space to active Space.
			This can slow down the process a little bit, and if an internal error occures, throw exceptions

@tparam	T	Pointer type parameter.
@param	[in] name	Name of the variable.

@exception std::exception Indicates, that an internal copy-action failed or the types are different
@exception std::bad_alloc Throws a bad_alloc, if the new-operator fails (will rethrow this exception)

@return	NULL if it fails, else the pointer.
 */

template <typename T>
T *StorageManager::getPointer(std::string name)
{
	try
	{
		std::map<std::string, pointerManagerBase*>::iterator it = StorageManager::pointers.find(name);
		if (it == StorageManager::pointers.end())
			return StorageManager::revivePointer<T>(name);
		if (std::type_index(typeid(T)) == it->second->getID())
			return (T*)it->second->getPtr();
		else
			throw std::exception("The Variable with the given name is of a different type");
	}
	catch (...)
	{
		if (StorageManager::nothrow)
			return NULL;
		else
			throw;
	}
}

/**
@public

@details Will move a pointer from loaded Space to active space, making it possible to access it. Will only be called internally

@tparam	T	Pointer type parameter.
@param	[in] name	Name of the variable..

@exception std::exception Indicates, that an internal copy-action failed or the types are different
@exception std::bad_alloc Throws a bad_alloc, if the new-operator fails (will rethrow this exception)

@return	NULL if it fails, else the pointer.
 */

template <typename T>
T *StorageManager::revivePointer(std::string name)
{
	try
	{
		std::map<std::string, rawPointer>::iterator it = StorageManager::loadData.find(name);
		if (it != StorageManager::loadData.end())
		{
			std::type_index idn(typeid(T));
			if (it->second.typeName == idn.name() && it->second.hashCode == idn.hash_code())
			{
				size_t size = it->second.dataSize / sizeof(T);
				T *ptr = NULL;
				if (size == 1)
				{
					try
					{
						ptr = (T*)new spacer < sizeof(T) > ;
						if (memcpy_s(ptr, sizeof(T), it->second.rawData, it->second.dataSize) != 0)
							throw std::exception("Faild to copy data");
						delete[] it->second.rawData;
					}
					catch (...)
					{
						if (ptr != NULL)
							delete ptr;
						if (StorageManager::nothrow)
							return NULL;
						else
							throw;
					}
					StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(name, new pointerManager<T>(ptr)));
					StorageManager::loadData.erase(it);
				}
				else
				{
					ptr = (T*)it->second.rawData;
					StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(name, new pointerArrayManager<T>(ptr, size)));
					StorageManager::loadData.erase(it);
				}

				return ptr;
			}
			else
				throw std::exception("The Variable with the given name is of a different type");
		}

		return NULL;
	}
	catch (...)
	{
		if (StorageManager::nothrow)
			return NULL;
		else
			throw;
	}
}

/**
@public

@details This function will move pointer from the heap to the Storage. Internally, it will take ownership of the given pointer, from now on beeing responsible for the pointer.
		 Can be used to transfer pointers to the Storage.

@warning Be careful with this function, because once the Storage obtained the pointer, it later will delete it. So don't pass pointer returned by functions or from variables, that are not located at the Heap.
		 Only use, if you should later delete the pointer - and if you move the pointer, don't delete it yourself. @n Another important thing to take care of: Do never obtain pointers, like the std::vector::data function will return you,
		 because the vector won't give you the ownership, it will keep it. Use StorageManager::copyPointer to save such data.

@tparam	T	Pointer type parameter.
@param	[in] name		The name of the Variable.
@param  [in] pointer	The pointer to obtain.
@param	[in] size		The size of the pointer (in Elements).
@param	[in] replace	If an variable with this name already exists, set true to replace it, false to keep it. Default is true. (optional)

@exception nothrow This function never throws exceptions

@return	true if the pointer was successfully obtained, false if not.
 */

template <typename T>
bool StorageManager::obtainPointer(std::string name, T *pointer, size_t size, bool replace)
{
	if (StorageManager::contains(name))
	{
		if (replace)
			StorageManager::removeVariable(name);
		else
			return false;
	}

	if (size == 1)
		StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(name, new pointerManager<T>(pointer)));
	else
		StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(name, new pointerArrayManager<T>(pointer, size)));

	return true;
}

/**
@public

@details Can be used to copy the data pointed by a pointer. It will create a new pointer with the spacer at the Storage and create a binary copy of the given pointer.
		 Won't call any constructors or other functions on the copied object on construction, but it will call the destructor if removed

@tparam	T	Pointer type parameter.
@param	[in] name		The name of the Variable.
@param  [in] pointer	The pointer to obtain.
@param	[in] size		The size of the pointer (in Elements).
@param	[in] replace	If an variable with this name already exists, set true to replace it, false to keep it. Default is true. (optional)

@exception std::bad_alloc Will be thrown, if new fails

@return	true if the pointer was successfully copied, false if not.
 */

template <typename T>
bool StorageManager::copyPointer(std::string name, T *pointer, size_t size, bool replace)
{
	if (StorageManager::contains(name))
	{
		if (replace)
			StorageManager::removeVariable(name);
		else
			return false;
	}

    T *ptr = NULL;
	try
	{
		if (size == 1)
            ptr = (T*) new spacer<sizeof(T)>;
		else
            ptr = (T*) new spacer<sizeof(T)>[size];
	}
	catch (...)
	{
		if (StorageManager::nothrow)
			return NULL;
		else
			throw;
	}
	if (memcpy_s(ptr, size * sizeof(T), pointer, size * sizeof(T)) != 0)
		return false;
	if (size == 1)
        StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(name, new pointerManager<T>(ptr)));
	else
        StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(name, new pointerArrayManager<T>(ptr, size)));

	return true;
}

/**
@public

@details Can be used to obsever the given pointer. Observation in this case means, that the pointer will be added to the StorageManager, but the manager won't take ownership of the pointer, which means it won't delete it, even if you call
		 StorageManager::removeVariable or StorageManager::freeStorage.

@warning Be careful with this function. The Manager won't notice, if the pointer gets invalid. So if you use auto-save, make shure the given pointer won't be deleted before the Manager. (Manager is located in DataSegment).
@warning NEVER use this function on pointers returned by functions like std::vector::data or std::string::data, because these pointers ar not consistent. As soon as you change the content of e.g. a string, it will delete
		 the old pointer and create a new one - which will cause the Manager to crush. If you want to store the data such a pointer referes to, use StorageManager::copyPointer

@tparam	T	Pointer type parameter.
@param	[in] name		The name of the Variable.
@param  [in] pointer	The pointer to obtain.
@param	[in] size		The size of the pointer (in Elements).
@param	[in] replace	If an variable with this name already exists, set true to replace it, false to keep it. Default is true. (optional)

@exception nothrow Never throws an exception

@return	true if the pointer was successfully copied, false if not.
 */

template <typename T>
bool StorageManager::observePointer(std::string name, T *pointer, size_t size, bool replace)
{
	if (StorageManager::contains(name))
	{
		if (replace)
			StorageManager::removeVariable(name);
		else
			return false;
	}

	if (size == 1)
		StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(name, new pointerManager<T>(pointer, false)));
	else
		StorageManager::pointers.insert(std::pair<std::string, pointerManagerBase*>(name, new pointerArrayManager<T>(pointer, size, false)));

	return true;
}

/**
@public

@details Will detach the given variable frome the Storage, meaning it will completly be removed from the Storage and moved to the heap. For the Manager it does the same as StorageManager::removeVariable,
		 but without deleting it from the Heap

@tparam	T	Generic type parameter.
@param	[in] name		The name of the Variable.
@param [out] size   	If non-NULL, the size of the released pointer will be written to size. (optional)
@param [out] isArray	If non-NULL, will be set to true if the pointer was created by new[], else is set to false (Use to finde out if delete or delete[] should be used). (optional)

@exception std::exception Indicates, that the types are different

@return	NULL if there is no pointer with this name or the type is not the same, else the pointer.
 */

template <typename T>
T *StorageManager::releasePointer(std::string name, size_t *size, bool *isArray)
{
	try
	{
		std::map<std::string, pointerManagerBase*>::iterator it1 = StorageManager::pointers.find(name);
		if (it1 != StorageManager::pointers.end())
		{
			if (std::type_index(typeid(T)) != it1->second->getID())
				throw std::exception("The Variable with the given name is of a different type");
			it1->second->setOwner(false);
			T *ret = (T*)it1->second->getPtr();
			if (size != NULL)
				*size = it1->second->getSize();
			if (isArray != NULL)
				*isArray = it1->second->isArray();
			delete it1->second;
			StorageManager::pointers.erase(it1);
			return ret;
		}

		std::map<std::string, rawPointer>::iterator it2 = StorageManager::loadData.find(name);
		if (it2 != StorageManager::loadData.end())
		{
			T* ret = StorageManager::revivePointer<T>(name);
			if (ret == NULL)
				return ret;
			else
				return StorageManager::releasePointer<T>(name, size, isArray);
		}

		return NULL;
	}
	catch (...)
	{
		if (StorageManager::nothrow)
			return NULL;
		else
			throw;
	}
}

/**
@public

@details This function is basically a combination of StorageManager::allocateStorage and StorageManager::getPointer. On call, it will first search the Manager for a variable with the given name.
		 If it was found, and is of the same type, it's pointer will be returned. If it was not found, or if types are different, but enforece is set to true, a new variable with this name will be created
		 (using the given parameters) and returned. @n
@attention	This function will fail/recreate, if you want to get a previously created variable of given type and name, but an array

@tparam	T   	Pointer type parameter.
@tparam	Args	Variadic template.
@param	[in] name   	The name of the variable.
@param	[in] enforce	true to enforce the creation, if a variable with the name already exists, but is different. If true, the old one will be deleted, if false, the function fails. Default is false. (optional)
@param	[in] params 	Variable arguments, that will be given to the constructor, if a new construction is needed (Will be ignored, if the variable already exists). (optional)

@exception std::bad_alloc Will be thrown, if a new-operator fails
@exception std::exception Will be thrown, if enforce is set to false, but the pointer found has a different type or is an array.

@return	NULL if the Variable has a different type/size as the given one, else the pointer to the variable.
 */

template <typename T, typename... Args>
T* StorageManager::getVariable(std::string name, bool enforce, Args... params)
{
	if (StorageManager::contains(name))
	{
		try
		{
			std::map<std::string, pointerManagerBase*>::iterator it = StorageManager::pointers.find(name);
			if (it != StorageManager::pointers.end())
			{
				if (std::type_index(typeid(T)) == it->second->getID() && !it->second->isArray())
					return (T*)it->second->getPtr();
				else
				{
					if (enforce)
					{
						delete it->second;
						StorageManager::pointers.erase(it);
						return StorageManager::allocateStorage<T>(name, params...);
					}
					else
						throw std::exception("Variable is of different type");
				}
			}
			else
			{
				T* ptr = StorageManager::revivePointer<T>(name);
				if (ptr == NULL)
					throw std::exception("Variable is of different type");
				else
				{
					std::map<std::string, pointerManagerBase*>::iterator it = StorageManager::pointers.find(name);
					if (it->second->isArray())
						throw std::exception("Variable is an array");
					else
						return ptr;
				}
			}
		}
		catch (...)
		{
			if (StorageManager::nothrow)
				return NULL;
			else
				throw;
		}
	}
	else
		return StorageManager::allocateStorage<T>(name, params...);
}

/**
@public

@details This function is basically a combination of StorageManager::allocateStorageArray and StorageManager::getPointer. On call, it will first search the Manager for a variable with the given name.
		 If it was found, and is of the same type (and an array), it's pointer will be returned. If it was not found, or if types are different, but enforce is set to true, a new Variable with this name will be created
		 (using the given parameters) and returned. @n
@attention	This function will fail/recreate, if you want to get a previously created bariable of given type and name, but it's not an array. Using the enforce_size parameter, you can specify, how the function will behave,
			if only the size is not the same as given

@tparam	T   	Pointer type parameter.
@param	[in] name   		The name of the variable.
@param	[in, out] size   	The size(in elements) the array should have/has.
@param	[in] enforce		true to enforce the creation, if a variable with the name already exists, but is different. If true, the old one will be deleted, if false, the function fails. Default is false. (optional)
@param	[in] enforce_size	true to enforce the the size of the array. If the array found has a different size and this is set to true, the function will fail/recretead (depending on enfroce). Default is false. (optional)

@exception std::bad_alloc Will be thrown, if a new-operator fails
@exception std::exception Will be thrown, if enforce is set to false, but the pointer found has a different type (or size) or is not an array.

@return	NULL if the Variable has a different type/size as the given one, else the pointer to the variable.
 */

template <typename T>
T* StorageManager::getArray(std::string name, size_t *size, bool enforce, bool enforce_size)
{
	if (StorageManager::contains(name))
	{
		try
		{
			std::map<std::string, pointerManagerBase*>::iterator it = StorageManager::pointers.find(name);
			if (it != StorageManager::pointers.end())
			{
				if (std::type_index(typeid(T)) == it->second->getID() && it->second->isArray())
				{
					if (enforce_size && it->second->getSize() != *size * sizeof(T))
					{
						if (enforce)
						{
							delete it->second;
							StorageManager::pointers.erase(it);
							return StorageManager::allocateStorageArray<T>(name, *size);
						}
						else
							throw std::exception("Array is of different size");
					}
					else
					{
						*size = it->second->getSize() / sizeof(T);
						return (T*)it->second->getPtr();
					}
				}
				else
				{
					if (enforce)
					{
						StorageManager::pointers.erase(it);
                        return StorageManager::allocateStorageArray<T>(name, *size);
					}
					else
						throw std::exception("Variable is of different type");
				}
			}
			else
			{
				T* ptr = StorageManager::revivePointer<T>(name);
				if (ptr == NULL)
					throw std::exception("Variable is of different type");
				else
				{
					std::map<std::string, pointerManagerBase*>::iterator it = StorageManager::pointers.find(name);
					if (it->second->isArray())
					{
                        if (enforce_size && it->second->getSize() != *size * sizeof(T))
						{
							if (enforce)
							{
								delete it->second;
								StorageManager::pointers.erase(it);
                                return StorageManager::allocateStorageArray<T>(name, *size);
							}
							else
								throw std::exception("Array is of different size");
						}
						else
						{
							*size = it->second->getSize() / sizeof(T);
							return ptr;
						}
					}
					else
					{
						if (enforce)
						{
							delete it->second;
							StorageManager::pointers.erase(it);
                            return StorageManager::allocateStorageArray<T>(name, *size);
						}
						else
							throw std::exception("Variable is not an array");
					}
				}
			}
		}
		catch (...)
		{
			if (StorageManager::nothrow)
				return NULL;
			else
				throw;
		}
	}
	else
        return StorageManager::allocateStorageArray<T>(name, *size);
}

/**
@public

@details This function will create a StorageVariable. It internally behaves like StorageManager::getVariable.
		 But after retrieving the pointer, it will create a StorageVariable from it. If unique is set to true, a new variable with id will be created, if not, it will share it's ID with other
		 StorageVariables. If unique is the to true, the Variable will be uncopyable. It's not recommended to make a StorageVariable unique, because making it shareable this will save Memory and Storage.

@tparam	T   	StorageVariable type parameter.
@tparam	Args	Variadic Template.
@param	[in] name   	The name of the Variable to refer to.
@param	[in] unique 	true if the Variable should be unique. Default is false. (optional)
@param	[in] enforce	true to enforce the creation, if a variable with the name already exists, but is different. If true, the old one will be deleted, if false, the function fails. Default is false. (optional)
@param	[in] params 	Variable arguments, that will be given to the constructor, if a new construction is needed (Will be ignored, if the variable already exists). (optional)

@exception std::bad_alloc Will be thrown, if a new-operator fails
@exception std::exception Will be thrown, if enforce is set to false, but the pointer found has a different type (or size), is an array or there are no free indexes for a Variable left.

@return	The new StorageVariable, refering to the pointer with the given name (Check StorageVariable::isValid to find out if this function succeeded or not).
 */

template <typename T, typename... Args>
StorageVariable<T> StorageManager::createVariable(std::string name, bool unique, bool enforce, Args... params)
{
	StorageVariable<T> ret;
	if(StorageManager::getVariable<T>(name, enforce, params...) == NULL)
		return ret;
	else
	{
		if (unique)
		{
			StorageManager::lastID++;
			while (StorageManager::storVars.find(StorageManager::lastID) != StorageManager::storVars.end())
			{
				if (StorageManager::lastID == 0)
					throw std::exception("Out of StorageVariables you used all (2^32)-1 of them...");
				StorageManager::lastID++;
			}

			StorVarCounter ctr = {name, 1, true};
			StorageManager::storVars.insert(std::pair<unsigned long, StorVarCounter>(StorageManager::lastID, ctr));

			ret.id = StorageManager::lastID;

			return ret;
		}
		else
		{
			for (std::map<unsigned long, StorVarCounter>::iterator it = StorageManager::storVars.begin(); it != StorageManager::storVars.end(); it++)
			{
				if (it->second.name == name && !it->second.unique)
				{
					ret.id = it->first;
					it->second.references++;

					return ret;
				}
			}

			StorageManager::lastID++;
			while (StorageManager::storVars.find(StorageManager::lastID) != StorageManager::storVars.end())
			{
				if (StorageManager::lastID == 0)
					throw std::exception("Out of StorageVariables you used all (2^32)-1 of them...");
				StorageManager::lastID++;
			}

			StorVarCounter ctr = {name, 1, false};
			StorageManager::storVars.insert(std::pair<unsigned long, StorVarCounter>(StorageManager::lastID, ctr));

			ret.id = StorageManager::lastID;

			return ret;
		}
	}
}

/**
@public

@details This function will create a StorageVariable to an array. The function internally behaves like StorageManager::getArray.
		 But after retrieving the pointer, it will create a StorageVariable from it. If unique is set to true, a new Variable with id will be created, if not, it will share it's ID with other
		 StorageVariables. If Unique is the to true, the Variable will be uncopyable. It's not recommended to make a StorageVariable unique, because making it shareable this will save Memory and Storage.

@tparam	T   	StorageVariable type parameter.
@param	[in] name   	The name of the Variable to refer to.
@param	[in, out] size   	The size(in elements) the array should have/has.
@param	[in] unique 	true if the Variable should be unique. Default is false. (optional)
@param	[in] enforce		true to enforce the creation, if a variable with the name already exists, but is different. If true, the old one will be deleted, if false, the function fails. Default is false. (optional)
@param	[in] enforce_size	true to enforce the the size of the array. If the array found has a different size and this is set to true, the function will fail/recretead (depending on enfroce). Default is false. (optional)

@exception std::bad_alloc Will be thrown, if a new-operator fails
@exception std::exception Will be thrown, if enforce is set to false, but the pointer found has a different type (or size), is not an array or there are no free indexes for a Variable left.

@return	The new StorageVariable, refering to the pointer with the given name (Check StorageVariable::isValid to find out if this function succeeded or not).
 */

template <typename T>
StorageVariable<T> StorageManager::createArray(std::string name, size_t *size, bool unique, bool enforce, bool enforce_size)
{
	StorageVariable<T> ret;
	if (StorageManager::getArray<T>(name, size, enforce, enforce_size) == NULL)
		return ret;
	else
	{
		if (unique)
		{
			StorageManager::lastID++;
			while (StorageManager::storVars.find(StorageManager::lastID) != StorageManager::storVars.end())
			{
				if (StorageManager::lastID == 0)
					throw std::exception("Out of StorageVariables you used all (2^32)-1 of them...");
				StorageManager::lastID++;
			}

			StorVarCounter ctr = {name, 1, true};
			StorageManager::storVars.insert(std::pair<unsigned long, StorVarCounter>(StorageManager::lastID, ctr));

			ret.id = StorageManager::lastID;

			return ret;
		}
		else
		{
			for (std::map<unsigned long, StorVarCounter>::iterator it = StorageManager::storVars.begin(); it != StorageManager::storVars.end(); it++)
			{
				if (it->second.name == name && !it->second.unique)
				{
					ret.id = it->first;
					it->second.references++;

					return ret;
				}
			}

			StorageManager::lastID++;
			while (StorageManager::storVars.find(StorageManager::lastID) != StorageManager::storVars.end())
			{
				if (StorageManager::lastID == 0)
					throw std::exception("Out of StorageVariables you used all (2^32)-1 of them...");
				StorageManager::lastID++;
			}

			StorVarCounter ctr = {name, 1, false};
			StorageManager::storVars.insert(std::pair<unsigned long, StorVarCounter>(StorageManager::lastID, ctr));

			ret.id = StorageManager::lastID;

			return ret;
		}
	}
}

/**
@public

@details Will dereference and delete the given StorageVariable (Without deleting the pointer it refers to). This is needed because many StorageVariables share one ID. This way dead links
		 won't occure. This function will automatically be called in the destructor of the Variable. If the Dereference succeeds, the StorageVariable will be made invalid. If this function
		 is used on an invalid StorageVariable, it won't do anything

@tparam	T	Type of the StorageVariable.
@param [in,out]	var	The StorageVariable, that should be deleted.

@exception nothrow This function never throws exceptions

@return	The number of references left on the Variable the StorageVariable was refering to.
 */

template <typename T>
size_t StorageManager::derefVariable(StorageVariable<T> &var)
{
	size_t ret = 0;
	std::map<unsigned long, StorVarCounter>::iterator it = StorageManager::storVars.find(var.id);

	if (it != StorageManager::storVars.end())
	{
		if (it->second.unique)
			StorageManager::storVars.erase(it);
		else
		{
			it->second.references--;
			ret = it->second.references;
			if (it->second.references == 0)
				StorageManager::storVars.erase(it);
		}
	}

	var.id = 0;

	return ret;
}

/**
@public

@details Constructs a basic StorageVariable object. The object will be invalid, because it won't refere to any Storage

@exception nothrow This function never throws exceptions
 */

template <typename T>
StorageVariable<T>::StorageVariable() :
	id(0)
{}

/**
@public

@details Normal way to construct a StorageVariable. The constructor will search the Storage for a variable with the given name, and if type and name match create a reference to this pointer.
		 This Variable is guaranteed to live beyond the program, as long as the pointer it referes to is not deleted (using StorageManager::freeStorage or StorageManager::removeVariable).
		 If the constructor fails to find the variable or the type won't fit, the created object will be invalid. Use StorageVariable::isValid to check.

@attention This function will not create a new variable, if the given one does not exist. To do this use StorageManager::createVariable or StorageManager::createArray

@param	[in] name	 	The name of the Variable this object should refere to.
@param	[in] setUnique	If unique is set true, the Variable will use a unique ID, that won't be shared with others (but may share the pointer itself). Default is false. (Optional)

@exception std::exception Thrown by StorageManager::getPointer
@exception std::bad_alloc Thrown by StorageManager::getPointer
 */

template <typename T>
StorageVariable<T>::StorageVariable(std::string name, bool setUnique) :
	id(0)
{
	if (StorageManager::contains(name))
	{
		try
		{
			if (StorageManager::getPointer<T>(name) == NULL)
				return;

			if (setUnique)
			{
				StorageManager::lastID++;
				while (StorageManager::storVars.find(StorageManager::lastID) != StorageManager::storVars.end())
				{
					if (StorageManager::lastID == 0)
						throw std::exception("Out of StorageVariables you used all (2^32)-1 of them...");
					StorageManager::lastID++;
				}

				StorageManager::StorVarCounter ctr = {name, 1, true};
				StorageManager::storVars.insert(std::pair<unsigned long, StorageManager::StorVarCounter>(StorageManager::lastID, ctr));

				this->id = StorageManager::lastID;
			}
			else
			{
				for (std::map<unsigned long, StorageManager::StorVarCounter>::iterator it = StorageManager::storVars.begin(); it != StorageManager::storVars.end(); it++)
				{
					if (it->second.name == name && !it->second.unique)
					{
						this->id = it->first;
						it->second.references++;
					}
				}

				StorageManager::lastID++;
				while (StorageManager::storVars.find(StorageManager::lastID) != StorageManager::storVars.end())
				{
					if (StorageManager::lastID == 0)
						throw std::exception("Out of StorageVariables you used all (2^32)-1 of them...");
					StorageManager::lastID++;
				}

				StorageManager::StorVarCounter ctr = {name, 1, false};
				StorageManager::storVars.insert(std::pair<unsigned long, StorageManager::StorVarCounter>(StorageManager::lastID, ctr));

				this->id = StorageManager::lastID;
			}

		}
		catch (...)
		{}
	}
}

/**
@public

@details Will create a copy of the given StorageVariable. This will only succeed, if the other Variable is not unique

@param	[in] other	The other StorageVariable.

@exception nothrow This function never throws exceptions
 */

template <typename T>
StorageVariable<T>::StorageVariable(const StorageVariable<T> &other) :
	id(0)
{
	std::map<unsigned long, StorageManager::StorVarCounter>::iterator it = StorageManager::storVars.find(other.id);
	if (it != StorageManager::storVars.end() && !it->second.unique)
	{
		this->id = it->first;
		it->second.references++;
	}
}

/**
@public

@details Will destroy the Variable, removing itself from the StorageManager using StorageManager::derefVariable

@exception nothrow This function never throws exceptions
*/

template <typename T>
StorageVariable<T>::~StorageVariable()
{
	StorageManager::derefVariable<T>(*this);
}

/**
@public

@exception nothrow This function never throws exceptions

@return	True if the Object is unique, false if not.
 */

template <typename T>
bool StorageVariable<T>::isUnique() const
{
	std::map<unsigned long, StorageManager::StorVarCounter>::iterator it = StorageManager::storVars.find(this->id);
	if (it != StorageManager::storVars.end())
		return it->second.unique;
	else
		return false;
}

/**
@public

@exception nothrow This function never throws exceptions

@return	True if the object valid, false if not.
 */

template <typename T>
bool StorageVariable<T>::isValid() const
{
	return (this->id == 0 ? false : true);
}

/**
@public

@exception nothrow This function never throws exceptions

@return	True if the object valid, false if not.
 */

template <typename T>
StorageVariable<T>::operator bool() const
{
	return (this->id == 0 ? false : true);
}

/**
@public

@exception nothrow This function never throws exceptions

@return	False if the object valid, true if not.
 */

template <typename T>
bool StorageVariable<T>::operator!() const
{
	return (this->id == 0 ? true : false);
}

/**
@public

@details Will scan the StorageManager for the Variable it refers to, and return it's pointer, if it exists. Otherways returns the NULL-pointer. This method my be a little slow, but it guarantees,
		 that the pointer will be found, even if it changes. If you know that you won't change the pointer while the Programm is running, only call this function after new Data was loaded and store
		 the Pointer given. This is save, as long as you neither delete the Variable nor call StorageManager::load

@exception std::exception Thrown by StorageManager::getPointer
@exception std::bad_alloc Thrown by StorageManager::getPointer

@return	The Pointer of the Variable refering to, NULL-ptr if it's not available
 */

template <typename T>
T *StorageVariable<T>::getPtr()
{
	std::map<unsigned long, StorageManager::StorVarCounter>::iterator it = StorageManager::storVars.find(this->id);
	if (it != StorageManager::storVars.end())
		return StorageManager::getPointer<T>(it->second.name);
	else
		return NULL;
}

/**
@public

@details Will scan the StorageManager for the Variable it refers to, and return it's size, if it exists. Otherways returns 0. This Method my be a little slow, but it guarantees,
		 that the Variable will be found, even if its pointer changes. If you know that you won't change the pointer while the Programm is running, only call this function after new Data was loaded and store
		 the size given. This is save, as long as you neither delete the Variable nor call StorageManager::load

@exception std::exception Thrown by StorageManager::getPointer
@exception std::bad_alloc Thrown by StorageManager::getPointer

@return	The size of the Data the object is pointing to. 0 if the Variable can't be found.
 */

template <typename T>
size_t StorageVariable<T>::getSize() const
{
	std::map<unsigned long, StorageManager::StorVarCounter>::iterator it = StorageManager::storVars.find(this->id);
	if (it != StorageManager::storVars.end())
	{
		if (StorageManager::getPointer<T>(it->second.name) == NULL)
			return 0;
		else
		{
			std::map<std::string, StorageManager::pointerManagerBase*>::iterator it2 = StorageManager::pointers.find(it->second.name);
			return it2->second->getSize();
		}
	}
	else
		return 0;
}

/**
@public

@details Does the same as StorageVariable::getPtr

@exception std::exception Thrown by StorageManager::getPointer
@exception std::bad_alloc Thrown by StorageManager::getPointer

@return	The Pointer of the Variable refering to, NULL-ptr if it's not available
 */

template <typename T>
StorageVariable<T>::operator T*()
{
	std::map<unsigned long, StorageManager::StorVarCounter>::iterator it = StorageManager::storVars.find(this->id);
	if (it != StorageManager::storVars.end())
		return StorageManager::getPointer<T>(it->second.name);
	else
		return NULL;
}

/**
@public

@details The assigment-operator. Will create a copy of the given StorageVariable. This will only succeed, if the other Variable is not unique

@param [in] value	The StorageVariable that should be copied

@exception nothrow This function never throws exceptions

@return	The object itself
 */

template <typename T>
StorageVariable<T> &StorageVariable<T>::operator=(const StorageVariable<T> &value)
{
	this->id = 0;
	std::map<unsigned long, StorageManager::StorVarCounter>::iterator it = StorageManager::storVars.find(value.id);
	if (it != StorageManager::storVars.end() && !it->second.unique)
	{
		this->id = it->first;
		it->second.references++;
	}

	return *this;
}

/**
@public

@details The dereferentiation operator. Is basically a shortcut for *(StorageVariable::getPtr()).
@warning Be careful with this one. If the Variable this object is refering to is not existing anymore or the object itself is not valid, the function will try to dereference the NULL-pointer *(NULL). This will cause the Program to crash.

@exception std::exception Thrown by StorageManager::getPointer
@exception std::bad_alloc Thrown by StorageManager::getPointer

@return	Value of the Variable the object is refering to
 */

template <typename T>
T &StorageVariable<T>::operator*()
{
	std::map<unsigned long, StorageManager::StorVarCounter>::iterator it = StorageManager::storVars.find(this->id);
	if (it != StorageManager::storVars.end())
		return *(StorageManager::getPointer<T>(it->second.name));
	else
		return *((T*)NULL);
}

/**
@public

@details overload for the member access operator. Can be used to access class members

@exception std::exception Thrown by StorageManager::getPointer
@exception std::bad_alloc Thrown by StorageManager::getPointer

@return	A pointer to the refered variable.
 */

template <typename T>
T* StorageVariable<T>::operator->()
{
	std::map<unsigned long, StorageManager::StorVarCounter>::iterator it = StorageManager::storVars.find(this->id);
	if (it != StorageManager::storVars.end())
		return StorageManager::getPointer<T>(it->second.name);
	else
		return NULL;
}

#endif
