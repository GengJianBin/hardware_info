// get_hardware_info_by_WMI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "hardware_info.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

int main()
{
	//std::wcout.imbue(std::locale(std::locale(),"",LC_CTYPE));加上后std::wcout和std::cout都可以输出中文.std::cout不加L说明符
	std::wcout.imbue(std::locale("chs"));//加上后std::wcout和std::cout都可以输出中文
    std::wcout << L"Hello World!\n";
    //std::wcout << L"你好!\n";
    hardware_info* hi = hardware_info::CreateInstance();

	std::vector<std::wstring> m_vecCPUItems = {
		L"ProcessorId",
		L"Name",
		L"NumberOfCores",
		L"NumberOfLogicalProcessors",
		L"SystemName",
		L"ProcessorType",
		L"MaxClockSpeed",
		L"CurrentClockSpeed",
		L"Architecture",
		L"Availability"
	};
	std::vector<std::wstring> m_vecPysicalMemoryItems = {
		L"Capacity",
		L"Caption",
		L"ConfiguredClockSpeed",
		L"MemoryType",
		L"Name",
		L"Speed"
	};
	std::vector<std::wstring> m_vecGPUItems = {
		L"AdapterRAM",
		L"Name",
		L"Description",
		L"MaxMemorySupported",
		L"MaxRefreshRate",
		L"MinRefreshRate",
		L"VideoMode",
		L"VideoProcessor",
		L"CurrentRefreshRate",
		L"CurrentVerticalResolution"
	};

    std::vector<std::wstring> vec;
    hi->query_hardware_info(L"Win32_Processor", m_vecCPUItems);
    hi->query_hardware_info(L"Win32_PhysicalMemory", m_vecPysicalMemoryItems);
    hi->query_hardware_info(L"Win32_VideoController ", m_vecGPUItems);
    std::this_thread::sleep_for(200ms);
    hardware_info::DestroyInstance(hi);
}
