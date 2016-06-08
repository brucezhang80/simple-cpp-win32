#include "StorageManager.h"
#include <iostream>
using namespace std;

class TestClass
{
public:
	StorageVariable<int> r;

	TestClass()
	{
		StorageManager::getVariable<int>("r", true, 11);
		this->r = StorageVariable<int>("r", false);
	}
	~TestClass()
	{
		StorageManager::removeVariable("r");
	}
};

int main()
{
	try
	{
		cout << StorageManager::getLoadProtocol() << endl;

		TestClass *test = StorageManager::getVariable<TestClass>("store");

		cout << *(test->r) << endl << "Enter new val";
		cin >> *(test->r);

		return 0;
	}
	catch (exception e)
	{
		cout << e.what() << endl;
		cin.ignore();
		return 1;
	}
}