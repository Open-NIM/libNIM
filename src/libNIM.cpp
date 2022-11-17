/* Copyright (C) 2022 Alluringbits */
#include "libNIM.h"

#include <thread>
#include <chrono>

namespace NIM{

	uint32_t string_to_uint32_t(const std::string &s) noexcept;
	uint64_t string_to_uint64_t(const std::string &s) noexcept;
	
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

	std::vector<NIM::ModuleInfo> listAvailableModules(NIM::moduleType t, bool force){
		std::vector<NIM::ModuleInfo> lsm;
		auto lsp{serial::list_ports()};
		serial::Serial tmpS{"", 9600, serial::Timeout{0,1000,4}};
		for(auto i : lsp){
			//do: force check to send serial number request to every serial port in case all modules aren't found
			//do:  
			if(force || i.description.find("rduino") != std::string::npos){
				try{
					tmpS.setPort(i.port);
					tmpS.open();
					uint64_t num{send_serialNumber_request(tmpS,5)};
					tmpS.close();
					switch(num){
						case 0:
							if(force) lsm.emplace_back(NIM::ModuleInfo{i.port, NIM::unknown, 0});
							break;
						case 1:
							//this looks extremely bad in style, look for a way to change this into something better. User will have to use NIM::universal when listing every single module and using force
							if(t == NIM::counter || t == NIM::universal) lsm.emplace_back(NIM::ModuleInfo{i.port, NIM::counter, num});
							break;
						default:
							break;
					}
				}
				catch(const serial::IOException &e){
					if(force){
						lsm.emplace_back(NIM::ModuleInfo{i.port, NIM::unknown, 0});
					}
				}
				catch(const NIM::MaxCommunicationAttempts & e){
					if(force){
						lsm.emplace_back(NIM::ModuleInfo{i.port, NIM::unknown, 0});
					}
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
			try{
				psp.flushInput();
				psp.write(out);
				/* while(!(sp.waitReadable())); */
				buff = psp.readline(10);
			}
			catch(const serial::IOException &e){
				throw ModuleUnreachable{psp.getPort()};
			}
			catch(const std::exception &e){
				throw UnexpectedException{e};
			}
			if((buff.find("ERR!") == std::string::npos) && ((buff.size()-1) == 8)) return string_to_uint64_t(buff);
		}
		throw MaxCommunicationAttempts{psp.getPort()}; 
	}

	uint32_t string_to_uint32_t(const std::string &s) noexcept {
		if(s.size() <4) return 0;
		return uint32_t(0) | ((s[0] & 255)  << 24) | ((s[1] & 255) << 16) | (( s[2] & 255) << 8) | (s[3] & 255);
	}

	uint64_t string_to_uint64_t(const std::string &s) noexcept {
		if(s.size() < 8) return 0;
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
				/* while(!(sp->waitReadable())); */
				/* std::cout << sp.available() << ":"; */
				//MODIFY 10 to be size
				buff = sp->readline(100);
				/* std::cout << std::atoi(buff.c_str()) << "\n"; */
				std::cout << buff.size() << ": ";
				for(auto i : buff) std::printf("%x ", i&255);
				/* std::cout << buff; */
				std::cout << "\t" << string_to_uint32_t(buff) << "\n";
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
	
	std::string typeStr(moduleType t) noexcept {
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

