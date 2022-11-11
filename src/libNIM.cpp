/* Copyright (C) 2022 Alluringbits */
#include "libNIM.h"

#include <thread>
#include <chrono>

namespace NIM{

	uint64_t send_serialNumber_request(serial::Serial &sp, uint16_t N= 5);
	uint32_t string_to_uint32_t(const std::string &s);
	uint64_t string_to_uint64_t(const std::string &s);
	
	Module_base::Module_base(const NIM::moduleType t, const std::string pi, uint64_t srl) :  tp{t}, serialNumber{srl}{
		try{
			sp->setPort(pi);
			sp->open();
		}
		catch(const serial::IOException & e){
			throw ModuleUnreachable{sp->getPort()};
		}catch(const std::exception & e){
			throw UnexpectedException{e};
		}
	}

	std::vector<NIM::ModuleInfo> listAvailableModules(){
		std::vector<NIM::ModuleInfo> lsm;
		auto lsp{serial::list_ports()};
		serial::Serial tmpS{"", 9600, serial::Timeout{0,1000,4}};
		for(auto i : lsp){
			//do: force check to send serial number request to every serial port in case all modules aren't found
			//do:  
			if(i.description.find("rduino") != std::string::npos){
				try{
					tmpS.setPort(i.port);
					tmpS.open();
					uint64_t num{send_serialNumber_request(tmpS,5)};
					tmpS.close();
					switch(num){
						case 0:
							lsm.emplace_back(NIM::ModuleInfo{i.port, NIM::counter, num});
							break;
						default:
							break;
					}
				}
				catch(const serial::IOException &e){
					throw ModuleUnreachable{i.port};
				}
				catch(const NIM::MaxCommunicationAttempts & e){
					throw e;
				}
				catch(const std::exception &e){
					throw UnexpectedException{e};
				}
			}
		}
		
		return lsm;
	}
	
	uint64_t send_serialNumber_request(serial::Serial &psp, uint16_t N){
		static std::string out{'S','R','L','N'};
		std::string buff;
		for(int attempts{}; attempts< N; ++attempts){
			/* std::this_thread::sleep_for(std::chrono::milliseconds(2000)); */
			psp.flushInput();
			psp.write(out);
			/* while(!(sp.waitReadable())); */
			buff = psp.readline(10);
			return 0;
			if((buff.find("ERR!") == std::string::npos) && ((buff.size()-1) == 8)) return string_to_uint64_t(buff);
		}
		throw MaxCommunicationAttempts{psp.getPort()}; 
	}

	uint32_t string_to_uint32_t(const std::string &s){
		return uint32_t(0) | (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | (s[3]);
	}

	uint64_t string_to_uint64_t(const std::string &s){
		uint64_t t{};
		for(int i{}; i<8; ++i) (t | ( s[i] << (64-8*(i+1))));
		return t;
	}

	bool Counter::send(const NIM::Counter::command_noargs_bool cmd){
		return send_and_check(cmdList[cmd], 1)[0];
	}

	uint32_t Counter::send(const NIM::Counter::command_noargs_32ret cmd){
		return string_to_uint32_t(send_and_check(cmdList[cmd], 4));
	}

	uint64_t Counter::send(const NIM::Counter::command_noargs_64ret cmd){
		return string_to_uint64_t(send_and_check(cmdList[cmd], 8));
	}

	
	std::string Module_base::send_and_check(const std::string &s, const uint8_t size, uint16_t N ){
		std::string buff{};
		/* std::cout << s << "\n"; */
		//5 default/N attempts to send the message
		for(int attempts{}; attempts<N; ++attempts){
			try{
				sp->flushInput();
				sp->write(s);
				/* while((sp.waitReadable())); */
				/* std::cout << sp.available() << ":"; */
				//MODIFY 10 to be size
				buff = sp->readline(10);
			}
			catch(const serial::IOException &e){
				throw ModuleUnreachable{sp->getPort(), tp, serialNumber, s};
			}
			catch(const std::exception &e){
				throw UnexpectedException{e};
			}
			if((buff.find(errstr) == std::string::npos) && ((buff.size()-1) == size)) return buff;
		}
		throw MaxCommunicationAttempts{sp->getPort(), tp, serialNumber, s}; 
	}
	
	std::string typeStr(moduleType t){
		switch(t){
			case counter:
				return "Counter";
				break;
			case universal:
				return "Universal";
				break;
			case unknown:
				return "Unknown";
				break;
			default:
				return "";
				break;
		}
	}
}

