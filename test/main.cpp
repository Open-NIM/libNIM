#include <cstdio>
#include <iostream>
#include <cstdio>
#include "serial.h"
#include "libNIM.h"

#include <thread>
#include <chrono>

int main(){
	/* serial::Serial a{"/dev/ttyUSB0"}; */
	/* std::string buff{}; */
	/* while(true){ */
	/* 	/1* for(int i{}; i<500000; ++i) asm("nop"); *1/ */
	/* 	std::cout << a.readline(10); */
	/* } */
	/* auto ls{serial::list_ports()}; */
	/* for(auto i : ls) std::cout << i.port << ":\t" << i.description << "\t[" << i.hardware_id << "]\n"; */
		
	/* auto avlMods{NIM::listAvailableModules()}; */
	/* for(auto i : avlMods) std::cout << i->getSerialNumber() << "\n"; */
	/* std::cout << avlMods.size() << "\n"; */

	/* NIM::Module_ptr t{std::make_shared<NIM::Module_base>(NIM::counter, "test", 0)}; */
	/* NIM::Module_base t2{NIM::counter, "/dev/ttyS0", 0}; */
	
	//TO TEST WITH STK500V2 BOOTLOADER ON ARDUINO MEGA 
	/* uint8_t bootloaderm[2]{0x03, 0x80}; */
	/* serial::Serial t0{"/dev/ttyUSB0"}; */
	/* std::this_thread::sleep_for(std::chrono::seconds(4)); */
	/* t0.flushInput(); */
	/* t0.flushOutput(); */
	/* t0.setDTR(); */
	/* t0.setDTR(false); */
	/* t0.setDTR(); */
	/* t0.write(bootloaderm, 1); */
	/* t0.write(bootloaderm+1, 1); */
	/* std::string response{t0.read(1)}; */
	/* std::printf("%x\n", response[0]); */
	/* std::this_thread::sleep_for(std::chrono::seconds(1)); */

		
	try{
		NIM::Counter t3{"/dev/ttyUSB0", 0};
		for(int i{}; i<10; i++){
			try{
				std::cout << i << ": " << t3.send(NIM::Counter::GET1) << "\n";
				/* t3.send(NIM::Counter::GET1); */
			}
			catch(const std::exception &e){std::cout << e.what() << "\n";};
		}
	}
	catch(const std::exception & e){
		std::cout << "\terror" << "\n";
	}
	catch(const char * e){
		std::cout << e << "\n";
	}
	return 0;
}
