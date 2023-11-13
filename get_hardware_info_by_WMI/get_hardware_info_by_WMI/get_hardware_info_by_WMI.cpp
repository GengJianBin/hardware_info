// get_hardware_info_by_WMI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "hardware_info.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

int main()
{
    std::cout << "Hello World!\n";
    hardware_info* hi = hardware_info::CreateInstance();
    hi->query_hardware_info(L"Win32_Processor");
    std::this_thread::sleep_for(200ms);
    hardware_info::DestroyInstance(hi);
}
