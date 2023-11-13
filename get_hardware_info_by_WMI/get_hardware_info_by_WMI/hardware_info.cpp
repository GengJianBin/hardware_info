#include "hardware_info.h"
#include <comdef.h>
#include <iostream>

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

std::string hardware_info::query_hardware_info(std::wstring hardware_name)
{
	//1.select 
    std::wstring str_query_statement(L"SELECT * FROM ");
	str_query_statement.append(hardware_name);
	HRESULT hr = m_pSvc->ExecQuery(bstr_t(L"WQL"), bstr_t(str_query_statement.c_str()), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr,&m_pEnumerator);
	if (FAILED(hr)) {
		std::cout << "ExecQuery failed.\r\n";
		//m_pWbemlocator->Release();
		//CoUninitialize();
	}
	//2.transverse the result of the querying
	IWbemClassObject * pObject = nullptr;
	ULONG uReturn = 0;
 	while (m_pEnumerator) {
		hr = m_pEnumerator->Next(0,1,&pObject,&uReturn);
		if (uReturn == 0) {
			break;
		}
		VARIANT vtProp;
		hr = pObject->Get(L"Name",0,&vtProp,0,0);
		if (SUCCEEDED(hr)) {
			std::wcout << "Processor:" << vtProp.bstrVal << std::endl;
			VariantClear(&vtProp);
		}
		pObject->Release();
	}
	return std::string();
}
