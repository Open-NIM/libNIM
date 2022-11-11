#include <cstdio>
#include <iostream>
#include <cstdio>
#include "serial.h"
#include "libNIM.h"

#include <thread>
#include <chrono>
#include <iomanip>

int main(){
	/* serial::Serial a{"/dev/ttyUSB0"}; */
	/* std::string buff{}; */
	/* while(true){ */
	/* 	/1* for(int i{}; i<500000; ++i) asm("nop"); *1/ */
	/* 	std::cout << a.readline(10); */
	/* } */
	auto ls{serial::list_ports()};
	for(auto i : ls) std::cout << i.port << ":\t" << i.description << "\t[" << i.hardware_id << "]\n";
		
	auto avlMods{NIM::listAvailableModules()};
	std::cout << std::setw(25) << std::left << "Module Type" << std::setw(25) << std::left << "Serial Number:" << "\n";
	std::cout << std::setw(50) << "================================================" << "\n";
	std::cout.clear();
	for(auto i : avlMods){
		std::cout << std::setw(25) << std::left << NIM::typeStr(i.type)
		<< std::setw(25) << std::left << i.serialNumber << "\n";
	}

	/* auto lssm{NIM::listSpecificModules<NIM::Counter>()}; */
	/* for(auto& i : lssm){ */
	/* 	std::cout << std::setw(25) << std::left << NIM::typeStr(i.getType()) */
	/* 	<< std::setw(25) << std::left << i.getSerialNumber() << "\n"; */
	/* } */

	/* std::cout << (char) avlMods[0]->send(NIM::Counter::RSTA) << "\n"; */
	/* NIM::Module_ptr t{std::make_shared<NIM::Module_base>(NIM::counter, "test", 0)}; */
	/* NIM::Module_base t2{NIM::counter, "/dev/ttyS0", 0}; */
	
	//TO TEST WITH STK500V2 BOOTLOADER ON ARDUINO MEGA 
	/* uint8_t bootloaderm[2]{0x31, 0x80}; */
	/* std::vector<uint8_t> stk500v2{0x30, 0x80}; */
	/* serial::Serial t0{"/dev/ttyACM1"}; */
	/* std::this_thread::sleep_for(std::chrono::milliseconds(1000)); */
	/* t0.setDTR(); */
	/* t0.setDTR(false); */
	/* t0.setDTR(); */
	/* /1* std::this_thread::sleep_for(std::chrono::milliseconds(400)); *1/ */
	/* t0.flushInput(); */
	/* t0.flushOutput(); */
	/* while(!t0.waitReadable()){ */
	/* 	t0.write(bootloaderm, 1); */
	/* 	std::this_thread::sleep_for(std::chrono::milliseconds(500)); */
	/* 	/1* std::cout << t0.available() << " "; *1/ */	
	/* } */
	/* std::string response{t0.read(7)}; */
	/* std::printf("%s\n", response.data()); */
	/* std::this_thread::sleep_for(std::chrono::seconds(1)); */

		
	/* try{ */
	/* 	NIM::Counter t3{"/dev/ttyUSB0", 0}; */
	/* 	for(int i{}; i<10; i++){ */
	/* 		try{ */
	/* 			std::cout << i << ": " << t3.send(NIM::Counter::GET1) << "\n"; */
	/* 			/1* t3.send(NIM::Counter::GET1); *1/ */
	/* 		} */
	/* 		catch(const std::exception &e){std::cout << e.what() << "\n";}; */
	/* 	} */
	/* } */
	/* catch(const std::exception & e){ */
	/* 	std::cout << "\terror" << "\n"; */
	/* } */
	/* catch(const char * e){ */
	/* 	std::cout << e << "\n"; */
	/* } */
	return 0;
}
