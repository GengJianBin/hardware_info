#pragma once

#include <Wbemidl.h>
#include <string>

class hardware_info
{
public:
	static hardware_info*  CreateInstance();
	static void DestroyInstance(hardware_info*);
	
	~hardware_info();
	std::string query_hardware_info(std::wstring hardware_name);

private:
	hardware_info();
	void init_hardware_info();

	IWbemLocator* m_pWbemlocator = nullptr;
	IWbemServices* m_pSvc = nullptr;
	IEnumWbemClassObject* m_pEnumerator = nullptr;
	static hardware_info* sta_instance;
};