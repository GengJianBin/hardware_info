#pragma once

#include <Wbemidl.h>
#include <string>
#include <vector>
#include <map>

class hardware_info
{
public:
	static hardware_info*  CreateInstance();
	static void DestroyInstance(hardware_info*);
	
	
	~hardware_info();
	std::map<std::wstring, std::wstring> query_hardware_info(std::wstring hardware_name, std::vector<std::wstring> items);

private:
	hardware_info();
	void init_hardware_info();

	IWbemLocator* m_pWbemlocator = nullptr;
	IWbemServices* m_pSvc = nullptr;
	IEnumWbemClassObject* m_pEnumerator = nullptr;
	//https://learn.microsoft.com/zh-cn/windows/win32/cimwin32prov/win32-processor
	std::map<std::wstring, std::wstring> m_mapHardwareInfo;
	static hardware_info* sta_instance;
};