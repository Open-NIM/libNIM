/* Copyright (C) 2022 Alluringbits */
#include "libNIM.h"

namespace NIM{
	
	std::vector<std::unique_ptr<NIM::Module>> listAvailableModules(){
		std::vector<std::unique_ptr<NIM::Module>> lsm;
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
							lsm.emplace_back(std::make_unique<NIM::Counter>(i.port, num));
							break;
						default:
							break;
					}
				}
				catch(const std::exception &e){
					std::cout << e.what() << "\n";
					//serial error checking and rethrowing with custom NIM-specific errors
					//do: create std::exception NIM classes 

				}
			}
		}

		return lsm;
	}
	
	uint64_t send_serialNumber_request(serial::Serial &sp){
		static std::string out{'S','R','L','N'};
		std::string buff;
		sp.flush();
		int attempts{};
		do{
			//5 attempts to send the message
			if(attempts >= 5){
				throw /* error */;
			}
			sp.write(out);
			buff = sp.readline(10);
		}while(!(buff.compare(0,4, "ERR!")));
		return string_to_uint64_t(buff);
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
		std::string tmp{send_and_check(cmdList[cmd])};
		if(tmp.size() != 1) throw /* */; /*error in the improbable eventuality the return value is different than expected (e.g. a 3byte+endline value is obtained instead of an expected 8 bytes +endline value).*/
		return tmp[0];
	}

	uint32_t Counter::send(const NIM::Counter::command_noargs_32ret cmd){
		std::string tmp{send_and_check(cmdList[cmd])};
		if(tmp.size() != 4) throw /* */;
		return string_to_uint64_t(tmp);
	}

	uint64_t Counter::send(const NIM::Counter::command_noargs_64ret cmd){
		std::string tmp{send_and_check(cmdList[cmd])};
		if(tmp.size() != 8) throw /* */;
		return string_to_uint64_t(tmp);
	}
	
	std::string Module::send_and_check(const std::string &s){
		sp.flush();
		int attempts{};
		std::string buff;
		do{
			//5 attempts to send the message
			if(attempts >= 5){
				throw /* error */;
			}
			try{
				sp.write(s);
				sp.readline(buff, 10);
			}
			catch(const std::exception &e){
				throw e;
			}
		}while(!(buff.compare(0,4, errstr)));
		return buff;
	}


	/* void Module::send_universal(const universalCmd_noargs cmd) const{ */
	/* 	switch(cmd){ */
	/* 		case universalCMD_noargs::SRLN: */


	/* } */
	
	/* int64_t send(serial::Serial& sp, NIM::commands cmd, std::vector<uint8_t> content){ */
	/* 	switch(cmd){ */
	/* 		case NIM::commands::RSTA: */
				
	/* 			break; */
	/* 		case NIM::commands::RST1: */
	/* 			break; */
	/* 		case NIM::commands::RST2: */
	/* 			break; */
	/* 		case NIM::commands::SRLN: */
	/* 			break; */
	/* 		case NIM::commands::GET1: */
	/* 			break; */
	/* 		case NIM::commands::GET2: */
	/* 			break; */
	/* 		case NIM::commands::EN_1: */
	/* 			break; */
	/* 		case NIM::commands::EN_2: */
	/* 			break; */
	/* 		default: */
	/* 			break; */
	/* 	} */
	/* } */


}

