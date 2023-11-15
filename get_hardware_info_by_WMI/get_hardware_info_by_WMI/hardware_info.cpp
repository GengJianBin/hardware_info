#include "hardware_info.h"
#include <comdef.h>
#include <iostream>
#include <sstream>

#pragma comment(lib,"wbemuuid.lib")

hardware_info* hardware_info::sta_instance = nullptr;

hardware_info::hardware_info() {
	init_hardware_info();
}

hardware_info* hardware_info::CreateInstance()
{
	if (!sta_instance) {
		sta_instance = new hardware_info();
		return sta_instance;
	}
	else {
		return sta_instance;
	}
}

void hardware_info::DestroyInstance(hardware_info* instance)
{
	if (instance) {
		delete instance;
		instance = nullptr;
	}
}

hardware_info::~hardware_info()
{
	m_pEnumerator->Release();
	m_pSvc->Release();
	m_pWbemlocator->Release();
	CoUninitialize();
}

void hardware_info::init_hardware_info()
{
	//1.init com runtime
	HRESULT hr = CoInitializeEx(nullptr,COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		std::cout << "CoInitializeEx failed.\r\n";
	}

	//2.create WbemLocator com interface intance
	hr = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,IID_IWbemLocator,(LPVOID*)&m_pWbemlocator);
	if (FAILED(hr)) {
		std::cout << "CoCreateInstance failed.\r\n";
		//CoUninitialize();
	}

	//3.create WMI service connect
	hr = m_pWbemlocator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),nullptr,nullptr,nullptr,0, nullptr,nullptr,&m_pSvc);
	if (FAILED(hr)) {
		std::cout << "ConnectServer failed.\r\n";
		//m_pWbemlocator->Release();
		//CoUninitialize();
	}
	//4.set security level
	hr = CoSetProxyBlanket(m_pSvc,RPC_C_AUTHN_WINNT,RPC_C_AUTHZ_NONE,nullptr,RPC_C_AUTHN_LEVEL_CALL,RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE);
	if (FAILED(hr)) {
		std::cout << "CoSetProxyBlanket failed.\r\n";
		//m_pSvc->Release();
		//m_pWbemlocator->Release();
		//CoUninitialize();
	}
	

}

std::map<std::wstring, std::wstring> hardware_info::query_hardware_info(std::wstring hardware_name,std::vector<std::wstring> items)
{
	m_mapHardwareInfo.clear();
	//1.select 
    std::wstring str_query_statement(L"SELECT ");
	if (items.size() > 0) {
		for (int i = 0; i < items.size();i++) {
			str_query_statement.append(items[i]);
			if(i < items.size() - 1){
				str_query_statement.append(L",");
			}
		}
	}
	else {
		str_query_statement.append(L"*");
	}
	str_query_statement.append(L" FROM ");
	//std::wstring str_query_statement(L"SELECT * FROM ");
	str_query_statement.append(hardware_name);
	HRESULT hr = m_pSvc->ExecQuery(bstr_t(L"WQL"), bstr_t(str_query_statement.c_str()), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr,&m_pEnumerator);
	if (FAILED(hr)) {
		std::cout << "ExecQuery failed.\r\n";
		//m_pWbemlocator->Release();
		//CoUninitialize();
		return m_mapHardwareInfo;
	}
	//2.transverse the result of the querying
	IWbemClassObject * pObject = nullptr;
	ULONG uReturn = 0;
 	while (m_pEnumerator) {
		hr = m_pEnumerator->Next(/*5000*/WBEM_INFINITE,1,&pObject,&uReturn);//need async
		if (uReturn == 0) {
			break;
		}
		VARIANT vtProp;
		VariantInit(&vtProp);
		std::wstring itemValue;
		std::wstringstream ss{};
		for (auto item : items) {
			hr = pObject->Get(item.c_str(), 0, &vtProp, 0, 0);
			if (SUCCEEDED(hr)) {
				//std::cout << "[info] variant type:" << vtProp.vt << std::endl;
				if (vtProp.vt == VARENUM::VT_I2) {
					itemValue = std::to_wstring(vtProp.iVal);
				}
				else if (vtProp.vt == VARENUM::VT_UI2) {
					itemValue = std::to_wstring(vtProp.uiVal);
				}
				else if (vtProp.vt == VARENUM::VT_INT) {
					itemValue = std::to_wstring(vtProp.intVal);
				}
				else if (vtProp.vt == VARENUM::VT_UINT) {
					itemValue = std::to_wstring(vtProp.uintVal);
				}
				else if (vtProp.vt == VARENUM::VT_I8) {
					itemValue = std::to_wstring(vtProp.llVal);
				}
				else if (vtProp.vt == VARENUM::VT_UI8) {
					itemValue = std::to_wstring(vtProp.ullVal);
				}
				else if (vtProp.vt == VARENUM::VT_I4) {
					itemValue = std::to_wstring(vtProp.lVal);
				}
				else if (vtProp.vt == VARENUM::VT_UI4) {
					itemValue = std::to_wstring(vtProp.ulVal);
				}
				else if (vtProp.vt == VARENUM::VT_BSTR /*|| vtProp.vt == VARENUM::VT_LPSTR || vtProp.vt == VARENUM::VT_LPWSTR*/) {
					itemValue = (bstr_t)vtProp.bstrVal;
				}
				else if (vtProp.vt == VARENUM::VT_NULL) {
					std::wcout << L"[error]:item " << item.c_str() << L"don't be rettived, variant type:" << vtProp.vt << std::endl;
					continue;
				}
				else {
					std::wcout <<L"[error]:item " << item.c_str() << L"don't identified, variant type is VARENUM::VT_NULL." << std::endl;
					continue;
				}
				m_mapHardwareInfo[item] = itemValue;
				ss << item << ":" << m_mapHardwareInfo[item] << "\n";
				VariantClear(&vtProp);
			}
			else {
				std::cout << "retrive " << item.c_str() << "item failed." << std::endl;
			}
		}
		std::wcout << ss.str();
		pObject->Release();
	}
	return m_mapHardwareInfo;
}
