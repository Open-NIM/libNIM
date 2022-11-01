/* Copyright (C) 2022 Alluringbits */
#include "libNIM.h"

namespace NIM{
	
	Module_base::Module_base(const NIM::moduleType t, const std::string pi, uint64_t srl) :  tp{t}, serialNumber{srl}{
		try{
			sp.setPort(pi);
			sp.open();
		}
		catch(const serial::IOException & e){
			throw ModuleUnreachable{sp.getPort()};
		}catch(const std::exception & e){
			throw UnexpectedException{e};
		}
	}

	std::vector<std::shared_ptr<NIM::Module_base>> listAvailableModules(){
		std::vector<std::shared_ptr<NIM::Module_base>> lsm;
		auto lsp{serial::list_ports()};
		serial::Serial tmpS{};
		for(auto i : lsp){
			//do: force check to send serial number request to every serial port in case all modules aren't found
			//do:  
			if(i.description.find("rduino") != std::string::npos){
				try{
					tmpS.setPort(i.port);
					tmpS.open();
					uint64_t num{send_serialNumber_request(tmpS)};
					tmpS.close();
					switch(num){
						case 0:
							lsm.emplace_back(std::make_shared<NIM::Counter>(i.port, num));
							break;
						default:
							break;
					}
				}
				catch(const serial::IOException &e){
					throw ModuleUnreachable{i.port};
					//serial error checking and rethrowing with custom NIM-specific errors
					//do: create std::exception NIM classes 

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
	
	uint64_t send_serialNumber_request(serial::Serial &sp, uint16_t N){
		static std::string out{'S','R','L','N'};
		std::string buff;
		for(int attempts{}; attempts< N; ++attempts){
			sp.flushInput();
			sp.write(out);
			while(!(sp.waitReadable()));
			buff = sp.readline(10);
			if((buff.find("ERR!") == std::string::npos) && (buff.size() == 8)) return string_to_uint64_t(buff);
		}
		throw MaxCommunicationAttempts{sp.getPort()}; 
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

	/* uint64_t Counter::send(const NIM::Counter::command_noargs_64ret cmd){ */
	/* 	std::string tmp{send_and_check(cmdList[cmd])}; */
	/* 	if(tmp.size() == 8) return string_to_uint64_t(tmp); */
	/* 	for(int att{}; att<5; ++att){ */
	/* 		tmp = send_and_check(cmdList[cmd]); */
	/* 		if(tmp.size() == 8) return string_to_uint64_t(tmp); */
	/* 	} */
	/* 	throw MaxCommunicationAttempts{sp.getPort(), tp, serialNumber, cmdList[cmd]}; */ 
	/* } */

	

	/* uint64_t Counter::send(const NIM::Counter::command_noargs_64ret cmd){ */
	/* 	std::string tmp{}; */
	/* 	int attempts{}; */
	/* 	do{ */
	/* 		if(attempts >= 5) throw MaxCommunicationAttempts{sp.getPort(), tp, serialNumber}; */ 
	/* 		tmp = send_and_check(cmdList[cmd]); */
	/* 		++attempts; */
	/* 	}while(tmp.size() != 8); */
	/* 	return string_to_uint64_t(tmp); */
	/* } */
	
	std::string Module_base::send_and_check(const std::string &s, const uint8_t size, uint16_t N ){
		std::string buff{};
		/* std::cout << s << "\n"; */
		//5 default/N attempts to send the message
		for(int attempts{}; attempts<N; ++attempts){
			try{
				sp.flushInput();
				sp.write(s);
				while(!(sp.waitReadable()));
				/* std::cout << sp.available() << ":"; */
				buff = sp.readline(10);
			}
			catch(const serial::IOException &e){
				throw ModuleUnreachable{sp.getPort(), tp, serialNumber, s};
			}
			catch(const std::exception &e){
				throw UnexpectedException{e};
			}
			if((buff.find(errstr) == std::string::npos) && (buff.size() == size)) return buff;
		}
		throw MaxCommunicationAttempts{sp.getPort(), tp, serialNumber, s}; 
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

