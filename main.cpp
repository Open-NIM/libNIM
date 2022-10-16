#include <cstdio>
#include <iostream>
#include "serial.h"

int main(){
	serial::Serial a{};
	auto ls{serial::list_ports()};
	for(auto i : ls) std::cout << i.port << ":\t" << i.description << "\t[" << i.hardware_id << "]\n";
	return 0;
}
